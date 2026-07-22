
/*------------------------------------------------------------------------------
 * @file    ENGINEERING_TASK.C
 * @author  FIRMWARE TEAM 
 * @date    2024/04/17 14:30:43
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


/* Files includes  -----------------------------------------------------------*/
#include "Engineering_Task.h"

#define LOG_TAG            "task.Engineering"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>


OS_TCB Engineering_TaskTCB;
CPU_STK Engineering_TASK_STK[ENGINEERING_TASK_STK_SIZE];
rt_thread_t ymodemtid = NULL;
/*----------------------------------------------------------------------------*/
int Engineering_Task_Init(void)
{
	OS_ERR err;
    // 工程模式任务
    OSTaskCreate( (OS_TCB     *) &Engineering_TaskTCB,
                (CPU_CHAR   *) "Engineering_Task",
                (OS_TASK_PTR ) Engineering_Task,
                (void       *) 0, 
                (OS_PRIO     ) ENGINEERING_TASK_PRIO, 
                (CPU_STK    *) &Engineering_TASK_STK[0],
                (CPU_STK_SIZE) ENGINEERING_TASK_STK_SIZE / 10, 
                (CPU_STK_SIZE) ENGINEERING_TASK_STK_SIZE, 
                (OS_MSG_QTY  ) 50, 
                (OS_TICK     ) 0, 
                (void       *) 0, 
                (OS_OPT      ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP), 
                (OS_ERR     *) &err);
}
INIT_APP_EXPORT(Engineering_Task_Init);
void Engineering_Task(void *p_arg)
{
    CanData_t *candata = NULL;
	OS_MSG_SIZE msg_size_t;
	OS_ERR err;
	CPU_TS ts_t;
	while(1)
	{
        candata = OSTaskQPend(0,OS_OPT_PEND_BLOCKING,&msg_size_t,&ts_t,&err);		                    
        RT_ASSERT(candata); //判断数据是否有效

        LOG_D("ASSERT ADDR:%x",candata->addr);
        LOG_D("ASSERT DATA:%x",candata->data);
        if (candata->addr == (uint16_t)&ramcof.config) {      
            //这里放第一次进入正常模式进入的操作
            App_SetDevMode(candata->data);  //设置模式
            rt_memset(&ramctl.parms, 0, sizeof(ramctl.parms));
        }
        else if(candata->addr == (uint16_t)&ramctl.bootmode)    //第一次进入这里
        {
            if(candata->data == 0x01)
            {
                if(ymodemtid == NULL) //没有创建Ymodem接收
                {
                    extern void _ymodem_task(void *p_arg);
                    ymodemtid = rt_thread_create("ymodem_task",_ymodem_task,RT_NULL,YMODEM_TASK_STK_SIZE,YMODEM_TASK_PRIO,10);
                    if (ymodemtid != RT_NULL)
                    {
                        rt_thread_startup(ymodemtid);
                    }
                    else
                    {
                        LOG_E("rt-fota thread create failed.");
                    }
                }
            }
            else
            {
                if(ymodemtid != RT_NULL)
                	rt_thread_delete(ymodemtid);
            }
        }
        else //在这里判断各模块关心的数据地址
        {
            if(ramctl.bootmode == 0x01) //当前是bootload指令
            {
                // 给单指令任务发送指令
                OSTaskQPost(&Fw_down_TaskTCB,(void *)&candata->data,\
                                    (OS_MSG_SIZE)4,OS_OPT_POST_FIFO,&err); 
                
            }
            if(candata->addr == (uint16_t)&ramctl.singleInstruction)    //当前是单指令模式
            {
                // 给单指令任务发送指令
                OSTaskQPost(&Instruction_TaskTCB,(void *)&candata->data,\
                                    (OS_MSG_SIZE)4,OS_OPT_POST_FIFO,&err); 
            }
            if(candata->addr == (uint16_t)&ramctl.reboot && candata->data == 0x01)
                rt_hw_cpu_reset();
            
        }
	} 
}
