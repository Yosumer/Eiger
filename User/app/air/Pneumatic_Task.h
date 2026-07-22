/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-04 13:21:02
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-05-14 11:52:03
 * @FilePath: \EigeV1.0.01\User\app\air\Pneumatic_Task.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __PNEUMATIC_TASK_H
#define __PNEUMATIC_TASK_H


#include <stdint.h>
#include "bsp_abstractPrsureSen.h"
typedef struct _Pneumatic_Task_Cof
{
    uint32_t    _vitduty;      //波切的占空比（内部使用）
    uint32_t    _pumbduty;     //隔膜泵控制占空比

    uint32_t    _en;        //功能是否使能
    uint32_t    _pedalcof;  //气路模块响应踏板
    uint32_t    _start;     //起始值
    uint32_t    _end;       //终止值        //0x26
}Pneumatic_Task_Cof;



typedef struct _Pneumatic_Task_Data
{
    uint32_t    _errCode;              //错误代码
    uint32_t    _vitrawdata[2];         //波切ADC原值
    float       _vitpressure[2];       //俩口的控制压力实时值
    float       _vitmaxpressure[2];    //换相时最大压力值
    uint32_t    _percentofPedal;       //踏板百分比 
    uint32_t    _ctlValue;             //控制值（读取限制值）   
}Pneumatic_Task_Data;


typedef struct _Pneumatic_Task_t
{
    PressureSensor_t    pressureSensorUp;   //上传感器
    PressureSensor_t    pressureSensorDown;  //下传感器
    CalibrationPara     *SensorUpcali;      //上传感器校准参数
    CalibrationPara     *SensorDowncali;     //下传感器校准参数
    void (*Vit_CtlCmp)(uint16_t cmp); 
    void(*En_Callback)(uint32_t _en);   //使能失能回调函数
    Pneumatic_Task_Cof *_cof;
    Pneumatic_Task_Data *_data;  //数据结构体指针
}Pneumatic_Task_t;



int Pneumatic_Task_Init(void);
void Pneumatic_Task(void *arg);
void Pneumatic_Task_SetWcof(Pneumatic_Task_Cof *_cof);
void Pneumatic_Task_SetData(Pneumatic_Task_Data *_data);
void Pneumatic_Pedal_Deal(uint16_t _code,uint32_t _data);
void Pneumatic_Pedal_Disconnect();


#endif // !__PNEUMATIC_TASK_H
