/*------------------------------------------------------------------------------
 * @file    PEDAL_USER_TASK.C
 * @author  FIRMWARE TEAM 
 * @date    2024/04/15 15:12:56
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
#include "includes.h"

#include "Phaco_Task.h"
#include "Diathermy_Task.h"
#include "Cassette_Task.h"
#include "Suction_Task.h"
#include "Pneumatic_Task.h"
#include "instruction_Task.h"
#define LOG_TAG            "task.UserPedal"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>
extern Config_Block ramcof;
LIST(pedal); //单指令链表
static uint8_t	pedal_mempool[12*20];
static struct rt_mempool pedal_mp;
typedef struct _pedal_fun_l{
	struct _pedal_fun_l *next;			//指向下一个链表
	void (*excu)(uint16_t _addr,uint32_t _data);	//单指令功能函数执行码
}pedal_fun_l;
static void Pedal_User_DisconnectFun ();
static void Pedal_User_DealFun (uint16_t _cmd,uint32_t _code);

static uint32_t pedal_cof = 1;  //默认使能踏板
/*----------------------------------------------------------------------------*/
/**
 * @description:用户踏板任务初始化
 * @return {*}
 */
int Pedal_User_Task_Init()
{
    OS_ERR  err;

    //1.将内存块的踏板配置任务放在
    // Pedal_Task_SetWData(&ramcof.footcof);
    Pedal_Task_SetWData(&pedal_cof);


    //2.注册本模块的踏板具体处理函数
    Pedal_Task_Register_DealFun(Pedal_User_DealFun);
    //3.注册踏板断联处理函数
    Pedal_Task_Register_Disconnect(Pedal_User_DisconnectFun);

    ramcof._pedaldetectCof = 1; //默认开启脚踏插拔检测
    //3.初始化踏板任务

        /* 初始化内存池对象 */
    err = rt_mp_init(&pedal_mp, "pedal_mp", &pedal_mempool[0], sizeof(pedal_mempool), 8);
  // 踏板任务
    OSTaskCreate( (OS_TCB     *) &Pedal_TaskTCB,
                (CPU_CHAR   *) "Pedal_TaskTCB",
                (OS_TASK_PTR ) Pedal_Task,
                (void       *) 0, 
                (OS_PRIO     ) PEDAL_TASK_PRIO, 
                (CPU_STK    *) &PEDAL_TASK_STK[0],
                (CPU_STK_SIZE) PEDAL_STK_SIZE / 10, 
                (CPU_STK_SIZE) PEDAL_STK_SIZE, 
                (OS_MSG_QTY  ) 10, 
                (OS_TICK     ) 0, 
                (void       *) 0, 
                (OS_OPT      ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP), 
                (OS_ERR     *) &err);
}
#ifdef  USE_PEDAL_MODE 
INIT_ENV_EXPORT(Pedal_User_Task_Init);
#endif



void Pedal_Task_RegisterFun(void (*_excu)(uint16_t _cmd,uint32_t _code))
{
	RT_ASSERT(_excu);	//判断当前执行函数是否为空
	pedal_fun_l * _l = rt_mp_alloc(&pedal_mp,RT_WAITING_FOREVER);
	if(_l == RT_NULL)
	{
		LOG_E("rt_mp_alloc fun error");
		return;
	}
	_l->excu = _excu;
	list_add(pedal,_l);
}
/**
 * @description: 用户层面踏板处理函数
 * @param {uint16_t} _cmd   踏板功能码 具体定义请查阅对应模块
 * @param {uint32_t} _code  按键+行程码
 * @return {*}
 */
static void Pedal_User_DealFun (uint16_t _cmd,uint32_t _code)
{
        LOG_D("Pedal cmd:%x",_cmd);
        LOG_D("Pedal code:%x",_code);
        pedal_fun_l *_item = list_head(pedal);
        uint16_t _len = list_length(pedal);
        for(int i=0;i<_len;i++)
        {
            _item->excu(_cmd,_code);
            _item = list_item_next(_item);
        }
}
//踏板断开处理函数
static void Pedal_User_DisconnectFun ()
{
    if (ramcof._pedaldetectCof == 1)
    {
        Diathermy_Pedal_Disconnect();       //电凝踏板断开连接
        Phaco_Pedal_Disconnect();           //超乳踏板断开连接
        Pneumatic_Pedal_Disconnect();       //波切脚踏断开连接
        Suction_Task_Pedal_Disconnect();    //抽吸响应关闭
    }
    
}