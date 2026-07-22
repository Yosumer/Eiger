/*------------------------------------------------------------------------------
 * @file    BSP_ABSTRACTLINEFUNC.C
 * @author  FIRMWARE TEAM
 * @date    2024/07/16 13:09:28
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


/* Files includes  -----------------------------------------------------------*/
#include "bsp_abstractLineFunc.h"
#include <math.h>
#define LOG_TAG            "drv.Line"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>
/*----------------------------------------------------------------------------*/
//根据输出的x获取Y
float bsp_abstractLineFunc_GetOut(uint32_t _x,CalibrationPara *_cali)
{
    if(_cali->C == 0)
        return (_x*_cali->A+_cali->B);
    return (_x*_x*_cali->A+_cali->B*_x+_cali->C);
}


//根据输出的x获取Y
uint32_t bsp_abstractLineFuncBeta_GetOut(float _x,CalibrationPara *_cali)
{
    if(_cali->C == 0)
        return (_x*_cali->A+_cali->B);
    return (_x*_x*_cali->A+_cali->B*_x+_cali->C);
}


uint32_t    bsp_abstractLineFunc_SolveX(float _y,CalibrationPara *_cali)
{
    uint32_t _ret;
    if(_cali->C == 0)   //使用的是线性拟合
    {
        // _dav_val = _y*_cali->A + _cali->B; //_y = A*_dac+B； dac = (_y - B)/A
        _ret = (_y - _cali->B) / _cali->A;
    }
    else    //使用的是二次拟合
    {
        float   _delta; //
        float   _valve1;    //2次函数2个解值
        float   _cvalve = (_cali->C - _y);
        _delta = _cali->B * _cali->B - 4 * _cali->A * _cvalve;

        if(_delta > 0)  //双根
        {
            _valve1 = (-_cali->B - sqrtf(_delta))/(2*_cali->A);    //只需要正值
        }
        else if (_delta == 0)   //重根
        {
            _valve1 = -_cali->B/(2*_cali->A);
        }
        else    //负数根
        {
            LOG_E("resolution power error");
        }
        _ret= _valve1;
    }
    return _ret;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_isCalParamEmpty
*	功能说明: 判断数据是否为空
*	形    参：_cali 校准结构体
*	返 回 值: 0 空 1 非空
*********************************************************************************************************
*/
int bsp_isCalParamEmpty(CalibrationPara *_cali)
{
    if((0.0f == _cali->A)&&(0.0f == _cali->B)&&(0.0f == _cali->C) )
        return 0;

    return 1;

}


