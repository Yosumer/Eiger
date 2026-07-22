/*------------------------------------------------------------------------------
 * @file    INCLUDES.H
 * @author  SIERRA TEAM 
 * @date    2024/04/09 15:42:16
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
#ifndef __INCLUDES_H
#define __INCLUDES_H

/********************当前设备是什么模块********************************/
// #define USE_LIGHT_MODE          //使用光电模块
// #define USE_FOOTBOARD_MODE      //使用踏板模块
// #define USE_CAP_TEST           //电容测试
#define PHACO_TEST              //超乳工装测试
// #define USE_ULTRASONIC_MODE     //超声模式
#define USE_QPC_MODULE           //使用QP状态机

#define USE_PEDAL_MODE           //是否使用踏板   不使用踏板的模块将该宏定义屏蔽
#define USE_FAL_MODE

#define USE_VOFA_DEBUG

/****************************************************/ 



/*
*********************************************************************************************************
*                                                 OS
*********************************************************************************************************
*/
#include <rtthread.h>
#include <rtatomic.h>
//UCOS3 封装层相关文件
#include  <os.h>
#include  <os_type.h>
#include  <cpu.h>
#include  <lib_def.h>
#include  <lib_ascii.h>
#include  <lib_math.h>
#include  <lib_mem.h>
#include  <lib_str.h>
//UCOS3 封装层相关文件

#include  "board.h"

#include "ErigerConfig.h"


// 是否使用QP状态机
#ifdef    USE_QPC_MODULE
#include  "qpc.h"
#include "ErigeEvent.h"

#endif

 #define ATOMIC_SET_BIT(REG, BIT)                             \
  do {                                                       \
    uint32_t val;                                            \
    do {                                                     \
      val = __LDREXW((__IO uint32_t *)&(REG)) | (BIT);       \
    } while ((__STREXW(val,(__IO uint32_t *)&(REG))) != 0U); \
  } while(0)

#define ATOMIC_CLEAR_BIT(REG, BIT)                           \
  do {                                                       \
    uint32_t val;                                            \
    do {                                                     \
      val = __LDREXW((__IO uint32_t *)&(REG)) & ~(BIT);      \
    } while ((__STREXW(val,(__IO uint32_t *)&(REG))) != 0U); \
  } while(0)

#define ATOMIC_MODIFY_REG(REG, CLEARMSK, SETMASK)                          \
  do {                                                                     \
    uint32_t val;                                                          \
    do {                                                                   \
      val = (__LDREXW((__IO uint32_t *)&(REG)) & ~(CLEARMSK)) | (SETMASK); \
    } while ((__STREXW(val,(__IO uint32_t *)&(REG))) != 0U);               \
  } while(0)

#define ATOMIC_WRITE(ADDR,DATA)                                             \
do{                                                                         \
    do{                                                                     \
        val = __LDREXW((__IO uint32_t *)(ADDR));                            \
    }while((__STREXW(DATA,(__IO uint32_t *)(ADDR))) != 0U);					\
}while(0)


/*
*********************************************************************************************************
*                                         STANDARD LIBRARIES
*********************************************************************************************************
*/

#include  <stdarg.h>
#include  <stdio.h>
#include <stdint.h>
#include  <stdbool.h>
#include  <stdlib.h>
#include  <math.h>
#include  <string.h>


/*
*********************************************************************************************************
*                                              LIBRARIES
*********************************************************************************************************
*/

/*数据结构相关操作头文件*/
#include    "list.h"
#include    "queue.h"
#include    "stack.h"
#include    "circular-list.h"
#include    "dbl-list.h"
#include    "dbl-circ-list.h"


/*
*********************************************************************************************************
*                                              APP / BSP
*********************************************************************************************************
*/
//#include  "board.h"

#include "bsp_algorithm.h"
#include "bsp_errorlist.h"
#include "App_ComFun.h"
#include "Task_Start.h"
#include "Can_Rx_Task.h"
#include "Can_IV_Task.h"
#include "Tcp_Server_Task.h"
#include "Data_Distribution_Task.h"
#include "Pedal_Task.h"
#include "Normal_Task.h"
#include "Engineering_Task.h"
#include "Fw_DownLoad_Task.h"
#include "Flash_Task.h"
#include "instruction_task.h"   
#include "Pedal_User_Task.h"

#include "bsp_abstractPrsureSen.h"
#include "bsp_valve.h"
#include "bsp_adc.h"
#include "bsp_timer1_pwm.h"

// QPC


/*
*********************************************************************************************************
*                                            INCLUDES END
*********************************************************************************************************
*/



#endif
