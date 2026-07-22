
/*------------------------------------------------------------------------------
 * @file    BSP_FAN.H
 * @author  SIERRA TEAM 
 * @date    2024/07/02 16:21:45
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
#ifndef __BSP_FAN_H
#define __BSP_FAN_H

/* Files includes  -----------------------------------------------------------*/
#include "bsp_pid.h"

/* Defines -------------------------------------------------------------------*/

typedef struct _fan_ops                //对下层的封装
{
    void (*fan_init)();          //初始化
    uint32_t (*get_raw_cnt)();      //获取原始计数
    float (*get_temp)();            //获取温度
    void (*set_pwm)(uint8_t pwm);    //设置PWM
}fan_ops;

typedef struct _bsp_fan_t
{
    float   _set_temp;    //设定温度
    uint32_t _set_freq;       //捕获定时器设定频率
    uint16_t _rmp_r;  //RMP值
    float    _temp_r; //温度值
    struct _fan_ops *_ops;
    struct PID _pid;     //pid控制器
    float   _error;      //pid误差
    float   _minout;     
    float   _maxout;
    uint8_t _out;       //占空比
    uint8_t _enable;    //使能
}bsp_fan_t;

/* Variables -----------------------------------------------------------------*/


/* Functions ----------------------------------------------------------------*/
void bsp_fan_Init(struct _bsp_fan_t *_fan, uint32_t _freq, struct _fan_ops *_ops);
void bsp_fan_SetCtlPid(struct _bsp_fan_t *_fan,float  kp,  float  ki,  float  kd);
void bsp_fan_SetTargetTemp(struct _bsp_fan_t *_fan, float _temp);
float bsp_fan_GetTemp(struct _bsp_fan_t *_fan);
uint16_t bsp_fan_GetRmp(struct _bsp_fan_t *_fan);
void bsp_fan_Handler(struct _bsp_fan_t *_fan);
void bsp_fan_Enable(struct _bsp_fan_t *_fan);
void bsp_fan_Disable(struct _bsp_fan_t *_fan);
#endif
