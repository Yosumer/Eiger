
/*------------------------------------------------------------------------------
 * @file    ENGINEERING_TASK.H
 * @author  SIERRA TEAM 
 * @date    2024/04/17 14:27:13
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
#ifndef __ENGINEERING_TASK_H
#define __ENGINEERING_TASK_H

/* Files includes  -----------------------------------------------------------*/
#include "includes.h"

/* Defines -------------------------------------------------------------------*/
#define ENGINEERING_TASK_PRIO   9
#define ENGINEERING_TASK_STK_SIZE    256

/* Variables -----------------------------------------------------------------*/
extern OS_TCB Engineering_TaskTCB;
extern CPU_STK Engineering_TASK_STK[ENGINEERING_TASK_STK_SIZE];
/* Functions ----------------------------------------------------------------*/
int Engineering_Task_Init(void);
void Engineering_Task(void *p_arg);
#endif
