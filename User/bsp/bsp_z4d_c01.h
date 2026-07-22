/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-04 13:21:02
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-03-27 19:26:21
 * @FilePath: \EigeV1.0.01\User\bsp\bsp_z4d_c01.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#ifndef __BSP_Z4D_C01_H__
#define __BSP_Z4D_C01_H__

#include <stdint.h>



typedef struct _bsp_z4dc01
{
    uint8_t _status;
    uint16_t _adcdata;
    void (*_pwm_set)(uint8_t _duty); //这里需要提供20HZ 占空比为20的脉冲信号
    uint16_t (*_adc_get)(); 
}bsp_z4dc01;


extern bsp_z4dc01  _z4dc01_1;
extern bsp_z4dc01  _z4dc01_2;

int bsp_z4dC01_init(bsp_z4dc01 *_this,void (*pwm_fn)(uint8_t _duty),uint16_t (*_adc_fn)());

uint16_t bsp_z4dC01_read(bsp_z4dc01 *_this);

int bsp_z4dC01_Task_Init(void);

#endif // !__BSP_Z4D_C01_H__
