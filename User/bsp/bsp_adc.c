/*------------------------------------------------------------------------------
 * @file    BSP_ADC.C
 * @author  FIRMWARE TEAM 
 * @date    2024/07/02 18:49:49
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
#include <board.h>
#include "bsp_adc.h"

ADC_HandleTypeDef hadc1 = {0};
ADC_HandleTypeDef hadc2 = {0};
ADC_HandleTypeDef hadc3 = {0};
DMA_HandleTypeDef hdma_adc3 = {0};
DMA_HandleTypeDef hdma_adc2 = {0};
DMA_HandleTypeDef hdma_adc1 = {0};

#include <rtdevice.h>
static struct rt_completion completion_adc2;

#define ADC_SAMPLE_LENGTH   6 

ALIGN_32BYTES(__attribute__((section (".RAM_D3"))) uint16_t ADCxValuesBuff1[MAX_ADC3_CH*ADC_SAMPLE_LENGTH*2]);
// ALIGN_32BYTES(__attribute__((section (".RAM_D3"))) uint16_t ADCxValuesBuff2[MAX_ADC3_CH*ADC_SAMPLE_LENGTH]);

ALIGN_32BYTES(__attribute__((section (".RAM_D3"))) uint16_t ADC1ValuesBuff1[(MAX_ADC1_CH-MAX_ADC3_CH-1)*ADC_SAMPLE_LENGTH*2]);
// ALIGN_32BYTES(__attribute__((section (".RAM_D3"))) uint16_t ADC1ValuesBuff2[(MAX_ADC1_CH-MAX_ADC3_CH-1)*ADC_SAMPLE_LENGTH]);


ALIGN_32BYTES(__attribute__((section (".RAM_D3"))) uint16_t ADC2ValuesBuff1[(MAX_ADC2_CH-MAX_ADC1_CH-1)*ADC_SAMPLE_LENGTH*2]);
// ALIGN_32BYTES(__attribute__((section (".RAM_D3"))) uint16_t ADC2ValuesBuff2[(MAX_ADC2_CH-MAX_ADC1_CH-1)*ADC_SAMPLE_LENGTH]);
typedef struct _adc_sample_dma
{
  uint8_t sample_adc_length;      //ADC 采集的长度
  uint8_t current_buff_index;     //当前使用的BUFF
  uint8_t state;                  //内存可用状态
  uint16_t buffer_size;           //内存大小 
  uint16_t *buffer[2];
}adc_sample_dma;

adc_sample_dma adc_sample_data = {
  .sample_adc_length = MAX_ADC3_CH,
  .current_buff_index = 1,
  .state = 0,
  .buffer_size = MAX_ADC3_CH*ADC_SAMPLE_LENGTH*2,
  .buffer = {(uint16_t*)ADCxValuesBuff1, (uint16_t*)&ADCxValuesBuff1[MAX_ADC3_CH*ADC_SAMPLE_LENGTH]}
};


adc_sample_dma adc1_sample_data = {
  .sample_adc_length = (MAX_ADC1_CH-MAX_ADC3_CH-1),
  .current_buff_index = 1,
  .state = 0,
  .buffer_size = (MAX_ADC1_CH-MAX_ADC3_CH-1)*ADC_SAMPLE_LENGTH*2,
  .buffer = {(uint16_t*)ADC1ValuesBuff1, (uint16_t*)&ADC1ValuesBuff1[(MAX_ADC1_CH-MAX_ADC3_CH-1)*ADC_SAMPLE_LENGTH]}
};


adc_sample_dma adc2_sample_data = {
  .sample_adc_length = MAX_ADC2_CH-MAX_ADC1_CH-1,
  .current_buff_index = 1,
  .state = 0,
  .buffer_size = (MAX_ADC2_CH-MAX_ADC1_CH-1)*ADC_SAMPLE_LENGTH*2,
  .buffer = {(uint16_t*)ADC2ValuesBuff1, (uint16_t*)&ADC2ValuesBuff1[(MAX_ADC2_CH-MAX_ADC1_CH-1)*ADC_SAMPLE_LENGTH]}
};

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
 static void MX_ADC3_Init(void)
 {
 
   /* USER CODE BEGIN ADC3_Init 0 */
 
   /* USER CODE END ADC3_Init 0 */
 
   ADC_ChannelConfTypeDef sConfig = {0};
 
   /* USER CODE BEGIN ADC3_Init 1 */
 
   /* USER CODE END ADC3_Init 1 */
 
   /** Common config
   */
   hadc3.Instance = ADC3;
   hadc3.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;
   hadc3.Init.ScanConvMode = ADC_SCAN_ENABLE;
   hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
   hadc3.Init.LowPowerAutoWait = DISABLE;
   hadc3.Init.ContinuousConvMode = ENABLE;
   hadc3.Init.NbrOfConversion = 8;
   hadc3.Init.DiscontinuousConvMode = DISABLE;
   hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
   hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
   hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
   hadc3.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
   hadc3.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
   hadc3.Init.OversamplingMode = DISABLE;
   hadc3.Init.Oversampling.Ratio = 1;
   if (HAL_ADC_Init(&hadc3) != HAL_OK)
   {
     Error_Handler();
   }
   hadc3.Init.Resolution = ADC_RESOLUTION_16B;
   if (HAL_ADC_Init(&hadc3) != HAL_OK)
   {
     Error_Handler();
   }
 
   /** Configure Regular Channel
   */
   sConfig.Channel = ADC_CHANNEL_2;
   sConfig.Rank = ADC_REGULAR_RANK_1;
   sConfig.SamplingTime = ADC_SAMPLETIME_8CYCLES_5;
   sConfig.SingleDiff = ADC_SINGLE_ENDED;
   sConfig.OffsetNumber = ADC_OFFSET_NONE;
   sConfig.Offset = 0;
   sConfig.OffsetSignedSaturation = DISABLE;
   if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
   {
     Error_Handler();
   }
 
   /** Configure Regular Channel
   */
   sConfig.Channel = ADC_CHANNEL_6;
   sConfig.Rank = ADC_REGULAR_RANK_2;
   if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
   {
     Error_Handler();
   }
 
   /** Configure Regular Channel
   */
   sConfig.Channel = ADC_CHANNEL_5;
   sConfig.Rank = ADC_REGULAR_RANK_3;
   if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
   {
     Error_Handler();
   }
 
   /** Configure Regular Channel
   */
   sConfig.Channel = ADC_CHANNEL_4;
   sConfig.Rank = ADC_REGULAR_RANK_4;
   if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
   {
     Error_Handler();
   }
 
   /** Configure Regular Channel
   */
   sConfig.Channel = ADC_CHANNEL_9;
   sConfig.Rank = ADC_REGULAR_RANK_5;
   if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
   {
     Error_Handler();
   }
 
   /** Configure Regular Channel
   */
   sConfig.Channel = ADC_CHANNEL_13;
   sConfig.Rank = ADC_REGULAR_RANK_6;
   if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
   {
     Error_Handler();
   }
 
   /** Configure Regular Channel
   */
   sConfig.Channel = ADC_CHANNEL_15;
   sConfig.Rank = ADC_REGULAR_RANK_7;
   if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
   {
     Error_Handler();
   }
 
   /** Configure Regular Channel
   */
   sConfig.Channel = ADC_CHANNEL_16;
   sConfig.Rank = ADC_REGULAR_RANK_8;
   if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
   {
     Error_Handler();
   }
   /* USER CODE BEGIN ADC3_Init 2 */
 
   /* USER CODE END ADC3_Init 2 */
 
 }

/*
*********************************************************************************************************
*	函 数 名: DMA1_Stream1_IRQHandler
*	功能说明: DMA1 Stream1中断服务程序
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void DMA1_Stream3_IRQHandler(void)
{
	/* 传输完成中断 */
	if((DMA1->LISR & DMA_FLAG_TCIF3_7) != RESET)
	{
		
		
		/*
		   1、使用此函数要特别注意，第1个参数地址要32字节对齐，第2个参数要是32字节的整数倍。
		   2、进入传输完成中断，当前DMA正在使用缓冲区的前半部分，用户可以操作后半部分。
		*/
		SCB_InvalidateDCache_by_Addr((uint32_t *)(adc1_sample_data.buffer[1]), adc1_sample_data.buffer_size*2);
    adc1_sample_data.current_buff_index = 1;  //切换当前使用BUFF
    if(adc1_sample_data.state == 0)
      adc1_sample_data.state = 1;

		/* 清除标志 */
		DMA1->LIFCR = DMA_FLAG_TCIF3_7;
	}

	/* 半传输完成中断 */    
	if((DMA1->LISR & DMA_FLAG_HTIF3_7) != RESET)
	{

    SCB_InvalidateDCache_by_Addr((uint32_t *)(adc1_sample_data.buffer[0]), adc1_sample_data.buffer_size*2);
    adc1_sample_data.current_buff_index = 0; //上半部分完成
		/* 清除标志 */
		DMA1->LIFCR = DMA_FLAG_HTIF3_7;
	}

	/* 传输错误中断 */
	if((DMA1->LISR & DMA_FLAG_TEIF3_7) != RESET)
	{
		/* 清除标志 */
		DMA1->LIFCR = DMA_FLAG_TEIF3_7;
	}

	/* 直接模式错误中断 */
	if((DMA1->LISR & DMA_FLAG_DMEIF3_7) != RESET)
	{
		/* 清除标志 */
		DMA1->LIFCR = DMA_FLAG_DMEIF3_7;
	}
}
/*
*********************************************************************************************************
*	函 数 名: DMA1_Stream1_IRQHandler
*	功能说明: DMA1 Stream1中断服务程序
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void DMA1_Stream1_IRQHandler(void)
{
	/* 传输完成中断 */
	if((DMA1->LISR & DMA_FLAG_TCIF1_5) != RESET)
	{
		
		
		/*
		   1、使用此函数要特别注意，第1个参数地址要32字节对齐，第2个参数要是32字节的整数倍。
		   2、进入传输完成中断，当前DMA正在使用缓冲区的前半部分，用户可以操作后半部分。
		*/
    SCB_InvalidateDCache_by_Addr((uint32_t *)(adc_sample_data.buffer[1]), adc_sample_data.buffer_size*2);
    adc_sample_data.current_buff_index = 1;  //切换当前使用BUFF
    if(adc_sample_data.state == 0)
        adc_sample_data.state = 1;

		/* 清除标志 */
		DMA1->LIFCR = DMA_FLAG_TCIF1_5;
	}

	/* 半传输完成中断 */    
	if((DMA1->LISR & DMA_FLAG_HTIF1_5) != RESET)
	{

    SCB_InvalidateDCache_by_Addr((uint32_t *)(adc_sample_data.buffer[0]), adc_sample_data.buffer_size*2);
    adc_sample_data.current_buff_index = 0;  //切换当前使用BUFF
		/* 清除标志 */
		DMA1->LIFCR = DMA_FLAG_HTIF1_5;
	}

	/* 传输错误中断 */
	if((DMA1->LISR & DMA_FLAG_TEIF1_5) != RESET)
	{
		/* 清除标志 */
		DMA1->LIFCR = DMA_FLAG_TEIF1_5;
	}

	/* 直接模式错误中断 */
	if((DMA1->LISR & DMA_FLAG_DMEIF1_5) != RESET)
	{
		/* 清除标志 */
		DMA1->LIFCR = DMA_FLAG_DMEIF1_5;
	}
}


void DMA1_Stream4_IRQHandler(void)
{
	/* 传输完成中断 */
	if((DMA1->HISR & DMA_FLAG_TCIF0_4) != RESET)
	{
		
		
		/*
		   1、使用此函数要特别注意，第1个参数地址要32字节对齐，第2个参数要是32字节的整数倍。
		   2、进入传输完成中断，当前DMA正在使用缓冲区的前半部分，用户可以操作后半部分。
		*/
 
  SCB_InvalidateDCache_by_Addr((uint32_t *)(adc2_sample_data.buffer[1]), adc2_sample_data.buffer_size*2);
  adc2_sample_data.current_buff_index = 1;  //切换当前使用BUFF
  if(adc2_sample_data.state == 0)
    adc2_sample_data.state = 1;
  rt_completion_done(&completion_adc2); //寄存器2完成采集

		/* 清除标志 */
		/* 清除标志 */
		DMA1->HIFCR = DMA_FLAG_TCIF0_4;
	}

	/* 半传输完成中断 */    
	if((DMA1->HISR & DMA_FLAG_HTIF0_4) != RESET)
	{

    SCB_InvalidateDCache_by_Addr((uint32_t *)(adc2_sample_data.buffer[0]), adc2_sample_data.buffer_size*2);
    adc2_sample_data.current_buff_index = 0;  //切换当前使用BUFF
		rt_completion_done(&completion_adc2); //寄存器2完成采集
		/* 清除标志 */
		DMA1->HIFCR = DMA_FLAG_HTIF0_4;
	}

	/* 传输错误中断 */
	if((DMA1->HISR & DMA_FLAG_TEIF0_4) != RESET)
	{
		/* 清除标志 */
		DMA1->HIFCR = DMA_FLAG_TEIF0_4;
	}

	/* 直接模式错误中断 */
	if((DMA1->HISR & DMA_FLAG_DMEIF0_4) != RESET)
	{
		/* 清除标志 */
		DMA1->HIFCR = DMA_FLAG_DMEIF0_4;
	}
}


static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc1.Init.OversamplingMode = ENABLE;
  hadc1.Init.Oversampling.Ratio = 16;
  hadc1.Init.Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_4;
  hadc1.Init.Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
  hadc1.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  hadc1.Init.Resolution = ADC_RESOLUTION_16B;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_32CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
 static void MX_ADC2_Init(void)
 {
 
   /* USER CODE BEGIN ADC2_Init 0 */
 
   /* USER CODE END ADC2_Init 0 */
 
   ADC_ChannelConfTypeDef sConfig = {0};
 
   /* USER CODE BEGIN ADC2_Init 1 */
 
   /* USER CODE END ADC2_Init 1 */
 
   /** Common config
   */
   hadc2.Instance = ADC2;
   hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV16;
   hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE;
   hadc2.Init.EOCSelection = ADC_EOC_SEQ_CONV;
   hadc2.Init.LowPowerAutoWait = DISABLE;
   hadc2.Init.ContinuousConvMode = ENABLE;
   hadc2.Init.NbrOfConversion = 2;
   hadc2.Init.DiscontinuousConvMode = DISABLE;
   hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
   hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
   hadc2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
   hadc2.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
   hadc2.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;

   hadc2.Init.OversamplingMode = DISABLE;
   hadc2.Init.Oversampling.Ratio = 1;
  //  hadc2.Init.OversamplingMode = ENABLE;
  //  hadc2.Init.Oversampling.Ratio = 16;
  //  hadc2.Init.Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_4;
  //  hadc2.Init.Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
  //  hadc2.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;
   if (HAL_ADC_Init(&hadc2) != HAL_OK)
   {
     Error_Handler();
   }
   hadc2.Init.Resolution = ADC_RESOLUTION_16B;
   if (HAL_ADC_Init(&hadc2) != HAL_OK)
   {
     Error_Handler();
   }
 
   /** Configure Regular Channel
   */
   sConfig.Channel = ADC_CHANNEL_15;
   sConfig.Rank = ADC_REGULAR_RANK_1;
   sConfig.SamplingTime = ADC_SAMPLETIME_64CYCLES_5;
   sConfig.SingleDiff = ADC_SINGLE_ENDED;
   sConfig.OffsetNumber = ADC_OFFSET_NONE;
   sConfig.Offset = 0;
   sConfig.OffsetSignedSaturation = DISABLE;
   if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
   {
     Error_Handler();
   }
 
   /** Configure Regular Channel
   */
   sConfig.Channel = ADC_CHANNEL_18;
   sConfig.Rank = ADC_REGULAR_RANK_2;
   if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
   {
     Error_Handler();
   }
   /* USER CODE BEGIN ADC2_Init 2 */
 
   /* USER CODE END ADC2_Init 2 */
 
 }




int bsp_adc_init(void)
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
	
	
  // rt_completion_init(&completion_adc1);
  rt_completion_init(&completion_adc2);

    /* DMA2_Stream0_IRQn interrupt configuration */
    // 初始化ADC1
   MX_ADC1_Init();
   MX_ADC2_Init();

    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
    {
        Error_Handler();
    }
    // 开始DMA采集
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC1ValuesBuff1, sizeof(ADC1ValuesBuff1)/2) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
    {
        Error_Handler();
    }
    // 开始DMA采集
    if (HAL_ADC_Start_DMA(&hadc2, (uint32_t *)ADC2ValuesBuff1, sizeof(ADC2ValuesBuff1)/2) != HAL_OK)
    {
      Error_Handler();
    }
    MX_ADC3_Init();
    if (HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
    {
        Error_Handler();
    }
    // 开始DMA采集
    if (HAL_ADC_Start_DMA(&hadc3, (uint32_t *)ADCxValuesBuff1, sizeof(ADCxValuesBuff1)/2) != HAL_OK)
    {
      Error_Handler();
    }
    //外部ADC初始化
    bsp_adc128S102_init(); //ADS128S102初始化
		bsp_ad7730_init(); //AD7730初始化
    return 0;
}

//INIT_ENV_EXPORT(bsp_adc_init);



uint16_t bsp_get_valve(uint8_t _ch) 
{
    uint32_t _sum=0;
    uint16_t _adcdata = 0;
    uint16_t *_buffer = NULL;
    if (_ch<MAX_ADC2_CH)  //内部ADC采集
    {
      if (adc_sample_data.state == 1&&_ch<MAX_ADC3_CH)//ADC3采集
      {
        
        _buffer = (uint16_t *)adc_sample_data.buffer[adc_sample_data.current_buff_index];
        
        for(uint8_t i=0;i<ADC_SAMPLE_LENGTH*adc_sample_data.sample_adc_length;i+=adc_sample_data.sample_adc_length)
        {
           _adcdata = _buffer[i+_ch];
            _sum += _adcdata;
        }
      }
      if(adc1_sample_data.state == 1&&(_ch<MAX_ADC1_CH&&_ch>MAX_ADC3_CH))//ADC1采集
      {
        // _buffer = (uint16_t *)adc1_sample_data.buffer[adc1_sample_data.current_buff_index];
        // for(uint8_t i=(_ch-MAX_ADC3_CH-1);i<adc1_sample_data.buffer_size;i+=adc1_sample_data.sample_adc_length)
        // {
        //    _adcdata = _buffer[i];
        //     _sum += _adcdata;
        // }
          _buffer = (uint16_t *)adc1_sample_data.buffer[adc1_sample_data.current_buff_index];
        for(uint8_t i=0;i<ADC_SAMPLE_LENGTH*adc1_sample_data.sample_adc_length;i+=adc1_sample_data.sample_adc_length)
        {
           _adcdata = _buffer[i+(_ch-MAX_ADC3_CH-1)];
            _sum += _adcdata;
        }
      }
      if (adc2_sample_data.state == 1&&(_ch<MAX_ADC2_CH&&_ch>MAX_ADC1_CH))
      {
        _buffer = (uint16_t *)adc2_sample_data.buffer[adc2_sample_data.current_buff_index];
        // for(uint8_t i=(_ch-MAX_ADC1_CH-1);i<adc2_sample_data.buffer_size;i+=adc2_sample_data.sample_adc_length)
        // {
        //    _adcdata = _buffer[i];
        //     _sum += _adcdata;
        // }
        for(uint8_t i=0;i<ADC_SAMPLE_LENGTH*adc2_sample_data.sample_adc_length;i+=adc2_sample_data.sample_adc_length)
        {
           _adcdata = _buffer[i+(_ch-MAX_ADC1_CH-1)];
            _sum += _adcdata;
        }
      }
      return (_sum/ADC_SAMPLE_LENGTH);
    }

    else if (_ch>MAX_ADC2_CH&&_ch<MAX_ADC128_CH)  //ADS128S102采集
    {
      _adcdata = bsp_adc128S102_ReadCh(adc128s_device,_ch-MAX_ADC2_CH-1);
    }
    else        //AD7730采集
    {
      _adcdata = bsp_ad7730_readraw(ad7730);  //这里不用这个 直接在负压那边去用
    }
    return _adcdata;
}





uint16_t bsp_adc_get_valve(uint8_t index, uint8_t _ch)
{
  uint16_t _adcdata = 0;
  if (index == 2)//ADC2
  {
      uint8_t _curConvCh = adc2_sample_data.current_buff_index; //当前转换通道
      while (adc2_sample_data.current_buff_index !=_curConvCh)
      {
        rt_completion_wait(&completion_adc2, RT_WAITING_FOREVER);
      }
      _adcdata = bsp_get_valve(_ch==0?CASSETTE_L_ADC_CH_2:CASSETTE_R_ADC_CH_2);
  }
  return _adcdata;
}