
/*------------------------------------------------------------------------------
 * @file    BSP_ABSTRACTENCODE.C
 * @author  FIRMWARE TEAM 
 * @date    2024/01/16 14:20:25
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
#define _BSP_ABSTRACTENCODE_C_

/* Files includes  -----------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "bsp_abstractEncode.h"
/**
 * @description: 初始化编码器
 * @param {abstractEncoder_t} *_this 结构体指针
 * @param {hw_inter_t} *_hw 底层接口函数需要在自己实现
 * @return {*}
 */
void bsp_abstractEncode_Init(abstractEncoder_t *_this,hw_inter_t *_hw,Encoder_conf_t *_conf)
{
    if( NULL == _this || NULL == _hw)
        return;
    _this->hw_inter = _hw;
    _this->hw_inter->_Init();       //底层初始化
    if(_conf == NULL)           
    {
        // 初始化编码器默认配置
        _this->cof._time = 100; //100ms
        _this->cof._resolution = 1000;
        _this->cof._Counting_mode = COUNTING_MODE_4;
        _this->cof._scale = 0.4;        // 电机一圈1600个脉冲 / 编码器按照上述配置一圈4000
    }
    else
    {
        memcpy(&_this->cof,_conf,sizeof(Encoder_conf_t));   //使用外部参数
    }
    bsp_abstractEncode_Reset(_this);
}
/**
 * @description: 配置编码器
 * @param {abstractEncoder_t} *_this 结构体指针
 * @param {Encoder_conf_t} _cof 配置参数
 * @return {*}
 */
void bsp_abstractEncode_SetConf(abstractEncoder_t *_this,Encoder_conf_t _cof)
{
    if( NULL == _this)
        return;
    memcpy(&_this->cof,&_cof,sizeof(Encoder_conf_t));
}
void bsp_abstractEncode_IQRHandler(abstractEncoder_t *_this)
{
    if( NULL == _this)
        return;
    if(bsp_abstractEncode_GetDir(_this) == ENCODER_DIR_CW)
        _this->over_count++;
    else if(bsp_abstractEncode_GetDir(_this)== ENCODER_DIR_CWW)
        _this->over_count--;
}
void bsp_abstractEncode_Task(abstractEncoder_t *_this)
{
    volatile int32_t count_per_unit;
    float _r_s;
    _this->dir = bsp_abstractEncode_GetDir(_this);
    _this->all_count = bsp_abstractEncode_GetCount(_this);
    count_per_unit = _this->all_count - _this->last_count;
    // _r_s = (float)count_per_unit/(_this->cof._resolution * _this->cof._Counting_mode)*(1000/_this->cof._time);
    _r_s = (float)(count_per_unit/ _this->cof._Counting_mode)*_this->cof._scale *(1000/_this->cof._time);
    // 0.1s * 6.283185 * 0.08123627136 = 0.56745055984 rad/s
    // 0.56745055984 rad/s * 60 * 0.01745329252 = 10.000000000018 rad/min
    // _this->vol = (6.283185f * _r_s)*(60*0.08123627136f);
    _this->vol = _r_s *60;  // r/min;
    _this->last_count = _this->all_count;
    _this->pos = (float)(_this->last_count/ _this->cof._Counting_mode)* _this->cof._scale;
}
void  bsp_abstractEncode_Reset(abstractEncoder_t *_this)
{
    if( NULL == _this)
        return;
    _this->hw_inter->_setCounter(0);
    _this->over_count = 0;
    _this->pos = 0;
    _this->last_count = 0;
    _this->all_count = 0;

}
uint8_t bsp_abstractEncode_GetDir(abstractEncoder_t *_this)
{

    if(_this->hw_inter->_getDir() == ENCODER_DIR_CW)
        _this->dir = ENCODER_DIR_CW;
    else if(_this->hw_inter->_getDir() == ENCODER_DIR_CWW)
        _this->dir = ENCODER_DIR_CWW;
}
float bsp_abstractEncode_GetVol(abstractEncoder_t *_this)
{
    
    return _this->vol;
}
int32_t bsp_abstractEncode_GetPos(abstractEncoder_t *_this)
{

    return _this->pos;
}
int32_t bsp_abstractEncode_GetCount(abstractEncoder_t *_this)
{
    return _this->hw_inter->_getCounter() + _this->over_count * _this->cof._resolution;
}
/*----------------------------------------------------------------------------*/

