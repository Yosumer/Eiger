/*
 * @Author: sum 476328608@qq.com
 * @Date: 2023-09-28 15:01:44
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-03-26 16:26:27
 * @FilePath: \Fluid module 模拟IOP 开放FPUc:\sierra\PhacoTest\code\phaco\Source\bsp\bsp_abstractPrsureSen.c
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#include <stddef.h>
#include "bsp_abstractPrsureSen.h"

#define __DIS_IRQ() __disable_irq()
#define __ENABLE_IRQ() __enable_irq() 

void bsp_prsureSen_Init(PressureSensor_t *sensor,readAd _read)
{
    if(sensor == NULL)
        return;
    sensor->read = _read;
    // 设置默认参数
    sensor->calib.A = 1.0;
    sensor->calib.B = 0;
    sensor->calib.C = 0;
}
void bsp_prsureSen_SetCalib(PressureSensor_t *sensor,CalibrationPara _cal)
{
    if (sensor == NULL)
        return;
    sensor->calib = _cal;
}

CalibrationPara* bsp_prsureSen_GetCalib(PressureSensor_t *sensor)
{
    if (sensor == NULL)
        return NULL;
    return &sensor->calib;
}
void bsp_prsureSen_ZeroAdjustment(PressureSensor_t *sensor)
{
    if (sensor == NULL)
        return;
    //1 获取大气下的压力
    float _airpressuer = bsp_prsureSen_GetPressure(sensor);
    // 2 获取该传感器的校准值
    CalibrationPara* _cal = bsp_prsureSen_GetCalib(sensor);
    // 3 校准该传感器的零点
    __DIS_IRQ();
    _cal->B -= _airpressuer;    //调整截距
    __ENABLE_IRQ();
}
uint32_t bsp_prsureSen_ReadAD(PressureSensor_t *sensor)
{
    if (sensor == NULL)
        return 0;
    return sensor->read(sensor->channel);
}
float bsp_prsureSen_GetPressure(PressureSensor_t *sensor)
{
    if (sensor == NULL)
        return 0;
    float _readSensor;
    if(sensor->calib.C == 0)    //使用的是线性拟合
        _readSensor = sensor->calib.A * (sensor->read(sensor->channel)) + (sensor->calib.B);
    else    //使用的是2次拟合
    {
        int x = (sensor->read(sensor->channel));
        _readSensor = sensor->calib.A * (x*x) + ((sensor->calib.B)*x)+(sensor->calib.C);
    }
    return _readSensor;
}