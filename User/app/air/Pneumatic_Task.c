#include <rtthread.h>
#include "includes.h"

#include "Data_Task.h"
#include "bsp_vitCtl.h"
#include "Pneumatic_Task.h"


#define LOG_TAG            "task.Pneumatic"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>


#define     PNEUMATIC_TASK_SIZE       512
#define     PNEUMATIC_TASK_PRIO       18


rt_uint8_t  pneumatic_task_stack[PNEUMATIC_TASK_SIZE];
static struct rt_thread pneumatic_tcb;  // 任务句柄
static struct rt_semaphore pneumatic_rx_sem;

Pneumatic_Task_t  _pneumatic = {0};



//波切换相时的操作函数
void Pneumatic_SwitchDir(uint8_t index)
{
    static struct {
        float _up_adc[5];
        float _down_adc[5];
        uint8_t _upindex;
        uint8_t _downindex;
    }air_adc={0};
    PressureSensor_t *sensor = &_pneumatic.pressureSensorUp;
    sensor += index;
    _pneumatic._data->_vitmaxpressure[index] =bsp_prsureSen_GetPressure(sensor);

    //0:下限值16000  1:下限值 22500

    if(index ==0)
    {
        air_adc._up_adc[air_adc._upindex] = _pneumatic._data->_vitmaxpressure[index];  //获取上传感器的adc值
        air_adc._upindex++;
        if (air_adc._upindex == 5)
        {
            float _upsensor =  bsp_mid_filt_f(air_adc._up_adc,5);
            if (_upsensor < 16000) //上传感器压力小于下限值
            {
                // bsp_error_list_add(AIR_LOW_PRESSURE);   //气压低于最低值
                _pneumatic._data->_errCode = AIR_LOW_PRESSURE;
                rt_sem_release(&pneumatic_rx_sem);
            }
            air_adc._upindex = 0;
        }
        
    }
    else if(index ==1)
    {
        air_adc._down_adc[air_adc._downindex] = _pneumatic._data->_vitmaxpressure[index];  //获取下传感器的adc值
        air_adc._downindex++;

        if (air_adc._downindex == 5)
        {
            float _downsensor =  bsp_mid_filt_f(air_adc._down_adc,5);
            if (_downsensor < 23000) //下传感器压力小于下限值
            {
                // bsp_error_list_add(AIR_LOW_PRESSURE);   //气压低于最低值
                _pneumatic._data->_errCode = AIR_LOW_PRESSURE;
                rt_sem_release(&pneumatic_rx_sem);
            }
            air_adc._downindex = 0;
        }
    }
    else
    {

    }
}


static void _Pump_Current_Check()
{
	//这里检测电机速度
    static struct {
        uint16_t _pump_adc[11];
        uint8_t _index;
        float _pump_adc_mid;
    }pump_adc={0};
    pump_adc._pump_adc[pump_adc._index] = bsp_get_valve(AIR_PUMB_ADC_CH);  //获取泵的adc值
    pump_adc._index++;
    if(pump_adc._index==11)
    {
        pump_adc._pump_adc_mid = bsp_mid_filt16(pump_adc._pump_adc,11);  //获取中值
        pump_adc._index =0;
        if(pump_adc._pump_adc_mid  >45000)  
        {
            LOG_E("Pump_Current_Check _pump_adc:%d",pump_adc._pump_adc_mid);
            bsp_error_list_add(AIR_PUMP_CURRENT_ERROR);   //气压低于最低值
        }
    }
}

void Pneumatic_CtlCmp(uint16_t cmp)
{
    static uint8_t _pump_current_flag=0;
    bsp_valve_Ctl(VALVE_AIR_PUMB,cmp>0?_pneumatic._cof->_pumbduty:0);   //隔膜泵控制
    Vitctl_SetCutrate(cmp);
    if (_pump_current_flag == 0 && cmp>0) //
    {
        _pump_current_flag = 1;
        Data_Task_ResetTaskTimer(_Pump_Current_Check,200);	//第一次开启后500ms进行检测
    }
    if(cmp==0)
    {
        _pump_current_flag = 0;
        Data_Task_ResetTaskTimer(_Pump_Current_Check,0);
    }
    
}

CalibrationPara SensorUP = {
    .A = 1,
    .B = 0,
    .C = 0,
};

CalibrationPara SensorDown = {
    .A = 1,
    .B = 0,
    .C = 0,
};


void Pneumatic_EnCallback(uint8_t _en)
{
    if (_en)
    {
        // bsp_valve_Ctl(VALVE_AIR_PUMB,_pneumatic._cof->_pumbduty);   //隔膜泵控制
    }
    else
    {
        bsp_valve_Ctl(VALVE_AIR_PUMB,0);  //隔膜泵关闭
        _pneumatic.Vit_CtlCmp(0);
    }
}


static uint32_t _PressureSensor_Read(PressureChannel channel)
{
    return bsp_get_valve(channel==VIT_PRESSURE1?VIT_PRESSURE1_ADC_CH:VIT_PRESSURE2_ADC_CH);
}




static void Pneumatic_LowLevel_Init(void)
{
   
    Vitctl_Init(Pneumatic_SwitchDir);
    _pneumatic.Vit_CtlCmp =Pneumatic_CtlCmp;
    _pneumatic.En_Callback = Pneumatic_EnCallback;

    ramcof.aircof._vitduty  = 58;    //波切占空比58
    ramcof.aircof._pumbduty = 80;    //泵的占空比80
    Vitctl_SetDutyCycle(ramcof.aircof._vitduty);
    //传感器校准
    _pneumatic.pressureSensorUp.channel = VIT_PRESSURE1;
    bsp_prsureSen_Init(&_pneumatic.pressureSensorUp,_PressureSensor_Read); //
    bsp_prsureSen_SetCalib(&_pneumatic.pressureSensorUp,SensorUP);
    _pneumatic.pressureSensorDown.channel = VIT_PRESSURE2;
    bsp_prsureSen_Init(&_pneumatic.pressureSensorDown,_PressureSensor_Read); //
    bsp_prsureSen_SetCalib(&_pneumatic.pressureSensorDown,SensorDown);



    Pneumatic_Task_SetWcof(&ramcof.aircof);
    Pneumatic_Task_SetData(&ramdata.air_data);

    Data_Task_RegisterSensor(&_pneumatic.pressureSensorUp,&_pneumatic._data->_vitrawdata[0],&_pneumatic._data->_vitpressure[0]);
    Data_Task_RegisterSensor(&_pneumatic.pressureSensorDown,&_pneumatic._data->_vitrawdata[1],&_pneumatic._data->_vitpressure[1]);

    Data_Task_RegisterTask(_Pump_Current_Check,0);
    // bsp_valve_Ctl(VALVE_AIR_PUMB,80);
}

static void Pneumatic_Task_UpdateCtlParams()
{
    if (_pneumatic._cof->_en)
    {
        int32_t _sppedrange = (_pneumatic._cof->_end - _pneumatic._cof->_start);
        _pneumatic._data->_ctlValue = _pneumatic._data->_percentofPedal==0?0:((int32_t)((int32_t)(_sppedrange*_pneumatic._data->_percentofPedal)/100)+_pneumatic._cof->_start);
        rt_sem_release(&pneumatic_rx_sem);
    }
    else
    {
         _pneumatic.En_Callback(0);
        _pneumatic._data->_ctlValue = 0;
        _pneumatic._data->_percentofPedal = 0;
    }
    
}

void  Pneumatic_Task_Cmd(uint16_t _addr,uint32_t _data)
{
    if (_addr >=(uint16_t)&_pneumatic._cof->_vitduty && _addr <= (uint16_t)&_pneumatic._cof->_end)   
    {
        if (_addr ==(uint16_t)&_pneumatic._cof->_en)
        {
            _pneumatic.En_Callback(_data);
        }
        else if (_addr == (uint16_t)&_pneumatic._cof->_vitduty)
        {
            Vitctl_SetDutyCycle(_data);
        }
        else if (_addr == (uint16_t)&_pneumatic._cof->_pumbduty)
        {
            bsp_valve_Ctl(VALVE_AIR_PUMB,_data);   //隔膜泵控制
        }
        Pneumatic_Task_UpdateCtlParams();
    }
    else if (_addr >= (uint16_t)&_pneumatic._data->_percentofPedal)
    {
        Pneumatic_Task_UpdateCtlParams();
    }
    else
    {
        
    }
}

int Pneumatic_Task_Init(void)
{
     rt_err_t res;
    Pneumatic_LowLevel_Init();

    rt_sem_init(&pneumatic_rx_sem, "pneumatic_rx", 20,RT_IPC_FLAG_FIFO);
    Normal_Task_RegisterFun(Pneumatic_Task_Cmd);
    Pedal_Task_RegisterFun(Pneumatic_Pedal_Deal);
    /* 创建数据接收线程 */
    res =  rt_thread_init(&pneumatic_tcb,"pneumatic_task",Pneumatic_Task,RT_NULL,pneumatic_task_stack,PNEUMATIC_TASK_SIZE,PNEUMATIC_TASK_PRIO,10);
    RT_ASSERT(res == RT_EOK);
    rt_thread_startup(&pneumatic_tcb);  
    LOG_D("Pneumatic_Task INIT OK!!");
}
INIT_APP_EXPORT(Pneumatic_Task_Init);
void Pneumatic_Task(void *arg)
{
    rt_err_t res;
    static uint32_t _oldctl;
    while (1)
    {
        rt_sem_take(&pneumatic_rx_sem, RT_WAITING_FOREVER);
        if (_oldctl !=_pneumatic._data->_ctlValue)
        {
            _pneumatic.Vit_CtlCmp(_pneumatic._data->_ctlValue);
            _oldctl = _pneumatic._data->_ctlValue;
        }
        if (_pneumatic._data->_errCode != 0)
        {
            bsp_error_list_add(_pneumatic._data->_errCode );   //气压低于最低值
            _pneumatic._data->_errCode = 0;
        }
//        rt_thread_mdelay(500);
    }
}
void Pneumatic_Task_SetWcof(Pneumatic_Task_Cof *_cof)
{
    RT_ASSERT(_cof);
    _pneumatic._cof = _cof;
}
void Pneumatic_Task_SetData(Pneumatic_Task_Data *_data)
{
    RT_ASSERT(_data);
    _pneumatic._data = _data;
}
void Pneumatic_Pedal_Deal(uint16_t _code,uint32_t _data)
{
    uint32_t    _cmd = _pneumatic._cof->_pedalcof;      //获取踏板的配置
    uint8_t  _key =0;   
    uint16_t _duty = (uint16_t) _data;       //获取占空比;
    _code = BSWAP_16(_code);
    if(_code>FOOTBOARD_SWITCH)  //开关占空比混合
    {
        _code -= FOOTBOARD_SWITCH;
    }
    if(_cmd>0&&_cmd<FOOTBOARD_SWITCH)   //踏板形成控制
    {
        switch (_cmd)
        {
            case FOOTBOARD_LINE_ONE:   //响应踏板第一阶段
            case FOOTBOARD_LINE_SECOND: //响应踏板第二阶段
            case FOOTBOARD_LINE_THIRD:
            {

                if(_code>_cmd && _code < FOOTBOARD_SWITCH)   //超过所设置的段就最大输出
                    _duty = 100;
                else
                {
                    _duty = (_code == _cmd)?_data:0;
                }
            }
            break;
            default:
                break;
        }
        if(_code == FOOTBOARD_ZERO) //踏板回到零点
            _duty = 0;
        if(_code>_cmd && _code < FOOTBOARD_SWITCH)   //超过所设置的段就最大输出
            _duty = 100;
        _pneumatic._data->_percentofPedal = (_duty > 100)?100:_duty;  //
        Pneumatic_Task_UpdateCtlParams();
    }
    else if(_cmd > FOOTBOARD_END)   //响应按键
    {
        // Pneumatic_Task_UpdateCtlParams();
    }
    else    //不检测
    {

    }
}

void Pneumatic_Pedal_Disconnect()
{
   if(_pneumatic.En_Callback !=NULL)
   {
       _pneumatic.En_Callback(0);
       _pneumatic._data->_ctlValue = 0;    //清空显示值
   }
}