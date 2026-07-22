/*------------------------------------------------------------------------------
 * @file    BSP_TIMER_CALLBACK.C
 * @author  FIRMWARE TEAM 
 * @date    2024/07/03 15:50:50
 * @brief   THIS FILE PROVIDES ALL THE FIRMWARE FUNCTIONS.
 * -----------------------------------------------------------------------------
 * @attention 
 
 * THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE 
 * CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE 
 * TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR 
 * CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH 
 * HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN 
 * CONNECTION WITH PRODUCTS MADE BY CUSTOMERS. 
 * 
 * <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2> 
------------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion  ------------------------------------*/


/* Files includes  -----------------------------------------------------------*/
#include "bsp_timer3_capture.h"
#include "bsp_abstractEncode.h"

#include "bsp_step_l6470.h"
extern TIM_HandleTypeDef htim8;
// extern bsp_step_motor_t     l6470_motor;

/*----------------------------------------------------------------------------*/


/**
  * @brief This function handles TIM8 update interrupt and TIM13 global interrupt.
  */
void TIM8_UP_TIM13_IRQHandler(void)
{
  /* USER CODE BEGIN TIM8_UP_TIM13_IRQn 0 */

  /* USER CODE END TIM8_UP_TIM13_IRQn 0 */
  HAL_TIM_IRQHandler(&htim8);
  /* USER CODE BEGIN TIM8_UP_TIM13_IRQn 1 */

  /* USER CODE END TIM8_UP_TIM13_IRQn 1 */
}

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM5_IRQHandler(void)
{
 /* USER CODE BEGIN TIM4_IRQn 0 */

 /* USER CODE END TIM4_IRQn 0 */
 HAL_TIM_IRQHandler(&htim5);
 /* USER CODE BEGIN TIM4_IRQn 1 */

 /* USER CODE END TIM4_IRQn 1 */
}

void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */

  /* USER CODE END TIM3_IRQn 1 */
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM3)
    {
        for (int i = 0; i < MAX_TIM3_FAN_NUM; i++)
        {
            fan_caputer[i].over_cnt++;
        }
    }
    else if (htim->Instance == TIM8)
    {
        // bsp_step_l6470_conf *_l6470cof =  (bsp_step_l6470_conf *)l6470_motor.cof;
        // bsp_abstractEncode_IQRHandler((abstractEncoder_t *)_l6470cof->_encoder);
    }
    else if (htim->Instance == TIM5)
    {
        for (int i = FAN3_CH; i < MAX_FAN_NUM; i++)
        {
            fan_caputer[i].over_cnt++;
        }
    }
    else
    {
     
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2 || htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)   //FAN2
        {
            uint8_t _fanch = (htim->Channel/4); //转换成风扇对应通道
            if(fan_caputer[_fanch].flag == 0)//第一次
            {
                fan_caputer[_fanch].first_cnt = HAL_TIM_ReadCapturedValue(&htim3,_fanch==0?TIM_CHANNEL_2:TIM_CHANNEL_3);
                fan_caputer[_fanch].over_cnt = 0;
                fan_caputer[_fanch].flag = 1;
            }
            else    //第二次
            {
                fan_caputer[_fanch].second_cnt = HAL_TIM_ReadCapturedValue(&htim3,_fanch==0?TIM_CHANNEL_2:TIM_CHANNEL_3);
                fan_caputer[_fanch].caputer_cnt = (fan_caputer[_fanch].second_cnt - fan_caputer[_fanch].first_cnt)+fan_caputer[_fanch].over_cnt*1000;
                fan_caputer[_fanch].over_cnt = 0;
                fan_caputer[_fanch].flag = 0;
            }
        }
        else
        {

        }
        
    }
    else if (htim->Instance == TIM5)
    {
        if( htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)   //FAN3
        {
            if (fan_caputer[FAN3_CH].flag == 0)
            {
                fan_caputer[FAN3_CH].first_cnt = HAL_TIM_ReadCapturedValue(&htim5,TIM_CHANNEL_2);
                fan_caputer[FAN3_CH].over_cnt = 0;
                fan_caputer[FAN3_CH].flag = 1;
            }
            else
            {
                fan_caputer[FAN3_CH].second_cnt = HAL_TIM_ReadCapturedValue(&htim5,TIM_CHANNEL_2); 
                fan_caputer[FAN3_CH].caputer_cnt = (fan_caputer[FAN3_CH].second_cnt - fan_caputer[FAN3_CH].first_cnt)+fan_caputer[FAN3_CH].over_cnt*1000;
                fan_caputer[FAN3_CH].over_cnt = 0;
                fan_caputer[FAN3_CH].flag = 0;
            }
            
        }
    }
    else
    {
     
    }
}