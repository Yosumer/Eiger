/*
 * @Author: sum 476328608@qq.com
 * @Date: 2023-09-28 13:30:35
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-03-26 15:51:14
 * @FilePath: \elabc:\sierra\LiquidModule\Liquid_Cj\code\phaco\Source\app\valve_task.c
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#include "includes.h"
#include "valve_task.h"
OS_TCB Valve_TaskTCB;
CPU_STK VALVE_TASK_STK[VALVE_STK_SIZE];
extern CAN_CONTROL Control_Block ramctl;           //全局控制块

Valve_Task_t valve_cof;
	
static int _Valve_Task_AppInit()
{

	OS_ERR err;
	// Valve_SetWdata(&ramdata._valve);
	rt_memcpy(&valve_cof,&ramdata._valve,sizeof(Valve_Task_t));
	bsp_valve_Init();
  //阀门控制任务
	OSTaskCreate( (OS_TCB     *) &Valve_TaskTCB,
                (CPU_CHAR   *) "Valve_Task",
                (OS_TASK_PTR ) Valve_Task,
                (void       *) 0, 
                (OS_PRIO     ) VALVE_TASK_PRIO, 
                (CPU_STK    *) &VALVE_TASK_STK[0],
                (CPU_STK_SIZE) VALVE_STK_SIZE / 10, 
                (CPU_STK_SIZE) VALVE_STK_SIZE, 
                (OS_MSG_QTY  ) 0, 
                (OS_TICK     ) 0, 
                (void       *) 0, 
                (OS_OPT      ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP), 
                (OS_ERR     *) &err);	
}
INIT_ENV_EXPORT(_Valve_Task_AppInit);
void Valve_Task(void *p_arg)
{
	while(1)
	{
		if (rt_memcmp(&valve_cof,&ramdata._valve,sizeof(Valve_Task_t)))	//不相等
		{
			rt_memcpy(&valve_cof,&ramdata._valve,sizeof(Valve_Task_t));
			// 操作所有阀门
			for (int i = 0; i < VALVE_MAX; i++)
			{
				bsp_valve_Ctl(i,valve_cof.buf[i]);
			}
		}

		Delay_ms(10);
	}
}
/**
 * @description: 指向配置块的控制内存
 * @param {Valve_Task_t} *wdata
 * @return {}
 */
void Valve_SetWdata(Valve_Task_t *wdata)
{
	rt_memcpy(&valve_cof,wdata,sizeof(Valve_Task_t));
}



