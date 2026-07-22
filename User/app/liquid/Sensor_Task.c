#include "Sensor_Task.h"
#include "Cassette_Task.h"
#include "Data_Task.h"
#include "Infusion_Task.h"
#include "Suction_Task.h"
#include "bsp_step_tmc5160.h"
#include "bsp_fsm_btn.h"
#include "bsp_adc.h"
#define LOG_TAG            "task.sensor"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>

#define CASIN						"CAIN"
#define CASLCK						"CALC"
#define CARES						"CARE"	//(MCU_SW1)
#define LEFT_SW						"CALSW" //(SW1)
#define RIGHT_SW					"CARSW" //(SW2)


#define IV_INSERT_SW 				"IVIN"		//IV杆插入检测
#define PEN_INSERT_SW 				"PENIN"		//脚踏插入检测



#define VALVE_SW					"VALVE"		//灌注回流阀检测
#define MOTOR_SPEED_SW				"MSPED"	//电机速度检测

#define TIMEOUT						100

#define CASSETTE_LATCH			0
#define CASSETTE_RELEASE_SW		1
#define CASSETTE_SWITCH01		2
#define CASSETTE_SWITCH02 		3	
#define CASSETTE_SWITCH_LEFT	4
#define CASSETTE_SWITCH_RIGHT	5




#define CASSETTE_SWITCH_DEFAULT		0XFFFF
#define CASSETTE_SWITCH_INOFFSET		5000 
#define CASSETTE_SWITCH_LATCHOFFSET		50000 

#define CASSETTE_SWITCH(X) (CASSETTE_SWITCH_DEFAULT-X)

rt_uint8_t  SENSOR_TASK_STK[SENSOR_TASK_STK_SIZE];
struct rt_thread Sensor_TaskTCB;  // 任务句柄


#define 	CASSETTE_RELEASE_PIN	GET_PIN(D,6)  //退盒按键
#define 	CASSETTE_LEFT_PIN	GET_PIN(G,3)	  //SW2
#define     CASSETTE_RIGHT_PIN    GET_PIN(D,5)   //SW1


#define 	IV_CONNECT_PIN		GET_PIN(A,9) 
#define		PEN_CONNECT_PIN		GET_PIN(D,10)

#define     LIGHT_MODE_SW1_PIN 		GET_PIN(C,12) //(MOV_SW1)(right)
#define     LIGHT_MODE_SW2_PIN 		GET_PIN(I,1) //(MOV_SW6)(left)

// #define  DECETE_MODE    USE_SLIDE_RHEOSTAT_MODE


static void _Valve_Current_Check();
static void _Motor_Speed_Check();

#if (DECETE_MODE == USE_LIGHT_MODE)
#include "bsp_z4d_c01.h"
#endif

btn_fsm_t cassette_release,cassette_sw,cassette_latch,cassette_left,cassette_right,iv_connect,pen_connect;//,cassette_sw1,cassette_sw2;

btn_fsm_t valve_btn; //阀电流检测句柄
btn_fsm_t motor_speed_btn; //电机速度检测句柄

static uint8_t __LowLevel_ReadSwitch(uint8_t ID)
{
	uint16_t Re_val;

	switch (ID)
	{
		case CASSETTE_RELEASE_SW:
			Re_val = !(rt_pin_read(CASSETTE_RELEASE_PIN));		//获取退盒按键处理

			break;
		case CASSETTE_SWITCH01:
		case CASSETTE_SWITCH02:
            {
				#if (DECETE_MODE == USE_LIGHT_MODE)
				{
					Re_val = !(rt_pin_read(ID==CASSETTE_SWITCH01?LIGHT_MODE_SW1_PIN:LIGHT_MODE_SW2_PIN));
				}
				#elif (DECETE_MODE == USE_SLIDE_RHEOSTAT_MODE)
				{
					uint16_t _adcvalue =  bsp_get_valve(ID==CASSETTE_SWITCH01?CASSETTE_L_ADC_CH:CASSETTE_R_ADC_CH);
					if (CASSETTE_SWITCH(_adcvalue)>=CASSETTE_SWITCH_INOFFSET)
					{
						Re_val = 1;
					}
					else
						Re_val = 0;
				}
				#endif
				
			}
			break;
		case CASSETTE_LATCH:
            {


				#if (DECETE_MODE == USE_LIGHT_MODE)
				{
					uint16_t _adcvalue_l =  bsp_z4dC01_read(&_z4dc01_1);
					uint16_t _adcvalue_r =  bsp_z4dC01_read(&_z4dc01_2);
					if (_adcvalue_l>=28000&&_adcvalue_r>=28000)
					{
						Re_val = 1;
					}
					else
						Re_val = 0;
				}
				#elif (DECETE_MODE == USE_SLIDE_RHEOSTAT_MODE)
				{
					uint16_t _adcvalue_l =  bsp_get_valve(CASSETTE_L_ADC_CH);
					uint16_t _adcvalue_r =  bsp_get_valve(CASSETTE_R_ADC_CH);
					if (CASSETTE_SWITCH(_adcvalue_l)>=CASSETTE_SWITCH_LATCHOFFSET&&CASSETTE_SWITCH(_adcvalue_r)>=CASSETTE_SWITCH_LATCHOFFSET)
					{
						Re_val = 1;
					}
					else
						Re_val = 0;
				}
				#endif
			}
			break;		
		case CASSETTE_SWITCH_LEFT:
		{
				Re_val = (rt_pin_read(CASSETTE_LEFT_PIN));		//获取退盒按键处理
		}
		break;
		case CASSETTE_SWITCH_RIGHT:
		{
				Re_val = (rt_pin_read(CASSETTE_RIGHT_PIN));		//获取退盒按键处理
		}
		break;
		default:
			break;
	}
	return Re_val;
}
uint8_t Cassette_ReleaseRead()
{
	//这里需要看情况 根据开关触发电平修改 
	uint8_t ret = __LowLevel_ReadSwitch(CASSETTE_RELEASE_SW);
	return ret;
}
uint8_t Cassette_SWRead()
{
	//这里需要看情况 根据开关触发电平修改 
	uint8_t ret = ((__LowLevel_ReadSwitch(CASSETTE_SWITCH01)==1?1:0) & ((__LowLevel_ReadSwitch(CASSETTE_SWITCH02)==1?1:0)));
	// uint8_t ret = (__LowLevel_ReadSwitch(CASSETTE_SWITCH01)==1?1:0) ;
	return ret;
}

uint8_t Cassette_LeftRead()
{
	uint8_t ret = __LowLevel_ReadSwitch(CASSETTE_SWITCH_LEFT);
	return ret;
}
uint8_t Cassette_RightRead()
{
	uint8_t ret = __LowLevel_ReadSwitch(CASSETTE_SWITCH_RIGHT);
	return ret;
}

uint8_t Cassette_LatchRead()
{
	//这里需要看情况 根据开关触发电平修改 
	uint8_t ret = __LowLevel_ReadSwitch(CASSETTE_LATCH);
	return ret;
}

//uint8_t Cassette_LatchRead()
//{
//	//这里需要看情况 根据开关触发电平修改 
//	uint8_t ret = __LowLevel_ReadSwitch(CASSETTE_LATCH);
//	return ret;
//}


uint8_t IV_IS_Connected()
{
	return !(rt_pin_read(IV_CONNECT_PIN));
	// return 1;
}
uint8_t PEN_IS_Connected()
{
	uint8_t ret = !(rt_pin_read(PEN_CONNECT_PIN));
	return ret;
}

//有一个阀门关上就开始检测
uint8_t Infusion_Reflux_Valve_Read()
{
	uint8_t ret =!(bsp_valve_read(VALVE_INFUSION)&bsp_valve_read(VALVE_REFLUX));
	return ret;
}

// 抽吸任务开启后就开始检测
uint8_t Suction_Task_Run()
{
	static int32_t _lastCtl; //上一次控制值
	uint8_t ret = 0;
	int32_t _ctl =ramdata._suctionData._ctlValue;
	if (abs(_ctl-_lastCtl)<=5&&_ctl>0)	//变化5以内进行判断
	{
		ret = 1;
	}
	else
		ret = 0;

	_lastCtl = _ctl;
	
	return ret;
}


static void _Sensor_EventHandler(const char *name,btn_event_t event)
{
	OS_ERR      err;
	switch (event)
	{
		case BTN_EVENT_PRESS:	//光电开关触发
			if (strcmp(name,CASIN)==0)	
			{
				LOG_D("%s:Press On",name);
			}
			else if(strcmp(name,CASLCK)==0)	
			{
				LOG_D("%s:Press On",name);
				ErigeEvt *pe = Q_NEW(ErigeEvt,CASS_LOCKED_SIG);
				QACTIVE_POST(AO_Cassette,&pe->super,NULL);
			}
			else if(strcmp(name,CARES)==0)	
			{
				ErigeEvt *pe = Q_NEW(ErigeEvt,CASSETTERELEASE_SIG);
				QACTIVE_PUBLISH(&pe->super,NULL);
				ramdata._cassette_state._release_sig = 1;	//退盒
				LOG_D("%s:Press On",name);
			}
			else if (strcmp(name,LEFT_SW)==0)
			{
				LOG_D("%s:Press On",name);
				ErigeEvt *pe = Q_NEW(ErigeEvt,CASSETTE_LEFT_ACTIVE_SIG);
				QACTIVE_POST(AO_Cassette,&pe->super,NULL);
			}
			else if (strcmp(name,RIGHT_SW)==0)
			{
				LOG_D("%s:Press On",name);
                ErigeEvt *pe = Q_NEW(ErigeEvt,CASSETTE_RIGHT_ACTIVE_SIG);
                QACTIVE_POST(AO_Cassette,&pe->super,NULL);
			}
			else if (strcmp(name,IV_INSERT_SW)==0)
			{
				LOG_D("%s:Press On",name);
				bsp_valve_Ctl(VALVE_BIT_CTL,IV_PWR_ON);	//打开IV杆电源
				rt_thread_mdelay(500);
				ramdata._infusion._ivenable = 1;	//IV杆插上
				ramdata._cassette_state._iv_is_connect = 1;	//IV杆插上
			}
			else if (strcmp(name,PEN_INSERT_SW)==0)
			{
				LOG_D("%s:Press On",name);
				bsp_valve_Ctl(VALVE_BIT_CTL,PEN_PWR_ON);    	//打开踏板电源
				ramdata._cassette_state._pen_is_connect = 1;	//踏板连接
			}
			else
			{
				
			}
			break;
		case BTN_EVENT_RELEASE:	//光电开关触发后移开
			if (strcmp(name,CASIN)==0)	//电机3光电开关触发
			{
				LOG_D("%s:Press Up",name);
			}
			else if(strcmp(name,CASLCK)==0)	//电机4光电开关2触发
			{
				LOG_D("%s:Press Up",name);
			}
			else if(strcmp(name,CARES)==0)	
			{
				// CassetteEvt *pe = Q_NEW(CassetteEvt, CASSETTERELEASE_SIG);
				// QACTIVE_PUBLISH(&pe->super,NULL);
				ramdata._cassette_state._release_sig = 0;
				LOG_D("%s:Press Up",name);
			}
			else if (strcmp(name,LEFT_SW)==0)
			{
				LOG_D("%s:Press Up",name);
				ramdata._cassette_state._release_left = 0;
				// ErigeEvt *pe = Q_NEW(ErigeEvt,CASSETTE_LEFT_ACTIVE_SIG);
				// QACTIVE_POST(AO_Cassette,&pe->super,NULL);
			}
			else if (strcmp(name,RIGHT_SW)==0)
			{
				LOG_D("%s:Press Up",name);
				ramdata._cassette_state._release_right = 0;
                // ErigeEvt *pe = Q_NEW(ErigeEvt,CASSETTE_RIGHT_ACTIVE_SIG);
                // QACTIVE_POST(AO_Cassette,&pe->super,NULL);
			}
			else if (strcmp(name,IV_INSERT_SW)==0)
			{
				LOG_D("%s:Press Up",name);	//关闭IV杆电源
				bsp_valve_Ctl(VALVE_BIT_CTL,IV_PWR_OFF);	//打开IV杆电源
				ramdata._infusion._ivenable = 0;	//IV断连接
			    ramdata._cassette_state._iv_is_connect = 0;	//IV断连接
			}
			else if (strcmp(name,PEN_INSERT_SW)==0)
			{	
				LOG_D("%s:Press Up",name);	//关闭踏板电源
				bsp_valve_Ctl(VALVE_BIT_CTL,PEN_PWR_OFF);		//打开IV杆电源
				ramdata._cassette_state._pen_is_connect = 0;	//踏板断连接
			}
			else
			{
				
			}
			break;
		
		case BTN_EVENT_LONG:	//光电开关长时间触发
			if (strcmp(name,CASIN)==0)	//电机3光电开关触发
			{
				LOG_E("%s:Press Long On",name);
				ErigeEvt *pe = Q_NEW(ErigeEvt,CASSETTEIN_SIG);
				QACTIVE_POST(AO_Cassette,&pe->super,NULL);
			}
			else
			{
				
			}
			break;
			break;
		default:
			break;
	}
}


static void _Trigger_EventHandler(const char *name,btn_event_t event)
{
	OS_ERR      err;
	switch (event)
	{
		case BTN_EVENT_PRESS:	//光电开关触发
			if (strcmp(name,VALVE_SW)==0)	
			{
				LOG_D("%s:Press On",name);
//				Data_Task_ResetTaskTimer(_Valve_Current_Check,200);		//某一个阀门打开，就开始检测
			}
			else
			{
				
			}
			break;
		case BTN_EVENT_RELEASE:	//光电开关触发后移开
			if (strcmp(name,VALVE_SW)==0)	//电机3光电开关触发
			{
				LOG_D("%s:Press Up",name);
				Data_Task_ResetTaskTimer(_Valve_Current_Check,0);		//俩个阀门关闭，就停止检测
			}
			else if (strcmp(name,MOTOR_SPEED_SW)==0)
			{
				Data_Task_ResetTaskTimer(_Motor_Speed_Check,0);	//进行失速检测关闭
			}
			else
			{
				
			}
			break;
		
		case BTN_EVENT_LONG:	//光电开关长时间触发
			if (strcmp(name,VALVE_SW)==0)	//电机3光电开关触发
			{
				LOG_E("%s:Press Long On",name);
				Data_Task_ResetTaskTimer(_Valve_Current_Check,200);		//某一个阀门打开，就开始检测
			}
			else if(strcmp(name,MOTOR_SPEED_SW)==0)
			{
				LOG_E("%s:Press Long On",name);
				Data_Task_ResetTaskTimer(_Motor_Speed_Check,200);	//进行失速检测
			}
			else
			{
				
			}
			break;
			break;
		default:
			break;
	}
}

static void _Sensors_LowLevel_Init()
{
	rt_pin_mode(CASSETTE_RELEASE_PIN, PIN_MODE_INPUT_PULLUP);	//初始化退盒按键
	rt_pin_mode(PEN_CONNECT_PIN, PIN_MODE_INPUT_PULLUP);		//初始化脚踏插入
	rt_pin_mode(IV_CONNECT_PIN, PIN_MODE_INPUT_PULLUP);			//初始化IV杆插入
	rt_pin_mode(CASSETTE_LEFT_PIN, PIN_MODE_INPUT_PULLUP);		//初始化左到位按键
	rt_pin_mode(CASSETTE_RIGHT_PIN, PIN_MODE_INPUT_PULLUP);		//初始化右到位按键
#if (DECETE_MODE == USE_LIGHT_MODE)
	bsp_z4dC01_Task_Init();
	rt_pin_mode(LIGHT_MODE_SW1_PIN, PIN_MODE_INPUT_PULLUP);		//初始化右到位按键
	rt_pin_mode(LIGHT_MODE_SW2_PIN, PIN_MODE_INPUT_PULLUP);		//初始化右到位按键

#endif // 
}




static void _Motor_Speed_Check()
{
	static uint8_t _errorcnt=0;
	//这里检测电机速度
    bsp_step_motor_t *_this =  (bsp_step_motor_t *) Suction_Task_GetSuctionHandle();
	if(bsp_step_motor_IsLoseSpeed(_this,0.9))	//失速百分之60报错
	{
		LOG_E("Motor LOSE SPEED SetSpeed:%d,MersureSpeed is :%d",(int32_t)_this->data._rps,(int32_t)_this->data._encoderps);
		_errorcnt++;
		if (_errorcnt >= 3)
		{
			bsp_error_list_add(MOTOR_LOSESPEED_ERROR);//灌注阀异常
			_errorcnt = 0;
		}
		// bsp_error_list_add(MOTOR_LOSESPEED_ERROR);//灌注阀异常
	}
	else
	{
		_errorcnt = 0;
	}
}


// 这里放一些电流电压检测的函数


//灌注或者回流阀门打开会进行电流检测
static void _Valve_Current_Check()
{
	static struct 
	{
		uint8_t _allvalvecnt;
		uint8_t _infusioncnt;
		uint8_t _refluxcnt;
	}valve_error = {0};
	
	if(bsp_valve_read(VALVE_INFUSION)&&bsp_valve_read(VALVE_REFLUX))	//俩个灌注阀门都打开
	{
		uint16_t _adcvalue =  bsp_get_valve(I_DCT_ADC_CH);
		if(bsp_get_valve(I_DCT_ADC_CH)>50)
		{
			valve_error._allvalvecnt++;
			if (valve_error._allvalvecnt >= 3)
			{
				valve_error._allvalvecnt = 0;
				bsp_error_list_add(INFUSION_VALVE_ERROR);//灌注阀异常
				bsp_error_list_add(REFLUX_VALVE_ERROR);//灌注阀异常
			}
			
		}
		LOG_E("All Valve Raw I value  is :%d",bsp_get_valve(I_DCT_ADC_CH));
	}
    else if(bsp_valve_read(VALVE_INFUSION))	//灌注阀门打开回流阀关闭
	{
		if(bsp_get_valve(I_DCT_ADC_CH)<350)
		{
			valve_error._infusioncnt++;
			if (valve_error._infusioncnt >= 3)
			{
				bsp_error_list_add(INFUSION_VALVE_ERROR);//灌注阀异常
				valve_error._infusioncnt = 0;
			}
		}
		LOG_E("Infusion Valve Raw I value  is :%d",bsp_get_valve(I_DCT_ADC_CH));
	}
	else if (bsp_valve_read(VALVE_REFLUX))	//回流阀门打开灌注阀关闭
	{
		if(bsp_get_valve(I_DCT_ADC_CH)<350)
		{
			valve_error._refluxcnt++;
			if (valve_error._refluxcnt >= 3)
			{
				bsp_error_list_add(REFLUX_VALVE_ERROR);//灌注阀异常
				valve_error._refluxcnt = 0;
			}
			// bsp_error_list_add(REFLUX_VALVE_ERROR);//灌注阀异常
		}
		LOG_E("Reflux Valve Raw I value  is :%d",bsp_get_valve(I_DCT_ADC_CH));
	}
	else
	{
		valve_error._allvalvecnt = 0;
		valve_error._infusioncnt = 0;
		valve_error._refluxcnt = 0;
	}
}




int Sensor_Init()
{
	rt_err_t res;

	// 已经在电机初始化中初始了IO口
	_Sensors_LowLevel_Init();
	// 2.初始化btn结构体

	btn_fsm_Init(&cassette_release,Cassette_ReleaseRead,TIMEOUT,CARES);
	btn_fsm_Init(&cassette_sw,Cassette_SWRead,TIMEOUT,CASIN);
	btn_fsm_Init(&cassette_latch,Cassette_LatchRead,TIMEOUT,CASLCK);
	btn_fsm_Init(&cassette_left,Cassette_LeftRead,TIMEOUT,LEFT_SW);
	btn_fsm_Init(&cassette_right,Cassette_RightRead,TIMEOUT,RIGHT_SW);
	btn_fsm_Init(&iv_connect,IV_IS_Connected,TIMEOUT,IV_INSERT_SW);
	btn_fsm_Init(&pen_connect,PEN_IS_Connected,TIMEOUT,PEN_INSERT_SW);


	btn_fsm_Init(&valve_btn,Infusion_Reflux_Valve_Read,TIMEOUT,VALVE_SW);
	Data_Task_RegisterTask(_Valve_Current_Check,0);


	btn_fsm_Init(&motor_speed_btn,Suction_Task_Run,TIMEOUT,MOTOR_SPEED_SW);
	Data_Task_RegisterTask(_Motor_Speed_Check,0);

	//3.设置回调函数 
	btn_fsm_set_event_cb(&cassette_release,_Sensor_EventHandler);
	btn_fsm_set_event_cb(&cassette_sw,_Sensor_EventHandler);
	btn_fsm_set_event_cb(&cassette_latch,_Sensor_EventHandler);
	btn_fsm_set_event_cb(&cassette_left,_Sensor_EventHandler);
	btn_fsm_set_event_cb(&cassette_right,_Sensor_EventHandler);
	btn_fsm_set_event_cb(&iv_connect,_Sensor_EventHandler);
	btn_fsm_set_event_cb(&pen_connect,_Sensor_EventHandler);

	btn_fsm_set_event_cb(&valve_btn,_Trigger_EventHandler);
	btn_fsm_set_event_cb(&motor_speed_btn,_Trigger_EventHandler);
	
    // 创建传感器任务
    res =  rt_thread_init(&Sensor_TaskTCB,"Sensor_Task",Sensor_Task,RT_NULL,SENSOR_TASK_STK,SENSOR_TASK_STK_SIZE,SENSOR_TASK_PRIO,10);
    RT_ASSERT(res == RT_EOK);
    rt_thread_startup(&Sensor_TaskTCB);  
}
INIT_ENV_EXPORT(Sensor_Init);
void Sensor_Task(void *p_arg)
{

	while (1)
	{
		if(ramctl.enable == 1)
		{
			if(ramdata._cassette_state._state ==  CASSETTE_UNLOAD)
			{
				btn_fsm_handle(&cassette_sw);
				btn_fsm_handle(&cassette_latch);
				btn_fsm_handle(&cassette_release);
			}
			else if (ramdata._cassette_state._state == CASSETTE_RELEASING)
			{
				btn_fsm_handle(&cassette_left);
				btn_fsm_handle(&cassette_right);
			}
			else if(ramdata._cassette_state._state ==  CASSETTE_LOAD || ramdata._cassette_state._state ==  CASSETTE_LOADERROR)
			{
				// if (ramdata._cassette_state._state == CASSETTE_RELEASING)
				// {
				// 	btn_fsm_handle(&cassette_left);
				// 	btn_fsm_handle(&cassette_right);
				// }
				btn_fsm_handle(&cassette_release);


				btn_fsm_handle(&valve_btn);		//积液盒上盒后才能进行阀的检测
				btn_fsm_handle(&motor_speed_btn);		//积液盒上盒后才能蠕动泵失速监测
			}
		}
		btn_fsm_handle(&iv_connect);
		btn_fsm_handle(&pen_connect);
		Delay_ms(TIMEOUT);
	}
	
}
