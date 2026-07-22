/*** 
 * @Author: sum 476328608@qq.com
 * @Date: 2024-07-18 09:53:39
 * @LastEditors: sum 476328608@qq.com
 * @LastEditTime: 2024-07-18 16:53:04
 * @FilePath: \light\User\bsp\opt\bsp_diathermy.h
 * @Description: 
 * @
 * @Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
/*------------------------------------------------------------------------------
 * @file    BSP_DIATHERMY.H
 * @author  SIERRA TEAM 
 * @date    2024/07/12 14:27:02
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
#ifndef __BSP_DIATHERMY_H
#define __BSP_DIATHERMY_H

/* Files includes  -----------------------------------------------------------*/
#include <stdint.h>
#include <board.h>
#include "bsp_abstractLineFunc.h"
/* Defines -------------------------------------------------------------------*/


/* Variables -----------------------------------------------------------------*/
// typedef struct {
//     float A;    //斜率或者2次项
//     float B;    //截距或者3次项
//     float C;    //截距
// }CalibrationPara;   //线性校准参数

typedef struct _bsp_dia_ops
{
    void  (*init)(uint16_t freq);                        //硬件初始化
    uint16_t (*get_vol)(void);                  //获取电压ADC
    uint16_t (*get_curr)(void);                 //获取电流ADC
    void  (*set_dac)(uint16_t _dacvalve);       //设置输出
    void  (*enable)(void *_ptr);                          //使能输出设置的功率
    void  (*disable)(void *_ptr);                         //失能只是不输出但是设置的功率还在
}bsp_dia_ops;

typedef struct _bsp_dia_info
{
    uint32_t        _dacval;    //设置的DAC数值
    float           _setpow;    //设置功率
    float           _calpow;    //计算功率
    float           _res;       //计算电阻
    float           _vol;       //测量电压
    float           _current;   //测量电流
}bsp_dia_info;

typedef struct{
    CalibrationPara _vcal;      //采集电压校准参数
    CalibrationPara _ical;      //电流校准参数
    CalibrationPara _outcal;    //输出功率校准参数
	float 			_outMax;	//最大功率限幅
	uint32_t    	_diaFreq;
}dia_cal_param;

typedef struct _bsp_dia_t
{
	#if 0
    CalibrationPara _vcal;      //采集电压校准参数
    CalibrationPara _ical;      //电流校准参数
    CalibrationPara _outcal;    //输出功率校准参数
	
	float 			_outMax;	//最大功率限幅
	uint32_t    	_diaFreq;
	#endif
	
	dia_cal_param	*_cal;
	uint8_t 		_adcUpdate; //adc值实时更新 用于辅助电凝硬件测试
	
    bsp_dia_ops     *_ops;
    bsp_dia_info    _info;
    uint8_t         _enable;   //输出状态
}bsp_dia_t;

extern bsp_dia_t   _dia_instance;

/* Functions ----------------------------------------------------------------*/

void bsp_dia_init(bsp_dia_t *_this,bsp_dia_ops *_ops,CalibrationPara _vcal,CalibrationPara _ical,CalibrationPara _outcal,float _outMax,uint16_t freq);
void bsp_dia_setPower(bsp_dia_t *_this,float _power);
void bsp_dia_setDac(bsp_dia_t *_this, uint16_t _dacvalve);
void bsp_dia_set_Vol_Calib(bsp_dia_t *_this,CalibrationPara _vcal);
void bsp_dia_set_Curr_Calib(bsp_dia_t *_this,CalibrationPara _ical);
void bsp_dia_set_Out_Calib(bsp_dia_t *_this,CalibrationPara _outcal);
bsp_dia_info bsp_dia_getInfo(bsp_dia_t *_this);

void bsp_dia_test_io(uint8_t _io,uint8_t _en);

void set_dia_freq(uint16_t freq);
void save_dia_para(void);
extern void bsp_dia_default(void);
extern void bsp_dia_register(dia_cal_param *instance);
#endif
