
/*** 
 * @Author: sum 476328608@qq.com
 * @Date: 2023-09-27 11:11:27
 * @LastEditors: sum 476328608@qq.com
 * @LastEditTime: 2023-11-13 15:27:17
 * @FilePath: \elabc:\sierra\LiquidModule\Liquid_Cj\code\phaco\Source\bsp\bsp_prsurSen.h
 * @Description: 
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef _BSP_PRSURSEN_H
#define _BSP_PRSURSEN_H

#include "bsp_abstractPrsureSen.h"
extern PressureSensor_t SensorTable[SENSOR_MAX];    //废液腔液位
void bsp_prsurSen_Init(CalibrationPara * caliTable);
float bsp_prsurSen_LinearFit(float x[],float y[],int n,float *A,float *B);          //线性拟合
uint16_t readAD(PressureChannel channel);
#endif