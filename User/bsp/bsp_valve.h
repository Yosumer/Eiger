/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-04 13:21:02
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-08-26 16:19:47
 * @FilePath: \EigeV1.0.01\User\bsp\bsp_valve.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef BSP_VALVE_H
#define BSP_VALVE_H

#include <stdint.h>

/* 位操作宏定义 */
#define BIT_SET(reg, bit)     ((reg) |= (1UL << (bit)))  // 设置指定位置为1
#define BIT_CLEAR(reg, bit)   ((reg) &= ~(1UL << (bit))) // 清除指定位置为0
#define BIT_READ(reg, bit)    (((reg) >> (bit)) & 1UL)   // 读取指定位置的值
#define BIT_TOGGLE(reg, bit)  ((reg) ^= (1UL << (bit)))  // 反转指定位置的值

#define MOTOR_LEFT_UP 0
#define  MOTOR_LEFT_DOWN 1

#define MOTOR_RIGHT_UP 2
#define MOTOR_RIGHT_DOWN 3


#define IV_PWR_OFF  0
#define IV_PWR_ON   1
#define IV_RSR_OFF  2
#define IV_RSR_ON   3
#define PEN_PWR_OFF 4
#define PEN_PWR_ON  5  
#define PEN_RSR_OFF 6
#define PEN_RSR_ON  7

enum VALVE_CHANNEL {
    // VALVE_IV_PWR = 0, 
    VALVE_BIT_CTL = 0,      //按位控制的阀门    0bit: IV杆阀门电源 1:IV杆复位阀门 2bit:踏板电源控制 3bit:踏板复位
    VALVE_INFUSION,     // 灌注阀门
    VALVE_REFLUX,        //回流阀门
    // VALVE_LEFT_BOX,     //积液盒左进
    // VALVE_RIGHT_BOX,    // 积液盒右进  

    VALVE_BOX_IN,   //积液盒进
    VALVE_BOX_OUT,  //积液盒出

    
    VALVE_AIR_PUMB,     //隔膜泵控制
    VALVE_MAX
};



int bsp_valve_Init();
void bsp_valve_Ctl(uint8_t ch, uint8_t _ctl);
void bsp_valve_ctlAll(uint8_t _ctl);
void bsp_pwmValve_Ctl(uint8_t ch,uint16_t duty);

uint8_t bsp_valve_read(uint8_t ch);

#endif // !BSP_VALVE_H