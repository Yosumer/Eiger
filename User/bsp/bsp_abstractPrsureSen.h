/*** 
 * @Author: sum 476328608@qq.com
 * @Date: 2023-09-28 15:01:44
 * @LastEditors: sum 476328608@qq.com
 * @LastEditTime: 2023-11-13 13:47:34
 * @FilePath: \elabc:\sierra\LiquidModule\Liquid_Cj\code\phaco\Source\bsp\bsp_abstractPrsureSen.h
 * @Description: 
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef _BSP_ABSTRACTPRSURESEN_H
#define _BSP_ABSTRACTPRSURESEN_H
#include <stdint.h>
#include "bsp_abstractLineFunc.h"
typedef enum {
	VIT_PRESSURE1,	   //波切气压传感器1
	VIT_PRESSURE2,	   //波切气压传感器2
	VACCUM_PRESSURE,   //气路负压传感器
	SENSOR_MAX
}PressureChannel;

// 当C为0时使用的拟合参数是线性拟合 反之使用的是2次函数
// typedef struct {
//     float A;    //斜率或者2次项
//     float B;    //截距或者3次项
//     float C;    //截距
// }CalibrationPara;   //线性校准参数

typedef uint32_t (*readAd)(PressureChannel channel);    //获取AD值 根据AD值进行传感器的计算
typedef struct {
    CalibrationPara 	calib;      //校准参数
    readAd          	read;
	PressureChannel 	channel;    //AD使用的通道
}PressureSensor_t;

void bsp_prsureSen_Init(PressureSensor_t *sensor,readAd read);
void bsp_prsureSen_SetCalib(PressureSensor_t *sensor,CalibrationPara _cal);
CalibrationPara* bsp_prsureSen_GetCalib(PressureSensor_t *sensor);
uint32_t bsp_prsureSen_ReadAD(PressureSensor_t *sensor);
float bsp_prsureSen_GetPressure(PressureSensor_t *sensor);
void bsp_prsureSen_ZeroAdjustment(PressureSensor_t *sensor);
#endif