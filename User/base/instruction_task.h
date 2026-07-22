/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-04 13:21:02
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-06-13 10:26:07
 * @FilePath: \EigeV1.0.01\User\base\instruction_task.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*** 
 * @Author: sum 476328608@qq.com
 * @Date: 2024-01-12 10:40:47
 * @LastEditors: sum 476328608@qq.com
 * @LastEditTime: 2024-04-17 10:38:51
 * @FilePath: \Fluid module 模拟IOP 开放FPUc:\RT-ThreadStudio\workspace\Template\User\base\instruction_task.h
 * @Description: 
 * @
 * @Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __INSTRUCTION_TASK_H
#define __INSTRUCTION_TASK_H	
#include "includes.h"


// 单指令任务优先级
#define INSTRUCTION_TASK_PRIO		30

// 单指令任务堆栈大小
#define INSTRUCTION_STK_SIZE 		512

// 单指令任务句柄 
extern OS_TCB Instruction_TaskTCB;

// 单指令任务堆栈
extern CPU_STK INSTRUCTION_TASK_STK[INSTRUCTION_STK_SIZE];



extern void Instruction_Task(void *p_arg);
extern void Instruction_Task_RegisterFun(uint16_t _code,void (*_excu)(uint16_t _code,uint16_t _data));
extern int Instruction_Task_Init(void);
#endif


