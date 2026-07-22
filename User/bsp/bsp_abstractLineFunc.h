/*------------------------------------------------------------------------------
 * @file    BSP_ABSTRACTLINEFUNC.H
 * @author  SIERRA TEAM 
 * @date    2024/07/16 13:01:02
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
#ifndef __BSP_ABSTRACTLINEFUNC_H
#define __BSP_ABSTRACTLINEFUNC_H

/* Files includes  -----------------------------------------------------------*/
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/
typedef struct {
    float A;    //斜率或者2次项
    float B;    //截距或者3次项
    float C;    //截距
}CalibrationPara;   //线性校准参数

/* Variables -----------------------------------------------------------------*/

/* Functions ----------------------------------------------------------------*/
float       bsp_abstractLineFunc_GetOut(uint32_t _x,CalibrationPara *_cali);    //根据输出的x获取Y

uint32_t 	bsp_abstractLineFuncBeta_GetOut(float _x,CalibrationPara *_cali);

uint32_t    bsp_abstractLineFunc_SolveX(float _y,CalibrationPara *_cali);       //根据输出y解方程

int 		bsp_isCalParamEmpty(CalibrationPara *_cali);

#endif
