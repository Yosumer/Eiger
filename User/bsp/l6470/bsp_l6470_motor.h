/*** 
 * @Author: sum 476328608@qq.com
 * @Date: 2024-05-29 16:10:12
 * @LastEditors: sum 476328608@qq.com
 * @LastEditTime: 2024-05-31 13:10:56
 * @FilePath: \Fluid module 模拟IOP 开放FPUc:\RT-ThreadStudio\workspace\Template\User\bsp\l6470\bsp_l6470_motor.h
 * @Description: 
 * @
 * @Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
/*------------------------------------------------------------------------------
 * @file    BSP_L6470_MOTOR.H
 * @author  SIERRA TEAM 
 * @date    2024/05/29 16:16:07
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
#ifndef __BSP_L6470_MOTOR_H
#define __BSP_L6470_MOTOR_H

/* Files includes  -----------------------------------------------------------*/
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdint.h>

#include "L6470.h"
/* Defines -------------------------------------------------------------------*/
typedef enum {
    MOTOR_IDLE,
    MOTOR_ACC,
    MOTOR_DEC,
    MOTOR_RUN,
    MOTOR_ERROR
}motor_l6470_state;


typedef enum {
    CW,         
    CWW
}motor_l6470_dir;

typedef struct 
{
    motor_l6470_dir     dir;
    uint16_t            statusRe;       //芯片状态寄存器
    rt_int32_t          setcoor;        //设置位置
    float               speed;          //电机运行速度
    float               acc;            //电机加速度
    float               dec;            //电机减速度
}motor_l6470_info;


struct motor_l6470_device
{
    struct rt_spi_device*  _device;
    rt_base_t flag_gpio_pin;        //错误标志位
    rt_base_t busy_gpio_pin;        //电机忙标志位
    rt_base_t reset_gpio_pin;       //电机复位引脚
    rt_base_t cs_gpio_pin;          //电机片选引脚
};

struct motor_l6470
{
    struct motor_l6470_device dev;  
    motor_l6470_info    info;
    MotorParameterData_t        *motorparameter;        //电机初始化时的配置参数
    StepperMotorDriverHandle_t  *handler;               //电机操作句柄
};



/* Variables -----------------------------------------------------------------*/


/* Functions ----------------------------------------------------------------*/

struct motor_l6470 * bsp_l6470_motor_create(const char *spi_bus_name,const char *spi_dev_name,
                                            rt_base_t cs_pin,rt_base_t flag_pin,rt_base_t reset_pin,rt_base_t busy_pin);
void bsp_l6470_motor_config(struct motor_l6470 * _me,MotorParameterData_t *_motorParam);
void bsp_l6470_motor_setRunData(struct motor_l6470 * _me,float _speed,float _acc,float _dec);
void bsp_l6470_motor_run(struct motor_l6470 * _me,motor_l6470_dir _dir,float _speed);
void bsp_l6470_motor_move(struct motor_l6470 * _me,motor_l6470_dir _dir,uint32_t step);
void bsp_l6470_motor_goto(struct motor_l6470 * _me,uint32_t _abspos);
void bsp_l6470_motor_softStop(struct motor_l6470 * _me);
void bsp_l6470_motor_softHiZ(struct motor_l6470 * _me);
void bsp_l6470_motor_hardStop(struct motor_l6470 * _me);
void bsp_l6470_motor_hardHiZ(struct motor_l6470 * _me);
void bsp_l6470_motor_resetPos(struct motor_l6470 * _me);
void bsp_l6470_motor_resetDev(struct motor_l6470 * _me);

motor_l6470_state bsp_l6470_motor_getState(struct motor_l6470 * _me);
rt_int32_t  bsp_l6470_motor_getCoor(struct motor_l6470 * _me);
void  bsp_l6470_motor_setPos(struct motor_l6470 * _me, int pos);
float bsp_l6470_motor_getSpeed(struct motor_l6470 * _me);
motor_l6470_dir bsp_l6470_motor_getDir(struct motor_l6470 * _me);
#endif
