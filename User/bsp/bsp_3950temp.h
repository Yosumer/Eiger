/*
 * @Author: sum 476328608@qq.com
 * @Date: 2024-07-03 10:04:22
 * @LastEditors: sum 476328608@qq.com
 * @LastEditTime: 2024-07-03 11:08:18
 * @FilePath: \patagonia\light\User\bsp\opt\bsp_3950temp.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*------------------------------------------------------------------------------
 * @file    BSP_3950TEMP.H
 * @author  SIERRA TEAM 
 * @date    2024/07/03 10:04:59
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
#ifndef __BSP_3950TEMP_H
#define __BSP_3950TEMP_H

/* Files includes  -----------------------------------------------------------*/
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/


/* Variables -----------------------------------------------------------------*/
typedef struct _bsp_3950temp_t
{
    float _temp;         //温度
    float (*get_res)(); //获取电阻
}bsp_3950temp_t;


/* Functions ----------------------------------------------------------------*/
void  bsp_3950temp_Init(bsp_3950temp_t *_this,float (*_get_res)());
float bsp_3950temp_Read(bsp_3950temp_t *_this);


#endif
