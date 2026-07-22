/*------------------------------------------------------------------------------
 * @file    BSP_ALGORITHM.H
 * @author  SIERRA TEAM 
 * @date    2023/11/22 10:57:15
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
#ifndef __BSP_ALGORITHM_H
#define __BSP_ALGORITHM_H

/* Files includes  -----------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#define AVG_POION_NUM           5
/* Defines -------------------------------------------------------------------*/
typedef struct _algorithm_avg {
    float data[AVG_POION_NUM];
    int read;
    int write;
}algorithm_avg;
/* Variables -----------------------------------------------------------------*/


/* Functions ----------------------------------------------------------------*/


uint16_t bsp_mid_filt16(uint16_t *data,uint16_t size);

float bsp_mid_filt_f(float *data,uint16_t size);
void  bsp_avg_Init(algorithm_avg *_this,float d);
void  bsp_avg_push(algorithm_avg *_this,float d);
void  bsp_avg_pop(algorithm_avg *_this);
float bsp_avg_getAvg(algorithm_avg *_this);
float bsp_avg_getData(algorithm_avg *_this,float d);

uint8_t bsp_algo_isInRange(float value, float min, float max);

#endif
