
/*------------------------------------------------------------------------------
 * @file    BSP_ADC.H
 * @author  SIERRA TEAM 
 * @date    2024/07/02 18:39:56
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
#ifndef __BSP_ADC_H
#define __BSP_ADC_H

/* Files includes  -----------------------------------------------------------*/
#include <stdint.h>
#include "./adc128S102/bsp_adc128S102.h"
#include "./ad7730/bsp_ad7730.h"
/* Defines -------------------------------------------------------------------*/


/* Variables -----------------------------------------------------------------*/



// enum {
//     I_ADC_CH =0,                //超乳电流通道
//     V_ADC_CH,                   //超乳电压通道
//     CASSETTE_L_ADC_CH,          //积液盒左进
//     CASSETTE_R_ADC_CH,          //积液盒右进
//     VIT_PRESSURE1_ADC_CH,       //玻切气压传感器1
//     VIT_PRESSURE2_ADC_CH,       //玻切切气压传感器2
//     VACCUM_PRESSURE_ADC_CH,     //负压传感器
//     AIR_PUMB_ADC_CH,            //气源隔膜泵通道       
//     MAX_ADC_CH
// };

enum {
    VIT_PRESSURE2_ADC_CH=0,       //玻切切气压传感器2
    VIT_PRESSURE1_ADC_CH,       //玻切气压传感器1
    VACCUM_PRESSURE_ADC_CH,     //负压传感器
    I_ADC_CH ,                //超乳电流通道
    V_ADC_CH,                   //超乳电压通道
    AIR_PUMB_ADC_CH,            //气源隔膜泵通道  
    CASSETTE_L_ADC_CH,          //积液盒左进(电阻方案)
    CASSETTE_R_ADC_CH,          //积液盒右进(电阻方案)
    MAX_ADC3_CH,                 //ADC3通道上的
    TEMP_ADC_CH, //
    MAX_ADC1_CH,                 //ADC1通道上的
    CASSETTE_L_ADC_CH_2,          //积液盒左进(光电方案)
    CASSETTE_R_ADC_CH_2,          //积液盒右进(光电方案)
    MAX_ADC2_CH,                 //ADC2通道上的
    I_IV_ADC_CH,                //ADS CH0(IV杆电流检测)
    I_STEP_M_ADC_CH,            //ADS CH1(步进电流检测)
    I_DCT_ADC_CH,               //ADS CH2(电磁铁电流检测)
    V_GDN0_ADC_CH,              //ADS CH3(地0电压)
    V_GDN1_ADC_CH,              //ADS CH4(地1电压)
    I_R_CLAM_MOTOR_ADC_CH,      //ADS CH5(右 clam 电机电流)
    I_L_CLAM_MOTOR_ADC_CH,      //ADS CH6(左 clam 电机电流)
    I_HIGH_SPEED_VALUE_ADC_CH,        //ADS CH7(高速阀电流)
    MAX_ADC128_CH, //ADC128
    EXTERN_VACCUM_PRESSURE_ADC_CH,  //外部负压传感器
    MAX_ADC7730_CH
};



/* Functions ----------------------------------------------------------------*/
int bsp_adc_init(void);
uint16_t bsp_get_valve(uint8_t _ch);


uint16_t bsp_adc_get_valve(uint8_t index, uint8_t _ch); 
#endif
