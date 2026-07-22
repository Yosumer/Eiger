/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-04 13:21:02
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-10 11:53:34
 * @FilePath: \EigeV1.0.01\User\app\liquid\Sensor_Task.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#ifndef __SENSOR_TASK_H
#define __SENSOR_TASK_H

#include	"includes.h"

#define SENSOR_TASK_PRIO					15

#define SENSOR_TASK_STK_SIZE 					1024


extern rt_uint8_t  SENSOR_TASK_STK[SENSOR_TASK_STK_SIZE];
extern  struct rt_thread Sensor_TaskTCB;  // 任务句柄

extern void Sensor_Task(void *p_arg);
uint8_t Cassette_LatchRead();
uint8_t Cassette_SWRead();


uint8_t IV_IS_Connected();
// uint8_t IV_Home_SWRead();
#endif

