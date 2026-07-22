#include "Data_Task.h"
#include "bsp_adc.h"
#define LOG_TAG            "task.DATA"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>
#define     POLL_PER_TIME        1 //ms  1ms轮训一次

rt_uint8_t  data_task_stack[DATA_STK_SIZE];
static struct rt_thread data_tcb;  // 任务句柄

LIST(poll_data); //单指令链表

LIST(poll_task); //轮训采集链表

static uint8_t	data_mempool[100];
static struct rt_mempool data_mp;
typedef struct _data_fun_l{
	struct _data_fun_l *next;			//指向下一个链表
    uint32_t *adc_value;                //存ADC的地址
    float *pressure;                    //存压力的地址
    PressureSensor_t *pressure_handler;
}data_fun_l;



static uint8_t	poll_task_mempool[100];
static struct rt_mempool poll_task_mp;
// typedef struct _task_fun_l{
// 	struct _data_fun_l *next;			//指向下一个链表
//     void (*_task)();
// }task_fun_l;


typedef struct _task_fun_l{
	struct _data_fun_l *next;			//指向下一个链表
    void (*_task)();
    uint16_t _poll_target_time;                //轮训目标时间
    uint16_t _poll_per_time;                    //轮训周期时间
    uint16_t _poll_count;                       //轮训计数
}task_fun_l;

void Data_Task(void *p_arg)
{
    while (1)
    {

        // //任务轮训
        // task_fun_l *_task_item = list_head(poll_task);
        // while (_task_item != NULL)
        // {
        //     if (_task_item->_task != NULL)
        //     {
        //         _task_item->_task();
        //     }
        //     _task_item = list_item_next(_task_item);
        // }
        //任务轮训
        task_fun_l *_task_item = list_head(poll_task);
        while (_task_item != NULL)
        {
            if (_task_item->_task != NULL && _task_item->_poll_target_time != 0)
            {
                _task_item->_poll_count++;
                if(_task_item->_poll_count*_task_item->_poll_per_time >= _task_item->_poll_target_time)
                {
                    rt_base_t level; //保存中断状态，以便在中断中恢复中断状态，防止中断嵌套
                    level = rt_hw_interrupt_disable();
                    _task_item->_task();
                    _task_item->_poll_count = 0;
                    rt_hw_interrupt_enable(level);
                }
                // _task_item->_task();
            }
            _task_item = list_item_next(_task_item);
        }


        //传感器采集
        data_fun_l *item = list_head(poll_data);
        uint8_t _len = list_length(poll_data);
        for (uint8_t i = 0; i < _len; i++)
        {
            if (item->pressure_handler !=NULL)
            {
                *item->pressure = bsp_prsureSen_GetPressure(item->pressure_handler);
                *item->adc_value = bsp_prsureSen_ReadAD(item->pressure_handler);
            }
            item = list_item_next(item);
        }
        
        Delay_ms(POLL_PER_TIME);
    }
}

int Data_Task_Init()
{
    rt_err_t res;

            /* 初始化内存池对象 */
    res = rt_mp_init(&data_mp, "data_mp", &data_mempool[0], sizeof(data_mempool), sizeof(data_fun_l));
    RT_ASSERT(res == RT_EOK);
    list_init(poll_data);

    //         /* 初始化内存池对象 */
    // res = rt_mp_init(&poll_task_mp, "poll_task_mp", &poll_task_mempool[0], sizeof(poll_task_mempool), sizeof(task_fun_l));
    // RT_ASSERT(res == RT_EOK);
    // list_init(poll_task);

    //初始化ADC
     bsp_adc_init();
//    注册ADC到轮训
   Data_Task_RegisterTask(bsp_adc128S102_task,1);

    res =  rt_thread_init(&data_tcb,"Data_task",Data_Task,RT_NULL,data_task_stack,DATA_STK_SIZE,DATA_TASK_PRIO,10);
    RT_ASSERT(res == RT_EOK);
    rt_thread_startup(&data_tcb);  
}

void Data_Task_RegisterSensor(PressureSensor_t *sensor,uint32_t *adc_addr,float *pressure)
{
    RT_ASSERT(sensor);
    RT_ASSERT(adc_addr);
    RT_ASSERT(pressure);
	data_fun_l * _l = rt_mp_alloc(&data_mp,RT_WAITING_FOREVER);
	if(_l == RT_NULL)
	{
		LOG_E("rt_mp_alloc Sensor error");
		return;
	}
    _l->pressure_handler = sensor;
    _l->adc_value = adc_addr;
    _l->pressure = pressure;
	list_add(poll_data,_l);
}

// void Data_Task_RegisterTask(void (*_task_func)(void))
// {
//     RT_ASSERT(_task_func);
//     	task_fun_l * _l = rt_mp_alloc(&poll_task_mp,RT_WAITING_FOREVER);
// 	if(_l == RT_NULL)
// 	{
// 		LOG_E("rt_mp_alloc Sensor error");
// 		return;
// 	}
//     _l->_task = _task_func;
// 	list_add(poll_task,_l);
// }



void Data_Task_RegisterTask(void (*_task_func)(void),uint16_t _poll_target_time)
{
	RT_ASSERT(_task_func);
	if(list_head(poll_task) == NULL)
	{
            rt_err_t res;
                /* 初始化内存池对象 */
        res = rt_mp_init(&poll_task_mp, "poll_task_mp", &poll_task_mempool[0], sizeof(poll_task_mempool), sizeof(task_fun_l));
        RT_ASSERT(res == RT_EOK);
        list_init(poll_task);
	}
	task_fun_l * _l = rt_mp_alloc(&poll_task_mp,RT_WAITING_FOREVER);
	if(_l == RT_NULL)
	{
		LOG_E("rt_mp_alloc Data Task error");
		return;
	}
    _l->_task = _task_func;
    _l->_poll_target_time = _poll_target_time;
    _l->_poll_per_time = POLL_PER_TIME;
    _l->_poll_count = 0;
	list_add(poll_task,_l);
}



void Data_Task_ResetTaskTimer(void (*_task_func)(void),uint16_t _poll_target_time)
{
	RT_ASSERT(_task_func);
	task_fun_l *_task_item = list_head(poll_task);
	while (_task_item!= NULL)
    {
        if (_task_item->_task == _task_func)
        {
            _task_item->_poll_target_time = _poll_target_time;
            _task_item->_poll_count = (_poll_target_time/_task_item->_poll_per_time);
            return;
        }
		_task_item = list_item_next(_task_item);
    }
}

INIT_COMPONENT_EXPORT(Data_Task_Init);




#ifdef FINSH_USING_MSH

#define OFFSET(TYPE, MEMBER,offset) (((unsigned long)(&(((TYPE *)0)->MEMBER)))/4+offset)

int Ram_GetOffset(int argc, char *argv[])
{
    uint16_t base_offset;
    if (argc > 1)
    {
        if(!strcmp(argv[1], "config") )        //校准指令正确
        {

            base_offset = 0x1000; //

            rt_kprintf("ramctl.heartEn addr: %x\n",OFFSET(Control_Block,heartEn,0x0e00));
            

            rt_kprintf("config addr: %x\n",OFFSET(Config_Block,config,base_offset));
            rt_kprintf("footcof addr: %x\n",OFFSET(Config_Block,footcof,base_offset));

            rt_kprintf("fancof._en addr: %x\n",OFFSET(Config_Block,fancof,base_offset)+OFFSET(Fan_Task_Conf,_en,0));
            rt_kprintf("fancof._set_temp addr: %x\n",OFFSET(Config_Block,fancof,base_offset)+OFFSET(Fan_Task_Conf,_set_temp,0));



            rt_kprintf("phacocof._mode addr: %x\n",OFFSET(Config_Block,phacocof,base_offset)+OFFSET(Phaco_Task_Cof,_mode,0));
            rt_kprintf("phacocof._detectionConn addr: %x\n",OFFSET(Config_Block,phacocof,base_offset)+OFFSET(Phaco_Task_Cof,_detectionConn,0));
            rt_kprintf("phacocof._footcof addr: %x\n",OFFSET(Config_Block,phacocof,base_offset)+OFFSET(Phaco_Task_Cof,_footcof,0));
            rt_kprintf("phacocof._rgb_cof addr: %x\n",OFFSET(Config_Block,phacocof,base_offset)+OFFSET(Phaco_Task_Cof,_rgb_cof,0));
            rt_kprintf("phacocof._prams._lpow addr: %x\n",OFFSET(Config_Block,phacocof,base_offset)+OFFSET(Phaco_Task_Cof,_prams,0)+OFFSET(Phaco_Prams_Cof,_lpow,0));
            rt_kprintf("phacocof._prams._vpow addr: %x\n",OFFSET(Config_Block,phacocof,base_offset)+OFFSET(Phaco_Task_Cof,_prams,0)+OFFSET(Phaco_Prams_Cof,_vpow,0));
            rt_kprintf("phacocof._prams._cutetims addr: %x\n",OFFSET(Config_Block,phacocof,base_offset)+OFFSET(Phaco_Task_Cof,_prams,0)+OFFSET(Phaco_Prams_Cof,_cutetims,0));
            rt_kprintf("phacocof._prams._slpow addr: %x\n",OFFSET(Config_Block,phacocof,base_offset)+OFFSET(Phaco_Task_Cof,_prams,0)+OFFSET(Phaco_Prams_Cof,_slpow,0));
            rt_kprintf("phacocof._prams._elpow addr: %x\n",OFFSET(Config_Block,phacocof,base_offset)+OFFSET(Phaco_Task_Cof,_prams,0)+OFFSET(Phaco_Prams_Cof,_elpow,0));
            rt_kprintf("phacocof._prams._svpow addr: %x\n",OFFSET(Config_Block,phacocof,base_offset)+OFFSET(Phaco_Task_Cof,_prams,0)+OFFSET(Phaco_Prams_Cof,_svpow,0));
            rt_kprintf("phacocof._prams._evpow addr: %x\n",OFFSET(Config_Block,phacocof,base_offset)+OFFSET(Phaco_Task_Cof,_prams,0)+OFFSET(Phaco_Prams_Cof,_evpow,0));
            rt_kprintf("phacocof._prams._ltim addr: %x\n",OFFSET(Config_Block,phacocof,base_offset)+OFFSET(Phaco_Task_Cof,_prams,0)+OFFSET(Phaco_Prams_Cof,_ltim,0));
            rt_kprintf("phacocof._prams._vtim addr: %x\n",OFFSET(Config_Block,phacocof,base_offset)+OFFSET(Phaco_Task_Cof,_prams,0)+OFFSET(Phaco_Prams_Cof,_vtim,0));
            rt_kprintf("phacocof._prams._stim addr: %x\n",OFFSET(Config_Block,phacocof,base_offset)+OFFSET(Phaco_Task_Cof,_prams,0)+OFFSET(Phaco_Prams_Cof,_stim,0));           
           

            rt_kprintf("diacof._mode addr: %x\n",OFFSET(Config_Block,diacof,base_offset)+OFFSET(Diathermy_Task_Cof,_mode,0));
            rt_kprintf("diacof._detectCon addr: %x\n",OFFSET(Config_Block,diacof,base_offset)+OFFSET(Diathermy_Task_Cof,_detectCon,0));
            rt_kprintf("diacof._footcof addr: %x\n",OFFSET(Config_Block,diacof,base_offset)+OFFSET(Diathermy_Task_Cof,_footcof,0));
            rt_kprintf("diacof._work_color addr: %x\n",OFFSET(Config_Block,diacof,base_offset)+OFFSET(Diathermy_Task_Cof,_work_color,0));
            rt_kprintf("diacof._port_color addr: %x\n",OFFSET(Config_Block,diacof,base_offset)+OFFSET(Diathermy_Task_Cof,_set_startValve,0));
            rt_kprintf("diacof._set_valve addr: %x\n",OFFSET(Config_Block,diacof,base_offset)+OFFSET(Diathermy_Task_Cof,_set_valve,0));


            rt_kprintf("aircof._vitduty addr: %x\n",OFFSET(Config_Block,aircof,base_offset)+OFFSET(Pneumatic_Task_Cof,_vitduty,0));
            rt_kprintf("aircof._pumbduty addr: %x\n",OFFSET(Config_Block,aircof,base_offset)+OFFSET(Pneumatic_Task_Cof,_pumbduty,0));
            rt_kprintf("aircof._en addr: %x\n",OFFSET(Config_Block,aircof,base_offset)+OFFSET(Pneumatic_Task_Cof,_en,0));
            rt_kprintf("aircof._pedalcof addr: %x\n",OFFSET(Config_Block,aircof,base_offset)+OFFSET(Pneumatic_Task_Cof,_pedalcof,0));
            rt_kprintf("aircof._start addr: %x\n",OFFSET(Config_Block,aircof,base_offset)+OFFSET(Pneumatic_Task_Cof,_start,0));
            rt_kprintf("aircof._end addr: %x\n",OFFSET(Config_Block,aircof,base_offset)+OFFSET(Pneumatic_Task_Cof,_end,0));

            rt_kprintf("inf_cof._pressure addr: %x\n",OFFSET(Config_Block,_inf_cof,base_offset)+OFFSET(Infusion_Task_Cof,_pedalcof,0));	//灌注脚踏配置
            rt_kprintf("inf_cof._polltime addr: %x\n",OFFSET(Config_Block,_inf_cof,base_offset)+OFFSET(Infusion_Task_Cof,_polltime,0));
            rt_kprintf("inf_cof._enable addr: %x\n",OFFSET(Config_Block,_inf_cof,base_offset)+OFFSET(Infusion_Task_Cof,_enable,0));
						rt_kprintf("check._enable addr: %x\n",OFFSET(Config_Block,_systemCheckCof,base_offset)+OFFSET(SystemCheck_Task_Cof,_enableCheck,0));

        }
        else if(!strcmp(argv[1], "data") )
        {

            base_offset = OFFSET(Data_Block,_valve,0x2000); //
            rt_kprintf("_valve.valve_iv_pwr addr: %x\n",OFFSET(Valve_Task_t,buf,base_offset));
            rt_kprintf("_valve.valve_infusion addr: %x\n",OFFSET(Valve_Task_t,buf,base_offset)+1);
            rt_kprintf("_valve.valve_reflux addr: %x\n",OFFSET(Valve_Task_t,buf,base_offset)+2);
            rt_kprintf("_valve.valve_left_box addr: %x\n",OFFSET(Valve_Task_t,buf,base_offset)+3);
            rt_kprintf("_valve.valve_right_box addr: %x\n",OFFSET(Valve_Task_t,buf,base_offset)+4);
            rt_kprintf("_valve.valve_air_pumb addr: %x\n",OFFSET(Valve_Task_t,buf,base_offset)+5);

            base_offset = OFFSET(Data_Block,fan_data,0x2000); //
            rt_kprintf("fan_data.temp1 addr: %x\n",OFFSET(Fan_Task_Data,temp1,base_offset));
            rt_kprintf("fan_data.temp2 addr: %x\n",OFFSET(Fan_Task_Data,temp2,base_offset));
            rt_kprintf("fan_data.fan1_rpm addr: %x\n",OFFSET(Fan_Task_Data,fan1_rpm,base_offset));
            rt_kprintf("fan_data.fan2_rpm addr: %x\n",OFFSET(Fan_Task_Data,fan2_rpm,base_offset));

            base_offset = OFFSET(Data_Block,phaco_data,0x2000); //
            rt_kprintf("phaco_data._conStatus addr: %x\n",OFFSET(Phaco_Task_Data,_conStatus,base_offset));
            rt_kprintf("phaco_data._lpwr addr: %x\n",OFFSET(Phaco_Task_Data,_lpwr,base_offset));
            rt_kprintf("phaco_data._vpwr addr: %x\n",OFFSET(Phaco_Task_Data,_vpwr,base_offset));
            rt_kprintf("phaco_data._cutetims addr: %x\n",OFFSET(Phaco_Task_Data,_cutetims,base_offset));
            rt_kprintf("phaco_data._pow addr: %x\n",OFFSET(Phaco_Task_Data,_pow,base_offset));
            rt_kprintf("phaco_data._sumpow addr: %x\n",OFFSET(Phaco_Task_Data,_sumpow,base_offset));

            base_offset = OFFSET(Data_Block,dia_data,0x2000); //
            rt_kprintf("dia_data._read_valve addr: %x\n",OFFSET(Diathermy_Task_Data,_read_valve,base_offset));
            rt_kprintf("dia_data._conStatus addr: %x\n",OFFSET(Diathermy_Task_Data,_conStatus,base_offset));
            
            base_offset = OFFSET(Data_Block,air_data,0x2000); //
            rt_kprintf("air_data._vitrawdata1 addr: %x\n",OFFSET(Pneumatic_Task_Data,_vitrawdata,base_offset));
            rt_kprintf("air_data._vitrawdata2 addr: %x\n",OFFSET(Pneumatic_Task_Data,_vitrawdata,base_offset)+1);
            rt_kprintf("air_data._vitpressure1 addr: %x\n",OFFSET(Pneumatic_Task_Data,_vitpressure,base_offset));
            rt_kprintf("air_data._vitpressure2 addr: %x\n",OFFSET(Pneumatic_Task_Data,_vitpressure,base_offset)+1);
            rt_kprintf("air_data._vitmaxpressure1 addr: %x\n",OFFSET(Pneumatic_Task_Data,_vitmaxpressure,base_offset));
            rt_kprintf("air_data._vitmaxpressure2 addr: %x\n",OFFSET(Pneumatic_Task_Data,_vitmaxpressure,base_offset)+1);

            rt_kprintf("air_data._percentofPedal addr: %x\n",OFFSET(Pneumatic_Task_Data,_percentofPedal,base_offset));
            rt_kprintf("air_data._ctlValue addr: %x\n",OFFSET(Pneumatic_Task_Data,_ctlValue,base_offset));



            base_offset = OFFSET(Data_Block,_infusion,0x2000); //
            rt_kprintf("_infusion._ivmotorstep addr: %x\n",OFFSET(Infusion_Task_Data,_ivmotorstep,base_offset));
            rt_kprintf("_infusion._ivcmhight addr: %x\n",OFFSET(Infusion_Task_Data,_ivcmhight,base_offset));

        }
        else
        {
            rt_kprintf("Usage:\n");
            rt_kprintf("Ram_GetOffset  config \n");
            rt_kprintf("Ram_GetOffset  data   \n");
        }
    }
    else
    {
        rt_kprintf("Usage:\n");
        rt_kprintf("Ram_GetOffset  config                     \n");
        rt_kprintf("Ram_GetOffset  data                     \n");
    }
} 
MSH_CMD_EXPORT(Ram_GetOffset, get ram offset);
#endif