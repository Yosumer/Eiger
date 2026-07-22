#include "includes.h"
#define LOG_TAG            "task.Instruct"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>

LIST(instruction); //单指令链表
static uint8_t	cmd_mempool[16*INSTRUCTION_CMD_SIZE];
static struct rt_mempool mp;
typedef struct _cmd_l{
	struct _cmd_l *next;			//指向下一个链表
	void (*excu)(uint16_t _code,uint16_t _data);	//单指令功能函数执行码
	uint16_t 	_code;				//单指令功能码
}cmd_l;
OS_TCB Instruction_TaskTCB;
CPU_STK INSTRUCTION_TASK_STK[INSTRUCTION_STK_SIZE];


int Instruction_Task_Init(void)
{
	rt_err_t err;
    /* 初始化内存池对象 */
    err = rt_mp_init(&mp, "cmd_mp", &cmd_mempool[0], sizeof(cmd_mempool), 12);
	/*初始化单指令任务*/
  	OSTaskCreate( (OS_TCB     *) &Instruction_TaskTCB,
                (CPU_CHAR   *) "Instruction_Task",
                (OS_TASK_PTR ) Instruction_Task,
                (void       *) 0, 
                (OS_PRIO     ) INSTRUCTION_TASK_PRIO, 
                (CPU_STK    *) &INSTRUCTION_TASK_STK[0],
                (CPU_STK_SIZE) INSTRUCTION_STK_SIZE / 10, 
                (CPU_STK_SIZE) INSTRUCTION_STK_SIZE, 
                (OS_MSG_QTY  ) 10, 
                (OS_TICK     ) 0, 
                (void       *) 0, 
                (OS_OPT      ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP), 
                (OS_ERR     *) &err);	
	return 0;
}
INIT_ENV_EXPORT(Instruction_Task_Init);
/**
 * @description: 注册单指令函数
 * @param {uint16_t} _code	单指令功能码
 * @param {uint16_t} _excu	单指令函数
 * @return {*}
 */
void Instruction_Task_RegisterFun(uint16_t _code,void (*_excu)(uint16_t _code,uint16_t _data))
{
	RT_ASSERT(_excu);	//判断当前执行函数是否为空
	cmd_l * _l = rt_mp_alloc(&mp,RT_WAITING_FOREVER);
	if(_l == RT_NULL)
	{
		LOG_E("rt_mp_alloc error");
		return;
	}
	_l->excu = _excu;
	_l->_code = _code;
	list_add(instruction,_l);
}
void Instruction_Task(void *p_arg)
{

 	uint32_t *cmd =NULL;
	OS_MSG_SIZE msg_size_t;
	OS_ERR err;
	CPU_TS ts_t;
	while(1)
	{

        cmd = OSTaskQPend(0,OS_OPT_PEND_BLOCKING,&msg_size_t,&ts_t,&err);		                    
        if(cmd == NULL)	//阻塞模式下不会进入这里
        {
					Delay_ms(10);
					continue;
        }
        else        //执行相应指令
        {

			uint16_t _len = list_length(instruction);
			for(uint8_t i=0;i<12;i++)
			{
				// 读单指令配置表
				if(ramctl.parms[i]>0)	//有配置
				{
					uint16_t _cmd = (uint16_t)(ramctl.parms[i] >> 16);
					uint16_t _data = (uint16_t)(ramctl.parms[i]);
					cmd_l *_item = list_head(instruction);
					for(int i=0;i<_len;i++)
					{
						if(_item->_code == _cmd)
							_item->excu(_cmd,_data);
						_item = list_item_next(_item);
					}
					// 原子操作写32位
					// 清0
        			App_Com_WriteRam(&ramctl.parms[i],0);
				}

			}
        }

	}  		
}



