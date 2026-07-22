/*------------------------------------------------------------------------------
 * @file    FAN_TASK.C
 * @author  FIRMWARE TEAM 
 * @date    2024/07/03 17:20:01
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
#define _FAN_TASK_C_

/* Files includes  -----------------------------------------------------------*/
#include <board.h>
#include "Fan_Task.h"
#include "bsp_adc.h"
#include "bsp_timer3_capture.h"
 #include "bsp_3950temp.h"
#define LOG_TAG            "task.Fan"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>


#define PWM2_DEV_NAME       "pwm2"

#define FAN1_TIME2_CHANNEL          2
#define FAN2_TIME2_CHANNEL          1

#define FAN_FRQ   5000

#define FAN_FRQ_ARR 1000 

OS_TCB Fan_TaskTCB;
CPU_STK Fan_TASK_STK[FAN_TASK_STK_SIZE];
struct rt_device_pwm *time2_pwm_dev;
Fan_Task_t _fan_handler = {0};
Fan_Task_Conf _fan_local_cof = {0};
extern TIM_HandleTypeDef htim3;
/*----------------------------------------------------------------------------*/

static void pwm_init(void)
{
    // time2_pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM2_DEV_NAME);
    // if(RT_NULL == time2_pwm_dev)
    // {
    //     LOG_E("Can not find pwm:%s",time2_pwm_dev);
    // }
    // // 设置pwm频率
    // rt_pwm_set(time2_pwm_dev,FAN1_TIME2_CHANNEL,FAN_FRQ,0);
    // rt_pwm_set(time2_pwm_dev,FAN2_TIME2_CHANNEL,FAN_FRQ,0);
    // rt_pwm_enable(time2_pwm_dev,FAN1_TIME2_CHANNEL);
    // rt_pwm_enable(time2_pwm_dev,FAN2_TIME2_CHANNEL);

    //定时器4在HAL库初始化中初始化了
    return;
}

static void fan1_set_duty(uint8_t _duty)
{
    uint16_t _plus = (_duty*FAN_FRQ_ARR)/100;
    // rt_pwm_set(time2_pwm_dev,FAN1_TIME2_CHANNEL,FAN_FRQ,_plus);
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,_plus);
	
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);

}
static void fan2_set_duty(uint8_t _duty)
{
    uint16_t _plus = (_duty*FAN_FRQ_ARR)/100;
    // rt_pwm_set(time2_pwm_dev,FAN2_TIME2_CHANNEL,FAN_FRQ,_plus);
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,_plus);
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
}

static void fan3_set_duty(uint8_t _duty)
{
    uint16_t _plus = (_duty*FAN_FRQ_ARR)/100;
    // rt_pwm_set(time2_pwm_dev,FAN2_TIME2_CHANNEL,FAN_FRQ,_plus);
    __HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_1,_plus);
    HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_1);
}

static void fans_set_duty(uint8_t _duty)
{
    fan1_set_duty(_duty);
    fan2_set_duty(_duty);
    fan3_set_duty(_duty);

//		static uint8_t duty=30;
//		fan1_set_duty(duty);
//    fan2_set_duty(duty);
//    fan3_set_duty(duty);
}
 bsp_3950temp_t _temp1;
// bsp_3950temp_t _temp2;
static float _get_ress(uint8_t _ch)
{
    uint16_t _ad =  bsp_get_valve(_ch);
	float _vol = ((_ad/65535.0)*3.0);
    float _cur = ((3.3-_vol)/10000.0);
	float _res = (_vol/_cur);
    return _res;
}
static float _get_res1()
{
    return _get_ress(TEMP_ADC_CH);
    // return 0;
}
static float _get_res2()
{
    // return _get_ress(TEMP_ADC_CH2);
    return 0;
}

static float _get_All_Temp()
{

    // return 50;
    return bsp_3950temp_Read(&_temp1);
    // return ((bsp_3950temp_Read(&_temp1)+bsp_3950temp_Read(&_temp2))/2);
}

void _Fan_Task_HardWare_Init()
{
    pwm_init();
    bsp_3950temp_Init(&_temp1,_get_res1);
    // bsp_3950temp_Init(&_temp2,_get_res2);
}
int Fan_Task_Init(void)
{
    OS_ERR err;
    static fan_ops _ops = {
        .fan_init = _Fan_Task_HardWare_Init,
        .get_temp = _get_All_Temp,
        .get_raw_cnt = bsp_timer3_get_fan1_Cnt,
        .set_pwm = fans_set_duty
    };
    bsp_fan_Init(&_fan_handler._fan,2000000,&_ops);
    // 全局配置块
    ramcof.fancof._en = 1;
    ramcof.fancof._set_temp = _fan_handler._fan._set_temp;
    
    _fan_handler._cof = &ramcof.fancof;
    _fan_handler._data = &ramdata.fan_data;

    bsp_fan_Enable(&_fan_handler._fan);
	OSTaskCreate( (OS_TCB     *) &Fan_TaskTCB,
                (CPU_CHAR   *) "fan_task",
                (OS_TASK_PTR ) Fan_Task,
                (void       *) 0, 
                (OS_PRIO     ) FAN_TASK_TASK_PRIO, 
                (CPU_STK    *) &Fan_TASK_STK[0],
                (CPU_STK_SIZE) FAN_TASK_STK_SIZE/ 10, 
                (CPU_STK_SIZE) FAN_TASK_STK_SIZE, 
                (OS_MSG_QTY  ) 10, 
                (OS_TICK     ) 0, 
                (void       *) 0, 
                (OS_OPT      ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP), 
                (OS_ERR     *) &err); 
}
INIT_APP_EXPORT(Fan_Task_Init);



void Fan_Task(void *p_arg)
{

    while (1)
    {

        if(rt_memcmp(&_fan_local_cof,_fan_handler._cof,sizeof(Fan_Task_Conf)))
        {
            rt_memcpy(&_fan_local_cof,_fan_handler._cof,sizeof(Fan_Task_Conf));
            bsp_fan_SetTargetTemp(&_fan_handler._fan,_fan_local_cof._set_temp);
            if(_fan_local_cof._en)
                bsp_fan_Enable(&_fan_handler._fan);
            else
                bsp_fan_Disable(&_fan_handler._fan);
        }
        bsp_fan_Handler(&_fan_handler._fan);
		App_Com_WriteRam(&_fan_handler._data->fan1_rpm,bsp_fan_GetRmp(&_fan_handler._fan));
		App_Com_WriteRam(&_fan_handler._data->fan2_rpm,(1.0/((1.0/2000000)*(float)bsp_timer5_get_fan3_Cnt()))*60);


		App_Com_WriteRam(&_fan_handler._data->temp1,(uint32_t)bsp_3950temp_Read(&_temp1));
		// App_Com_WriteRam(&_fan_handler._data->temp2,(uint32_t)bsp_3950temp_Read(&_temp2));
        Delay_ms(100);
    }

}

