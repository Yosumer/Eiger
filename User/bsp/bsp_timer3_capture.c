
/*------------------------------------------------------------------------------
 * @file    BSP_TIMER4_CAPTURE.C
 * @author  FIRMWARE TEAM 
 * @date    2024/07/03 15:33:56
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
#include "bsp_timer3_capture.h"

/* Files includes  -----------------------------------------------------------*/







caputer_t fan_caputer[MAX_FAN_NUM]={0};


TIM_HandleTypeDef htim3={0};

TIM_HandleTypeDef htim5={0};

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 120-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000 -1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}
/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
 static void MX_TIM5_Init(void)
 {
 
   /* USER CODE BEGIN TIM5_Init 0 */
 
   /* USER CODE END TIM5_Init 0 */
 
   TIM_ClockConfigTypeDef sClockSourceConfig = {0};
   TIM_MasterConfigTypeDef sMasterConfig = {0};
   TIM_OC_InitTypeDef sConfigOC = {0};
   TIM_IC_InitTypeDef sConfigIC = {0};
 
   /* USER CODE BEGIN TIM5_Init 1 */
 
   /* USER CODE END TIM5_Init 1 */
   htim5.Instance = TIM5;
   htim5.Init.Prescaler = 120-1;
   htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
   htim5.Init.Period = 1000 -1;
   htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
   htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
   if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
   {
     Error_Handler();
   }
   sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
   if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
   {
     Error_Handler();
   }
   if (HAL_TIM_PWM_Init(&htim5) != HAL_OK)
   {
     Error_Handler();
   }
   if (HAL_TIM_IC_Init(&htim5) != HAL_OK)
   {
     Error_Handler();
   }
   sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
   sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
   if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
   {
     Error_Handler();
   }
   sConfigOC.OCMode = TIM_OCMODE_PWM1;
   sConfigOC.Pulse = 0;
   sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
   sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
   if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
   {
     Error_Handler();
   }
   sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
   sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
   sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
   sConfigIC.ICFilter = 0;
   if (HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
   {
     Error_Handler();
   }
   /* USER CODE BEGIN TIM5_Init 2 */
 
   /* USER CODE END TIM5_Init 2 */
   HAL_TIM_MspPostInit(&htim5);
 
 }

static void _bsp_timer3_INT_Init()
{
    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_2); //fan 1
    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_3); //fan 2
    __HAL_TIM_ENABLE_IT(&htim3,TIM_IT_UPDATE);
}
static void _bsp_timer5_INT_Init()
{
    HAL_TIM_Base_Start_IT(&htim5);
    HAL_TIM_IC_Start_IT(&htim5,TIM_CHANNEL_2); //fan 3

    __HAL_TIM_ENABLE_IT(&htim5,TIM_IT_UPDATE);

}

int bsp_timer3_capture_Init()
{
  MX_TIM3_Init();
  _bsp_timer3_INT_Init();
  MX_TIM5_Init();
	_bsp_timer5_INT_Init();

}
INIT_ENV_EXPORT(bsp_timer3_capture_Init);
uint32_t bsp_timer3_get_fan1_Cnt()
{
    return fan_caputer[FAN1_CH].caputer_cnt;
}

uint32_t bsp_timer3_get_fan2_Cnt()
{
    return fan_caputer[FAN2_CH].caputer_cnt;
}

uint32_t bsp_timer5_get_fan3_Cnt()
{
  return fan_caputer[FAN3_CH].caputer_cnt;
}