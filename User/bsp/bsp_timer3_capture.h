/*------------------------------------------------------------------------------
 * @file    BSP_TIMER4_CAPTURE.H
 * @author  SIERRA TEAM 
 * @date    2024/07/03 15:23:36
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
#ifndef __BSP_TIMER4_CAPTURE_H
#define __BSP_TIMER4_CAPTURE_H

/* Files includes  -----------------------------------------------------------*/
#include <stdint.h>
#include <board.h>

/* Defines -------------------------------------------------------------------*/
#define MAX_FAN_NUM         3
#define MAX_TIM3_FAN_NUM    2 
#define FAN3_CH            2


#define FAN1_CH             1
#define FAN2_CH             0
/* Variables -----------------------------------------------------------------*/

typedef struct _caputer_t
{
    uint8_t flag;           //捕获标志位
    uint8_t over_cnt;       //溢出标志位
    int32_t first_cnt;      //第一次计数值
    int32_t second_cnt;     //第二次计数值
    uint32_t caputer_cnt;   //采集值
}caputer_t;

extern caputer_t fan_caputer[MAX_FAN_NUM];
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim5;
/* Functions ----------------------------------------------------------------*/

int bsp_timer3_capture_Init();
uint32_t bsp_timer3_get_fan1_Cnt();
uint32_t bsp_timer3_get_fan2_Cnt();
uint32_t bsp_timer5_get_fan3_Cnt();
#endif
