/*------------------------------------------------------------------------------
 * @file    NORMAL_TASK.H
 * @author  SIERRA TEAM 
 * @date    2024/04/17 11:06:13
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
#ifndef __NORMAL_TASK_H
#define __NORMAL_TASK_H

/* Files includes  -----------------------------------------------------------*/
#include "includes.h"

/* Defines -------------------------------------------------------------------*/
// #define NORMAL_TASK_TASK_PRIO   15

#define NORMAL_TASK_TASK_PRIO   8
#define NORMAL_TASK_STK_SIZE    1024

/* Variables -----------------------------------------------------------------*/
extern OS_TCB Normal_TaskTCB;
extern CPU_STK Normal_TASK_STK[NORMAL_TASK_STK_SIZE];
/* Functions ----------------------------------------------------------------*/
int Normal_Task_Init(void);
void Normal_Task(void *p_arg);
void Normal_Task_RegisterFun(void (*_excu)(uint16_t _addr,uint32_t _data));
#endif
