
/*------------------------------------------------------------------------------
 * @file    PEDAL_TASK.H
 * @author  SIERRA TEAM 
 * @date    2023/11/16 13:24:12
 * @brief   THIS FILE PROVIDES ALL THE SIERRA FUNCTIONS.
 * -----------------------------------------------------------------------------
 * @attention 
 
 * THE EXISTING SIERRA IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE 
 * CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE 
 * TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR 
 * CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH 
 * HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN 
 * CONNECTION WITH PRODUCTS MADE BY CUSTOMERS. 
 * 
 * <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2> 
------------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion  ------------------------------------*/
#ifndef __PEDAL_TASK_H
#define __PEDAL_TASK_H

/* Files includes  -----------------------------------------------------------*/
#include "includes.h"

/* Defines -------------------------------------------------------------------*/
#define PEDAL_TASK_PRIO     29
#define PEDAL_STK_SIZE      256



#define DISABLE_FOOTBOARD               FOOTBOARD_ZERO

#define FOOTBOARD_ZERO                  0X0000  
#define FOOTBOARD_LINE_ONE              0x0001  //响应第一段踏板
#define FOOTBOARD_POS1                  0X0002  //位置1输出固定
#define FOOTBOARD_LINE_SECOND           0x0003  //响应第二段踏板
#define FOOTBOARD_POS2                  0x0004  //位置2输出固定
#define FOOTBOARD_LINE_THIRD            0x0005  //响应第三段踏板
#define FOOTBOARD_END                   0X0006  //终点
#define FOOTBOARD_SWITCH                0X0007  //开关
/* Variables -----------------------------------------------------------------*/
typedef struct _Pedal_Task_t
{
    uint32_t *  _footcof;                               //踏板配置指针，指向ramcof中的踏板配置
    uint32_t    _is_connect;                            //踏板连接状态，1连接，0断开
    uint32_t    _recover_cof;                            //用于踏板失能后保存上位机存储的踏板配置
    void (*Pedal_deal)(uint16_t _cmd,uint32_t _data);   //踏板数据处理，各模块根据自己模块响应踏板的命令进行注册
    void (*Pedal_disconnect)(); //
}Pedal_Task_t;
extern  OS_TCB  Pedal_TaskTCB;
extern  CPU_STK PEDAL_TASK_STK[PEDAL_STK_SIZE];

/* Functions ----------------------------------------------------------------*/
extern void Pedal_Task(void *p_arg);
extern void Pedal_Task_SetWData(uint32_t *_cof);
extern void Pedal_Task_Register_DealFun( void (*_fun)(uint16_t _cmd,uint32_t _code));
extern void Pedal_Task_Disable(void);
extern void Pedal_Task_RecoverCof(void);

extern uint32_t Pedal_Task_GetConnectStatus();

#endif
