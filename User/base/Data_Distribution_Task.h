/*** 
 * @Author: sum 476328608@qq.com
 * @Date: 2024-04-09 09:45:41
 * @LastEditors: sum 476328608@qq.com
 * @LastEditTime: 2024-05-16 10:58:32
 * @FilePath: \Fluid module 模拟IOP 开放FPUc:\RT-ThreadStudio\workspace\Template\User\base\Data_Distribution_Task.h
 * @Description: 
 * @
 * @Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
/*------------------------------------------------------------------------------
 * @file    DATA_DISTRIBUTION_TASK.H
 * @author  SIERRA TEAM 
 * @date    2024/04/09 09:45:44
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
#ifndef __DATA_DISTRIBUTION_TASK_H
#define __DATA_DISTRIBUTION_TASK_H

/* Files includes  -----------------------------------------------------------*/
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/
#define     DATA_DISTRIBUTION_TASK_SIZE       1024
#define     DATA_DISTRIBUTION_TASK_PRIO       9

/* Variables -----------------------------------------------------------------*/
typedef int (*send_msg)(uint8_t* buf,uint8_t _size);

enum Data_Com_Mode{
    COM_USE_CAN = 0,        //使用can通信
    COM_USE_ZIGBEE,          //使用ZIGBEE进行通信
    COM_USE_UART,           //使用串口进行通信
    COM_USE_TCP,            //使用TCP进行通信   
    COM_USE_BOOT            //使用shell 进行boot升级
};

typedef struct _Data_Distribution_t {
    uint8_t _mode;          //当前通信使用的模式
    send_msg    _send;      //数据发送指针
}Data_Distribution_t;


/* Extern -----------------------------------------------------------------*/
extern rt_uint8_t  dataDistribution_task_stack[DATA_DISTRIBUTION_TASK_SIZE];
extern struct rt_thread dataDistribution_tcb;  //数据分发任务分发句柄


/* Functions ----------------------------------------------------------------*/
void Data_Distribution_Task(void *parameter);
int Data_Distribution_Init();
void Data_Distribution_SendMsg(uint8_t* _buf,uint8_t _size);
void Data_Distribution_CtlMode(uint8_t _mode);

#endif
