/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-18 15:41:17
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-27 17:01:01
 * @FilePath: \EigeV1.0.01\User\base\Can_IV_Task.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

// 防止头文件被重复包含
#ifndef CAN_IV_TASK_H
#define CAN_IV_TASK_H
/* Files includes  -----------------------------------------------------------*/
#include <stdint.h>
/* Defines -------------------------------------------------------------------*/
#define SELF_IV_CAN_ID                             IV_CAN_ID   
#define IV_CAN_ID                               0XA0 
#define PHACO_BOARD_MODULE_CAN_ID               0X88            //  超乳板模块
/* Variables -----------------------------------------------------------------*/
// IV 杆的通信格式为 01 E6 10 01 00 00 00 00

/* Functions ----------------------------------------------------------------*/
int Can_IV_Task_Init(void);
int Can_IV_Task_SendMsg(uint8_t *_msg,uint8_t _len);
void Can_IV_Task_Control(uint32_t bottle_height);
int Can_IV_Task_OTA_Send(uint8_t *_msg,uint8_t _len);
int Can_PhacoBoard_Task_OTA_Send(uint8_t *_msg,uint8_t _len);



#endif // CAN_IV_TASK_H