#include "bsp_abstractEncode.h"
#include "bsp_encode_Timer8.h"

 TIM_HandleTypeDef htim8 = {0};

void bsp_Encoder_Timer8_Init()
{
 
  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 60000-1;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0xf;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0xf;
  if (HAL_TIM_Encoder_Init(&htim8, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* 清零计数器 */
  bsp_Encoder_Timer8_SetCount(0);
   /* 清零中断标志位 */
   __HAL_TIM_CLEAR_IT(&htim8,TIM_IT_UPDATE);
   /* 使能定时器的更新事件中断 */
   __HAL_TIM_ENABLE_IT(&htim8,TIM_IT_UPDATE);

   /* 使能编码器接口 */
    HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_1|TIM_CHANNEL_2);

    //  开启定时器8中断
    HAL_TIM_Base_Start_IT(&htim8);

}
uint32_t bsp_Encoder_Timer8_GetCount()
{
    return (uint32_t) __HAL_TIM_GET_COUNTER(&htim8);
}
uint32_t bsp_Encoder_Timer8_GetDir()
{
    return __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim8);
}
void bsp_Encoder_Timer8_SetCount(uint16_t _count)
{
    __HAL_TIM_SET_COUNTER(&htim8, _count);
}


