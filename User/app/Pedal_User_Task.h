
/*------------------------------------------------------------------------------
 * @file    PEDAL_USER_TASK.H
 * @author  SIERRA TEAM 
 * @date    2024/04/15 15:12:40
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
#ifndef __PEDAL_USER_TASK_H
#define __PEDAL_USER_TASK_H

/* Files includes  -----------------------------------------------------------*/
#include <stdint.h>
/* Defines -------------------------------------------------------------------*/


/* Variables -----------------------------------------------------------------*/

/* Functions ----------------------------------------------------------------*/


int Pedal_User_Task_Init(void);
void Pedal_Task_RegisterFun(void (*_excu)(uint16_t _cmd,uint32_t _code));
#endif
