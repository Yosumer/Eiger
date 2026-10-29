/*** 
 * @Author: sum 476328608@qq.com
 * @Date: 2024-04-08 16:35:53
 * @LastEditors: sum 476328608@qq.com
 * @LastEditTime: 2024-05-10 17:35:11
 * @FilePath: \Fluid module 模拟IOP 开放FPUc:\RT-ThreadStudio\workspace\Template\User\base\Can_Rx_Task.h
 * @Description: 
 * @
 * @Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */

/*------------------------------------------------------------------------------
 * @file    CAN_RX_TASK.H
 * @author  SIERRA TEAM 
 * @date    2024/04/08 16:35:57
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
#ifndef __CAN_RX_TASK_H
#define __CAN_RX_TASK_H

/* Files includes  -----------------------------------------------------------*/
#include <stdint.h>
/* Defines -------------------------------------------------------------------*/
#define SELF_CAN_ID                             MONITOR_CAN_ID   
#define MONITOR_CAN_ID							0xB0			//  监控板模块
#define PEDAL_MODULE_CAN_ID                     0xB1            //  踏板模块CANID 
#define OPT_MODULE_CAN_ID						0x80			//	照明模块ID
#define PNE_MODULE_CAN_ID						0x81			//	气路模块ID	
#define LIQUID_MODULE_CAN_ID				    0x82			//  液路模块ID
#define LASER_MODULE_CAN_ID					    0x83			//  激光模块ID
#define ELECT_MODULE_CAN_ID                     0x84            //  电凝模块ID
#define PHACO_MODULE_CAN_ID                     0X85            //  超乳模块
#define IV_MODULE_CAN_ID                        0X86            //  IV模块
#define PHACO_BOARD_MODULE_CAN_ID               0X88            //  超乳板模块
/* Variables -----------------------------------------------------------------*/
#define CAN_OTA_ID_COV(ID) (0XFF-ID)

/* Functions ----------------------------------------------------------------*/
int Can_Task_Init(void);
int Can_Task_SendMsg(uint8_t *_msg,uint8_t _len);

int Can_Pedal_Task_OTA_Send(uint8_t *_msg,uint8_t _len);
#endif
