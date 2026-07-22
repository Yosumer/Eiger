
/*------------------------------------------------------------------------------
 * @file    PEDAL_TASK.C
 * @author  FIRMWARE TEAM 
 * @date    2023/11/16 13:24:29
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

#define LOG_TAG            "task.Pedal"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>
/*----------------------------------------------------------------------------*/

// #define PEDAL_COM_TIMEOUT_MS    57
#define PEDAL_COM_TIMEOUT_MS    200

Pedal_Task_t pedal = {0};
OS_TCB  Pedal_TaskTCB;
CPU_STK PEDAL_TASK_STK[PEDAL_STK_SIZE];
/**
 * @description: 踏板任务全局任务指针赋值
 * @param {Pedal_Task_WData} *_data
 * @return {*}
 */
void Pedal_Task_SetWData(uint32_t *_cof)
{
    RT_ASSERT(_cof);
    pedal._footcof = _cof;
}
/**
 * @description: 注册踏板处理函数
 * @param {uint16_t} _cmd
 * @param {uint32_t} _code
 * @return {*}
 */
void Pedal_Task_Register_DealFun( void (*_fun)(uint16_t _cmd,uint32_t _code))
{
    RT_ASSERT(_fun);
    pedal.Pedal_deal = _fun;
}
void Pedal_Task_Register_Disconnect(void (*_disconnect)())
{
    RT_ASSERT(_disconnect);
    pedal.Pedal_disconnect = _disconnect;
}
/**
 * @description: 失能踏板任务
 * @return {*}
 */
void Pedal_Task_Disable()
{
    //1.save old config
    pedal._recover_cof= *pedal._footcof;
    //2.原子操作写入ram
    App_Com_WriteRam(pedal._footcof,DISABLE_FOOTBOARD);
}
/**
 * @description: 恢复踏板任务配置，该函数一般与失能踏板任务成对出现
 * @return {*}
 */
void Pedal_Task_RecoverCof()
{
    App_Com_WriteRam(pedal._footcof,pedal._recover_cof);
}
void Pedal_Task_Timeout(void *param)
{
    if (pedal.Pedal_disconnect != RT_NULL)
    {
        pedal.Pedal_disconnect();
        pedal._is_connect = 0;
    }
}

uint32_t Pedal_Task_GetConnectStatus()
{
    return pedal._is_connect;
}

/**
 * @description: 踏板任务入口函数
 * @param {void} *p_arg
 * @return {*}
 */
void Pedal_Task(void *p_arg)
{
    uint8_t *cmd = NULL;
    OS_MSG_SIZE msg_size_t;
    OS_ERR err;
    CPU_TS ts_t;
    CanData_t candata1 = {0};

    static rt_timer_t pedaltimer;    //踏板超时定时器

    pedaltimer = rt_timer_create("pedal", Pedal_Task_Timeout,RT_NULL,PEDAL_COM_TIMEOUT_MS,RT_TIMER_FLAG_ONE_SHOT);
    if (pedaltimer != RT_NULL)
    {
        LOG_D("Timer created successfully!\n");
    }
    else
    {
        LOG_E("Failed to create timer!\n");
        // return -1;
    }
    while(1)
    {
        rt_timer_start(pedaltimer); //启动定时器超时
        cmd = OSTaskQPend(0,OS_OPT_PEND_BLOCKING,&msg_size_t,&ts_t,&err);
        if(cmd[1] != PEDAL_MODULE_ID)
            continue;
        rt_timer_stop(pedaltimer);  //关闭定时器防止超时发生
        pedal._is_connect = 1;  //有数据认为脚踏有链接
        if(rt_memcmp(&candata1,cmd,msg_size_t))  //踏板数据不相等才进行处理
        {
            //因为在数据处理任务中已经进行了大小端转换所以这里不需要进行转换
            rt_memcpy(&candata1,cmd,msg_size_t);		
            if(*pedal._footcof != DISABLE_FOOTBOARD)    //踏板配置不为失能才会进行解析
            {
                LOG_D("Pedal cmd:%x",candata1.addr);
                LOG_D("Pedal code:%x",candata1.data);
                pedal.Pedal_deal(candata1.addr,candata1.data);
            }
        }
    }  

}
