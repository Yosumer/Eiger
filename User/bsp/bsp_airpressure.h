/*** 
 * @Author: sum 476328608@qq.com
 * @Date: 2023-09-28 15:01:44
 * @LastEditors: sum 476328608@qq.com
 * @LastEditTime: 2024-01-09 10:59:14
 * @FilePath: \elabc:\sierra\LiquidModule\Liquid_Cj\code\phaco\Source\bsp\bsp_airpressure.h
 * @Description: 
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef _BSP_AIRPRESSURE_H
#define _BSP_AIRPRESSURE_H

#include <stdint.h>
#include "bsp_pid.h"
typedef struct _bsp_airpressure_ops
{
    void (*inValve)(uint16_t pwm);  //进气阀  
    void (*outValve)(uint16_t pwm); //出气阀
    float (*GetPressureSensor)();   //获取传感器的值
    void (*task)(void * _me);       //特殊处理函数
}bsp_airpressure_ops;


typedef struct 
{
    int     target;         //目标压力 0~120mm
    bsp_airpressure_ops *_ops;
    struct  PID pid;        //pid控制器
    float       out;        //pid的输出
    float       error;      //pid误差
    float       minout;     //最小输出
    float       maxout;     //最大输出
}bsp_airpressure_t; 

void bsp_airpressure_Init(bsp_airpressure_t *_this,bsp_airpressure_ops *_ops);
void bsp_airpressure_IncPidInit(bsp_airpressure_t *_this,bsp_airpressure_ops *_ops);
void bsp_airpressure_SetCtrlPrm(bsp_airpressure_t *_this,float  kp,  float  ki,  float  kd);
void bsp_airpressure_SetIntegralPrm(bsp_airpressure_t *_this,float  integral_up,  float  integral_low);
void bsp_airpressure_SetTarget(bsp_airpressure_t *_this,int _target);
void bsp_airpressure_SetOut(bsp_airpressure_t *_this,float _min,float _max);
void bsp_airpressure_Handle(bsp_airpressure_t *_this);
#endif