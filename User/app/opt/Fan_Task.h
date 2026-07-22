/*------------------------------------------------------------------------------
 * @file    FAN_TASK.H
 * @author  SIERRA TEAM 
 * @date    2024/07/03 16:46:18
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
#ifndef __FAN_TASK_H
#define __FAN_TASK_H

/* Files includes  -----------------------------------------------------------*/
#include "bsp_fan.h"
/* Defines -------------------------------------------------------------------*/
#define FAN_TASK_TASK_PRIO   29
#define FAN_TASK_STK_SIZE    256

/* Variables -----------------------------------------------------------------*/
typedef struct _Fan_Task_Data
{
    uint32_t    temp1;          //温度传感器1
    uint32_t    temp2;          //温度传感器2
    uint32_t    fan1_rpm;       //风扇1转速
    uint32_t    fan2_rpm;       //风扇2转速
} Fan_Task_Data;

typedef struct _Fan_Task_Conf
{
    uint32_t    _en;            //使能风扇控制
    uint32_t    _set_temp;      //设定启动温度
} Fan_Task_Conf;


typedef struct _Fan_Task_t
{
    struct _Fan_Task_Conf   *_cof; 
    struct _Fan_Task_Data   *_data;
    bsp_fan_t               _fan;
}Fan_Task_t;

#include "includes.h"
extern OS_TCB Fan_TaskTCB;
extern CPU_STK Fan_TASK_STK[FAN_TASK_STK_SIZE];
/* Functions ----------------------------------------------------------------*/
int Fan_Task_Init(void);
void Fan_Task(void *p_arg);


#endif
