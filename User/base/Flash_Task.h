/*** 
 * @Author: sum 476328608@qq.com
 * @Date: 2024-04-16 17:04:24
 * @LastEditors: sum 476328608@qq.com
 * @LastEditTime: 2024-05-16 13:37:11
 * @FilePath: \Fluid module 模拟IOP 开放FPUc:\RT-ThreadStudio\workspace\Template\User\base\Flash_Task.h
 * @Description: 
 * @
 * @Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */

#ifndef __FLASH_TASK_H
#define __FLASH_TASK_H
#include "includes.h"

#define FLASH_TASK_PRIO		21
#define FLASH_STK_SIZE 		512



#define FLASH_PRAMS_VERIFY      0xAA5555AA

#define FLASH_CMD_READ_PAGE     0XF01   //
#define FLASH_CMD_WRITE_PAGE    0XF02
#define FLASH_CMD_PROTECT       0XF03
extern OS_TCB Flash_TaskTCB;
extern CPU_STK Flash_TASK_STK[FLASH_STK_SIZE];


typedef struct 
{
    uint8_t code;
    uint8_t size;
    char name[10];
    void *writeptr;
    void *readptr;
}Flash_Cof;

extern void Flash_Task(void *p_arg);

extern void Flash_Task_Write(const char *name,uint8_t *writebuf,uint16_t len);
extern uint8_t* Flash_Task_Read(const char *name);
#endif
