/*------------------------------------------------------------------------------
 * @file    FW_DOWNLOAD_TASK.H
 * @author  SIERRA TEAM 
 * @date    2024/04/17 15:26:10
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
#ifndef __FW_DOWNLOAD_TASK_H
#define __FW_DOWNLOAD_TASK_H

/* Files includes  -----------------------------------------------------------*/
#include "includes.h"

/* Defines -------------------------------------------------------------------*/
#define FW_DOWNLOAD_TASK_PRIO   10
#define FW_DOWNLOAD_TASK_STK_SIZE    256

//#define YMODEM_TASK_PRIO        (FW_DOWNLOAD_TASK_PRIO+1)
#define YMODEM_TASK_PRIO        (21)
#define YMODEM_TASK_STK_SIZE        2048
/* Variables -----------------------------------------------------------------*/
extern OS_TCB Fw_down_TaskTCB;
extern CPU_STK Fw_down_TASK_STK[FW_DOWNLOAD_TASK_STK_SIZE];
/* Functions ----------------------------------------------------------------*/
int Fw_down_Task_Init(void);
void Fw_down_Task(void *p_arg);


#endif
