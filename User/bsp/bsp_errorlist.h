/*------------------------------------------------------------------------------
 * @file    BSP_ERRORLIST.H
 * @author  SIERRA TEAM 
 * @date    2024/04/30 15:52:50
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
#ifndef __BSP_ERRORLIST_H
#define __BSP_ERRORLIST_H

/* Files includes  -----------------------------------------------------------*/


/* Defines -------------------------------------------------------------------*/
#define ERROR_LEVEL_NORMAL  0       //无错误
#define ERROR_LEVEL_1       1        //
#define ERROR_LEVEL_2       2        //
#define ERROR_LEVEL_3       3    
#define ERROR_LEVEL_4       4   

//错误代码
#define ERROR_CODE_NORAML   0X7FFF  
/* Variables -----------------------------------------------------------------*/

/* Functions ----------------------------------------------------------------*/
void bsp_error_list_add(uint32_t code);
uint16_t bsp_error_list_pop();
uint16_t bsp_error_list_getpack();
#endif
