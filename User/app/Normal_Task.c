/*------------------------------------------------------------------------------
 * @file    NORMAL_TASK.C
 * @author  FIRMWARE TEAM 
 * @date    2024/04/17 11:11:15
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
#include "Normal_Task.h"
#define LOG_TAG            "task.Normal"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>



LIST(cmd); //单指令链表
static uint8_t	cmd_mempool[12*50];
static struct rt_mempool cmd_mp;
typedef struct _cmd_fun_l{
	struct _cmd_fun_l *next;			//指向下一个链表
	void (*excu)(uint16_t _addr,uint32_t _data);	//单指令功能函数执行码
}cmd_fun_l;

OS_TCB Normal_TaskTCB;
CPU_STK Normal_TASK_STK[NORMAL_TASK_STK_SIZE];

extern OS_Q      GLOBAL_Msg;                     //全局使能消息队列
/*----------------------------------------------------------------------------*/
int Normal_Task_Init(void)
{
    OS_ERR err;
    /* 初始化内存池对象 */
    err = rt_mp_init(&cmd_mp, "cmd_nor_mp", &cmd_mempool[0], sizeof(cmd_mempool), 8);
    // 正常模式任务
    OSTaskCreate( (OS_TCB     *) &Normal_TaskTCB,
                (CPU_CHAR   *) "Normal_TaskTCB",
                (OS_TASK_PTR ) Normal_Task,
                (void       *) 0, 
                (OS_PRIO     ) NORMAL_TASK_TASK_PRIO, 
                (CPU_STK    *) &Normal_TASK_STK[0],
                (CPU_STK_SIZE) NORMAL_TASK_STK_SIZE / 10, 
                (CPU_STK_SIZE) NORMAL_TASK_STK_SIZE, 
                (OS_MSG_QTY  ) 50, 
                (OS_TICK     ) 0, 
                (void       *) 0, 
                (OS_OPT      ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP), 
                (OS_ERR     *) &err);

	return 0;
}

INIT_ENV_EXPORT(Normal_Task_Init);
void Normal_Task(void *p_arg)
{
    CanData_t *candata = NULL;
    CanData_t _send = {
        .dir = 0x02,
        .id = CURRENT_MODULE_ID,
    };
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
        }
        else //在这里判断各模块关心的数据地址
        {
            if(candata->addr == (uint16_t)&ramctl.enable)   //
            {
                // 给所有任务发送全局使能指令
                OSQPost((OS_Q*)&GLOBAL_Msg,		
                        (void *)&candata->data,
                        (OS_MSG_SIZE)4,
                        (OS_OPT		)OS_OPT_POST_FIFO | OS_OPT_POST_ALL,
                        (OS_ERR*	)&err);	
            }
            cmd_fun_l *_item = list_head(cmd);
						uint16_t _len = list_length(cmd);
            for(int i=0;i<_len;i++)
            {
                _item->excu(candata->addr,candata->data);
                _item = list_item_next(_item);
            }
            if(candata->addr == (uint16_t)&ramctl.bat.bat_enable && candata->data == 1)   //批量读使能
            {
                if(ramctl.bat.bat_S_addr>=DATA_BASE&&ramctl.bat.bat_S_addr<MAX_BASE)//检测访问地址是否合法
                {
                    // 获取批量读的首地址
                    uint32_t *_readAddr = (uint32_t *)((uint32_t)&ramdata+((ramctl.bat.bat_S_addr - DATA_BASE)*4));
                    for(uint16_t i=0; i<ramctl.bat.bat_len;i++)
                    {
                        _send.data = BSWAP_32(*_readAddr),
                        _send.addr = BSWAP_16(ramctl.bat.bat_S_addr),
                        Data_Distribution_SendMsg((uint8_t*)&_send,8);
                        _readAddr++;
                        ramctl.bat.bat_S_addr++;
                    }
                }

            }
        }
	}  
}



void Normal_Task_RegisterFun(void (*_excu)(uint16_t _addr,uint32_t _data))
{
	RT_ASSERT(_excu);	//判断当前执行函数是否为空
	cmd_fun_l * _l = rt_mp_alloc(&cmd_mp,RT_WAITING_FOREVER);
	if(_l == RT_NULL)
	{
		LOG_E("rt_mp_alloc fun error");
		return;
	}
	_l->excu = _excu;
	list_add(cmd,_l);
}