
/*------------------------------------------------------------------------------
 * @file    BSP_CAN_YMODEM.H
 * @author  SIERRA TEAM 
 * @date    2024/04/25 15:13:25
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
#ifndef __BSP_CAN_YMODEM_H
#define __BSP_CAN_YMODEM_H

/* Files includes  -----------------------------------------------------------*/
#include "includes.h"

/* Defines -------------------------------------------------------------------*/


/* Variables -----------------------------------------------------------------*/

/* Functions ----------------------------------------------------------------*/
void bsp_can_ymodem_recv(const void *buffer, rt_size_t size);

#endif
