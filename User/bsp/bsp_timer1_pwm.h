/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-09-09 09:56:15
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-09-09 13:20:25
 * @FilePath: \01LightModule_V2\light\User\bsp\opt\bsp_timer8_pwm.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef BSP_TIMER1_PWM_H 
#define BSP_TIMER1_PWM_H 

#include <stdint.h>
#include <board.h>


extern void updata_time1_freq(uint16_t freq);
extern int bsp_timer1_pwm_Init();      //v2.0 使用的pwm生成
#endif // BSP_TIMER1_PWM_H