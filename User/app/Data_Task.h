
#ifndef _DATA_TASK_H
#define _DATA_TASK_H
#include "includes.h"

#define DATA_TASK_PRIO		28

//任务堆栈大小	
#define DATA_STK_SIZE 		1024


void Data_Task(void *p_arg);

int Data_Task_Init();
void Data_Task_RegisterSensor(PressureSensor_t *sensor,uint32_t *adc_addr,float *pressure);
// void Data_Task_RegisterTask(void (*_task_func)(void));
void Data_Task_RegisterTask(void (*_task_func)(void),uint16_t _poll_target_time);
void Data_Task_ResetTaskTimer(void (*_task_func)(void),uint16_t _poll_target_time);
#endif
