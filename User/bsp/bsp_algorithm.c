/*
 * @Author: sum 476328608@qq.com
 * @Date: 2023-11-22 10:59:54
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-08-07 13:40:38
 * @FilePath: \Fluid module 模拟IOP 开放FPUc:\sierra\LiquidModule\Liquid_UCOSV3.08\code\phaco\Source\bsp\bsp_algorithm.c
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */

/*------------------------------------------------------------------------------
 * @file    BSP_ALGORITHM.C
 * @author  FIRMWARE TEAM 
 * @date    2023/11/22 10:59:56
 * @brief   THIS FILE PROVIDES ALL THE FIRMWARE FUNCTIONS.
 * -----------------------------------------------------------------------------
 * @attention 
 
 * THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE 
 * CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE 
 * TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR 
 * CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH 
 * HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN 
 * CONNECTION WITH PRODUCTS MADE BY CUSTOMERS. 
 * 
 * <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2> 
------------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion  ------------------------------------*/
#define _BSP_ALGORITHM_C_

/* Files includes  -----------------------------------------------------------*/
#include "bsp_algorithm.h"
#include <string.h>
static void swap16(uint16_t *a,uint16_t *b)
{
    uint16_t _temp = *a;
    *a = *b;
    *b = _temp;
}
static void swap_f(float *a,float *b)
{
    float _temp = *a;
    *a = *b;
    *b = _temp;
}
uint16_t bsp_mid_filt16(uint16_t *data,uint16_t size)
{
    uint16_t _temp[size];

    // 复制数组
    for (int i = 0; i < size; i++)
    {
        _temp[i] = data[i];
    }
    // 冒泡排序
    for (int i = 0; i < size -1; i++)
    {
        for (int j = 0; j < size -1 -i; j++)
        {
            if(_temp[j] > _temp[j+1])
                swap16(&_temp[j],&_temp[j+1]);
        }
    }
    
    if(size % 2 == 1)
        return _temp[size/2];
    else
        return ((_temp[size/2]+_temp[(size/2)-1])/2);
}
float bsp_mid_filt_f(float *data,uint16_t size)
{
    float _temp[10];

    // 复制数组
    for (int i = 0; i < size; i++)
    {
        _temp[i] = data[i];
    }
    // 冒泡排序
    for (int i = 0; i < size -1; i++)
    {
        for (int j = 0; j < size -1 -i; j++)
        {
            if(_temp[j] > _temp[j+1])
                swap_f(&_temp[j],&_temp[j+1]);
        }
    }
    
    if(size % 2 == 1)
        return _temp[size/2];
    else
        return ((_temp[size/2]+_temp[(size/2)-1])/2); 
}
/*----------------------------------------------------------------------------*/

void  bsp_avg_Init(algorithm_avg *_this,float d)
{
    if(_this == NULL)
        return;
    _this->read = 0;
    _this->write =0;
    memset(_this->data,(int)d,AVG_POION_NUM);
}
void  bsp_avg_push(algorithm_avg *_this,float d)
{
    _this->data[_this->write] = d;
    _this->write++;
    _this->write =  _this->write % AVG_POION_NUM;
}
void  bsp_avg_pop(algorithm_avg *_this)
{
    _this->read++;
    _this->read = _this->read % AVG_POION_NUM;
}
float bsp_avg_getAvg(algorithm_avg *_this)
{
    // float sum = 0;
    // for(int i=0; i< AVG_POION_NUM;i++)
    // {
    //     sum += _this->data[i];
    // }
    // return sum/AVG_POION_NUM;

    return bsp_mid_filt_f(_this->data,AVG_POION_NUM);
}
float bsp_avg_getData(algorithm_avg *_this,float d)
{
    bsp_avg_push(_this,d);
    return bsp_avg_getAvg(_this);
}


uint8_t bsp_algo_isInRange(float value, float min, float max)
{
     return (value >= min && value <= max);
}