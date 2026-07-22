/*
 * @Author: sum 476328608@qq.com
 * @Date: 2023-09-28 15:01:44
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-08-19 09:17:41
 * @FilePath: \elabc:\sierra\LiquidModule\Liquid_Cj\code\phaco\Source\bsp\bsp_airpressure.c
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */

#include <rtthread.h>
#include "bsp_airpressure.h"
#define LOG_TAG            "drv.air"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>

/**
 * @description: 气路控制初始化
 * @param {bsp_airpressure_t} *_this
 * @param {bsp_airpressure_ops} *_ops
 * @return {*}
 */
void bsp_airpressure_Init(bsp_airpressure_t *_this,bsp_airpressure_ops *_ops)
{
    RT_ASSERT(_this);
    RT_ASSERT(_ops)

    _this->_ops = _ops;
    // 1.构建PID参数
    PID_Constructor(&_this->pid);
    // 2.设置PID的积分限幅 
    _this->pid.SetIntegralPrm(&_this->pid,1000,0);
    // 3.设置默认PID参数
    _this->pid.SetCtrlPrm(&_this->pid,1,1,0);
    // 4.设置最小最大输出
    _this->minout = 0;
    _this->maxout = 65535;
}

void bsp_airpressure_IncPidInit(bsp_airpressure_t *_this,bsp_airpressure_ops *_ops)
{
    RT_ASSERT(_this);
    RT_ASSERT(_ops)

    _this->_ops = _ops;
    // 1.构建PID参数
    PID_Constructor_Inc(&_this->pid);
    // 2.设置PID的积分限幅 
    _this->pid.SetIntegralPrm(&_this->pid,1000,0);
    // 3.设置默认PID参数
    _this->pid.SetCtrlPrm(&_this->pid,1,1,0);
    // 4.设置最小最大输出
    _this->minout = 0;
    _this->maxout = 65535;
}
/**
 * @description: 设置气路控制PID参数
 * @param {bsp_airpressure_t} *_this 气路结构体对象
 * @param {float } kp   
 * @param {float } ki
 * @param {float } kd
 * @return {*}
 */
void bsp_airpressure_SetCtrlPrm(bsp_airpressure_t *_this,float  kp,  float  ki,  float  kd)
{
    if(_this == NULL)
        return;
    _this->pid.SetCtrlPrm(&_this->pid,kp,ki,kd);
}
void bsp_airpressure_SetIntegralPrm(bsp_airpressure_t *_this,float  integral_up,  float  integral_low)
{
    if(_this == NULL)
        return;
   _this->pid.SetIntegralPrm(&_this->pid,integral_up,integral_low);
}
/**
 * @description: 设置气路pid控制目标值 单位mmHg （0~120）
 * @param {bsp_airpressure_t} *_this
 * @param {int} _target
 * @return {*}
 */
void bsp_airpressure_SetTarget(bsp_airpressure_t *_this,int _target)
{
    if(_this == NULL)
        return;
    _this->target=_target;
}
/**
 * @description: 设置PID最小最大输出
 * @param {bsp_airpressure_t} *_this
 * @param {float} _min
 * @param {float} _max
 * @return {*}
 */
void bsp_airpressure_SetOut(bsp_airpressure_t *_this,float _min,float _max)
{
    if(_this == NULL)
        return;
    _this->minout = _min;
    _this->maxout = _max;
    _this->pid.SetIntegralPrm(&_this->pid,_max,_min);
}
/**
 * @description: 气路控制处理对象 需要初始化后在任务中调用该函数
 * @param {bsp_airpressure_t} *_this
 * @return {*}
 */
// void bsp_airpressure_Handle(bsp_airpressure_t *_this)
// {
//     if(_this == NULL || _this->_ops == NULL)
//         return;
//     if(_this->_ops->task)   //特殊处理函数不为空
//         _this->_ops->task(_this);
//     else        //使用公用处理流程
//     {
//         // 1.获取当前压力   
//         float crut_pressure = _this->_ops->GetPressureSensor();
//         // 2.PID 计算
//         _this->error  = _this->target - crut_pressure;
//         _this->out = _this->pid.PID(&_this->pid,_this->error);
//         if(_this->out>=_this->maxout)
//             _this->out = _this->maxout;
//         if(_this->out<=_this->minout)
//             _this->out = _this->minout;
//         // 3.根据PID进行比例阀控制
//         uint16_t _in = (uint16_t)_this->out;
//         _this->_ops->inValve(_in);
//         // 判断是否使用out
//         if(_this->_ops->outValve !=NULL)
//         {
//             // float _out = (_this->pid.pri.integral_up - _this->out);
					
//             uint16_t _out = (uint16_t)(_this->out/2);
						
// 						// uint16_t _out  = _this->out;
//             _this->_ops->outValve(_out);
//         }
//     }
// }


void bsp_airpressure_Handle(bsp_airpressure_t *_this)
{
    if(_this == NULL || _this->_ops == NULL)
        return;
    if(_this->_ops->task)   //特殊处理函数不为空
        _this->_ops->task(_this);
    else        //使用公用处理流程
    {
        // 1.获取当前压力   
        float crut_pressure = _this->_ops->GetPressureSensor();
        // 2.PID 计算
        _this->error  = _this->target - crut_pressure;
        _this->out = _this->pid.PID(&_this->pid,_this->error);
        if(_this->out>=_this->maxout)
            _this->out = _this->maxout;
        if(_this->out<=_this->minout)
            _this->out = _this->minout;
        // 3.根据PID进行比例阀控制
        uint16_t _in = (uint16_t)_this->out;
        // 判断是否使用out
        if(_this->_ops->outValve !=NULL)
        {
            uint16_t _out = (uint16_t)(_this->out/1.5);
            _this->_ops->inValve(_in);

            _this->_ops->outValve(_out);
        }
        else
        {
            _this->_ops->inValve(_in);
        }
    }
}