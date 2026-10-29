/*------------------------------------------------------------------------------
 * @file    DIATHERMY_TASK.C
 * @author  FIRMWARE TEAM
 * @date    2024/07/19 10:10:04
 * @brief   THIS FILE PROVIDES ALL THE FIRMWARE FUNCTIONS.
 * -----------------------------------------------------------------------------
 * @attention

 * THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE
 * CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE
 * TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH
 * HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN
 * CONNECTION WITH PRODUCTS MADE BY CUSTOMERS.
 *
 * <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2>
------------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion  ------------------------------------*/
#define _DIATHERMY_TASK_C_

/* Files includes  -----------------------------------------------------------*/
#include <rtthread.h>
#include "includes.h"
#include "Diathermy_Task.h"
#include "bsp_ws2812b.h"
#include "bsp_m95512.h"
#define LOG_TAG            "task.DiaTask"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>

#define  DIA_RFID1_INDEX        2
#define  DIA_RFID2_INDEX        3

#define DIA_TASK_PRIO   20

//#define DIA_TASK_STK_SIZE    512
#define DIA_TASK_STK_SIZE    256

#define DIA_ACTIVE_COLOR        0X007F00
#define DIA_UNACTIVE_COLOR      0x000000


#define DIA_MAX_OUT         	(10)        //10W
#define DIA_GET_MAX_PWR(X)      ((X*DIA_MAX_OUT)/100.0)

Diathermy_Task_t _diathermy = {0};

OS_TCB Dia_TaskTCB;
CPU_STK Dia_TASK_STK[DIA_TASK_STK_SIZE];

static void _Diathermy_Task_Set_color(uint32_t _color)
{
    LED_COLOR_32(color, LED_COLOR_RGB2GRB(_color));
    // bsp_ws2812b_Ctl_RangeColor(&ws1,0,9,color);
    bsp_ws2812b_Ctl_AllColor(&ws1, color);
    // bsp_ws2812b_Ctl_RangeColor(&ws2,0,4,color);
}

static void _Diathermy_Task_CheckCof()
{
    float _pwr;

    if(_diathermy._cof->_mode == 1)  //固定模式
    {
        // 获取踏板的激活状态
        // _diathermy._pedal.bytes._set_key = (_diathermy._cof->_footcof >> 24);   //获取设置触发按键

        // if(_diathermy._pedal.bytes._set_key == _diathermy._pedal.bytes._active_key) //触发
        // {
        _pwr = DIA_GET_MAX_PWR(_diathermy._cof->_set_valve);
        _diathermy._data->_read_valve = _diathermy._cof->_set_valve;
    }
    else if(_diathermy._cof->_mode == 2)    //线性模式
    {
        _diathermy._data->_read_valve = _diathermy._pedal.bytes._duty>0?(((_diathermy._cof->_set_valve -_diathermy._cof->_set_startValve)  * ((float)_diathermy._pedal.bytes._duty / 100.0f)) + _diathermy._cof->_set_startValve):0;
        _pwr = (DIA_GET_MAX_PWR(_diathermy._data->_read_valve));
        // _pwr = (DIA_GET_MAX_PWR(_diathermy._cof->_set_valve) * _diathermy._pedal.bytes._duty) / 100.0;

    }
    else    //不使能
    {
        _pwr = 0;
        _diathermy._pedal._percentofPedal = 0;
        _diathermy._data->_read_valve = 0;
    }

    if( rt_memcmp(&_pwr, &_diathermy._setpower, 4)) //不相等
    {
        _diathermy._setpower = _pwr;
        bsp_dia_setPower(_diathermy._dia_handler, _diathermy._setpower);
        // _Diathermy_Task_Set_color((_pwr>0)?_diathermy._cof->_work_color:DIA_UNACTIVE_COLOR);
    }
}




int Diathermy_Task_Init(void)
{
    OS_ERR err;
    _diathermy._dia_handler = &_dia_instance;
    _diathermy._errorCode = 0;
		bsp_dia_register(&ramdata._diaCalData);
	
	  Flash_DiaParam_init();
	
    Diathermy_Task_SetWcof(&ramcof.diacof);
    Diathermy_Task_SetData(&ramdata.dia_data);
    _diathermy._data->_conStatus = 1;
    ramcof.diacof._work_color = APP_RGB_BLUE;   //工作颜色默认蓝色
    Pedal_Task_RegisterFun(Diathermy_Pedal_Deal);

    OSTaskCreate( (OS_TCB     *) &Dia_TaskTCB,
                  (CPU_CHAR   *) "Diathermy_task",
                  (OS_TASK_PTR ) Diathermy_Task,
                  (void       *) 0,
                  (OS_PRIO     ) DIA_TASK_PRIO,
                  (CPU_STK    *) &Dia_TASK_STK[0],
                  (CPU_STK_SIZE) DIA_TASK_STK_SIZE / 10,
                  (CPU_STK_SIZE) DIA_TASK_STK_SIZE,
                  (OS_MSG_QTY  ) 10,
                  (OS_TICK     ) 0,
                  (void       *) 0,
                  (OS_OPT      ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                  (OS_ERR     *) &err);
}
INIT_APP_EXPORT(Diathermy_Task_Init);

//是否使能看门狗
#define ENABLE_WATCHDOG                 1
#define WDT_DEVICE_NAME    "wdt"    /* 看门狗设备名称 */
static rt_device_t wdg_dev = RT_NULL; /* 看门狗设备句柄 */
rt_uint32_t timeout =40;        /* 溢出时间，单位：毫秒 */

int8_t _Diathermy_WatchDog_Init()
{
    rt_err_t ret = RT_EOK;

    //初始化看门狗任务
    /* 根据设备名称查找看门狗设备，获取设备句柄 */
    wdg_dev = rt_device_find(WDT_DEVICE_NAME);
    if (!wdg_dev)
    {
        LOG_E("find %s failed!\n", WDT_DEVICE_NAME);
        return RT_ERROR;
    }
    /* 初始化设备 */
     rt_device_init(wdg_dev);
    /* 设置看门狗溢出时间 */
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
    if (ret != RT_EOK)
    {
        LOG_E("set %s timeout failed!\n", WDT_DEVICE_NAME);
        return RT_ERROR;
    }
    /* 启动看门狗 */
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_START, RT_NULL);
    if (ret != RT_EOK)
    {
        LOG_E("start %s failed!\n", WDT_DEVICE_NAME);
        return -RT_ERROR;
    }
}

#define DIA_MAX_VOL_ADC   32937 //83V  
#define DIA_MIN_VOL_ADC   2051 //83V
#define DIA_MAX_CUR_ADC   41000 //83V
#define DIA_MIN_CUR_ADC   500 //83V

static void _Diathermy_CurrentVol_Check()
{
	//这里检测电机速度
    static struct {
        uint16_t _vol_adc[5];
        uint16_t _current_adc[5];
        uint8_t _index;
        float _vol_adc_mid;
        float _current_adc_mid;
    }dia_adc={0};
    dia_adc._vol_adc[dia_adc._index] = bsp_get_valve(V_ADC_CH);  //获取泵的adc值
    dia_adc._current_adc[dia_adc._index] = bsp_get_valve(I_ADC_CH);  //获取泵的adc值
    dia_adc._index++;
    if(dia_adc._index==5)
    {
        dia_adc._vol_adc_mid = bsp_mid_filt16(dia_adc._vol_adc,5);  //获取中值
        dia_adc._current_adc_mid = bsp_mid_filt16(dia_adc._current_adc,5);  //获取中值
        dia_adc._index =0;
        if (dia_adc._vol_adc_mid > DIA_MAX_VOL_ADC)
        {
            _diathermy._errorCode = DIATHERMY_VOL_MAX_ERROR;
            bsp_error_list_add(DIATHERMY_VOL_MAX_ERROR);   //气压低于最低值
        }
        else if (dia_adc._vol_adc_mid < DIA_MIN_VOL_ADC)
        {
            _diathermy._errorCode = DIATHERMY_VOL_MIN_ERROR;
            bsp_error_list_add(DIATHERMY_VOL_MIN_ERROR);   //气压低于最低值
        }
        else
        {
//            _diathermy._errorCode = 0;
        }
        if (dia_adc._current_adc_mid > DIA_MAX_CUR_ADC)
        {

            _diathermy._errorCode = DIATHERMY_CURRENT_MAX_ERROR;
            bsp_error_list_add(DIATHERMY_CURRENT_MAX_ERROR);   //气压低于最低值
    
        }
        else if (dia_adc._current_adc_mid < DIA_MIN_CUR_ADC)
        {
            _diathermy._errorCode = DIATHERMY_CURRENT_MIN_ERROR;
            bsp_error_list_add(DIATHERMY_CURRENT_MIN_ERROR);   //气压低于最低值
        }
        else
        {
//		    _diathermy._errorCode = 0;
        }
    }
}


void Diathermy_Task(void *arg)
{
#if ENABLE_WATCHDOG
    _Diathermy_WatchDog_Init();
#endif 
    while (1)
    {
#if ENABLE_WATCHDOG
        rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
#endif 
        _Diathermy_Task_CheckCof();

        //更新电零功率
        if(_diathermy._dia_handler->_enable)    //输出使能才读
        {
            _diathermy._data->_info =  bsp_dia_getInfo(_diathermy._dia_handler);
            
            if(ramcof._is_dia_error_detected)
            {
                if (_diathermy._data->_read_valve > 2)  //输出大于百分之2才检测
                {
                    _Diathermy_CurrentVol_Check();
                }
                if (_diathermy._errorCode >0)
                {
                    Diathermy_Pedal_Disconnect();
                    _diathermy._errorCode = 0;
                }
            }
        }        
        rt_thread_delay(5);
    }

}
void Diathermy_Task_SetWcof(Diathermy_Task_Cof *_cof)
{
    RT_ASSERT(_cof);
    _diathermy._cof = _cof;
}

void Diathermy_Task_SetData(Diathermy_Task_Data *_data)
{
    RT_ASSERT(_data);
    _diathermy._data = _data;
}

void Diathermy_Pedal_Deal(uint16_t _code, uint32_t _data)
{
    uint32_t _cmd = _diathermy._cof->_footcof;      //获取当前电凝踏板配置
    uint8_t  _key = 0;
    uint16_t _duty = 0;
    _code = BSWAP_16(_code);

    _key  = (uint8_t)(_data >> 24);  //获取按键分量
    _duty = (uint16_t) _data;       //获取占空比

    if(_code > FOOTBOARD_SWITCH) //开关占空比混合
    {
        _code -= FOOTBOARD_SWITCH;
    }

    if(_cmd > 0 && _cmd < FOOTBOARD_SWITCH) //踏板形成控制
    {
        switch (_cmd)
        {
            case FOOTBOARD_LINE_ONE:   //响应踏板第一阶段
            case FOOTBOARD_LINE_SECOND: //响应踏板第二阶段
            case FOOTBOARD_LINE_THIRD:
            {
                if(_code > _cmd && _code < FOOTBOARD_SWITCH) //超过所设置的段就最大输出
                    _duty = 100;
                else
                {
                    _duty = (_code == _cmd) ? _data : 0;
                }
            }
            break;

            default:
                break;
        }

        if(_code == FOOTBOARD_ZERO) //踏板回到零点
            _duty = 0;

        if(_code > _cmd && _code < FOOTBOARD_SWITCH) //超过所设置的段就最大输出
            _duty = 100;
        _diathermy._pedal.bytes._duty = (_duty>100?100:_duty);      //
    }
    else if(_cmd > FOOTBOARD_END)   //响应按键
    {
        _diathermy._pedal.bytes._active_key = _key;
    }
    else    //不检测
    {

    }

}


void Diathermy_Task_SetPwr(uint8_t percent)
{
    float _pwr = 0;
    _pwr = DIA_GET_MAX_PWR(percent);
    bsp_dia_setPower(_diathermy._dia_handler, _pwr);
}

void Diathermy_Pedal_Disconnect()
{
    _diathermy._cof->_set_valve = 0; //关闭电凝输出
    _diathermy._cof->_set_startValve = 0; //关闭电凝输出
    _diathermy._pedal.bytes._duty  = 0;
    _diathermy._data->_read_valve = 0;  
    if (_diathermy._dia_handler != RT_NULL && _diathermy._dia_handler->_enable)
    {
        bsp_dia_setPower(_diathermy._dia_handler, 0);   //直接关闭电凝输出
    }
}

#ifdef FINSH_USING_MSH
int Dia_test(int argc, char *argv[])
{
    if (argc > 1)
    {
        if(!strcmp(argv[1], "Dac") )        //校准指令正确
        {
            uint16_t _dac = atoi(argv[2]);
            rt_kprintf("Dia out dac: %d\n", _dac);
            bsp_dia_setDac(_diathermy._dia_handler, _dac);
        }
        else if(!strcmp(argv[1], "Pwr") )
        {
            float _pwr = DIA_GET_MAX_PWR(atoi(argv[2]));
            rt_kprintf("Dia out percentage: %d\n", atoi(argv[2]));
            rt_kprintf("Dia out power: %d\n", _pwr);
            bsp_dia_setPower(_diathermy._dia_handler, _pwr);
        }
        else
        {
            rt_kprintf("Usage:\n");
            rt_kprintf("Dia_test  Dac xx                    - Use Dac Out\n");
            rt_kprintf("Dia_test  Pwr xx                    - Use Power Out\n");
        }
    }
    else
    {
        rt_kprintf("Usage:\n");
        rt_kprintf("Dia_test  Dac xx                    - Use Dac Out\n");
        rt_kprintf("Dia_test  Pwr xx                    - Use Power Out\n");
    }
}
MSH_CMD_EXPORT(Dia_test, Dia test);
#endif