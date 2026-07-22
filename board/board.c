/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include "board.h"

void PeriphCommonClock_Config(void);

/**
  * @brief System Clock Configuration
  * @retval None
  */
 void SystemClock_Config(void)
 {
   RCC_OscInitTypeDef RCC_OscInitStruct = {0};
   RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
 
   /** Supply configuration update enable
   */
   HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
 
   /** Configure the main internal regulator output voltage
   */
   __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
 
   while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
 
   __HAL_RCC_SYSCFG_CLK_ENABLE();
   __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
 
   while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
 
   /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
   RCC_OscInitStruct.HSEState = RCC_HSE_ON;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
   RCC_OscInitStruct.PLL.PLLM = 5;
   RCC_OscInitStruct.PLL.PLLN = 192;
   RCC_OscInitStruct.PLL.PLLP = 2;
   RCC_OscInitStruct.PLL.PLLQ = 12;
   RCC_OscInitStruct.PLL.PLLR = 2;
   RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
   RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
   RCC_OscInitStruct.PLL.PLLFRACN = 0;
   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
   {
     Error_Handler();
   }
 
   /** Initializes the CPU, AHB and APB buses clocks
   */
   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                               |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                               |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
   RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
   RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
   RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
   RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
 
   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
   {
     Error_Handler();
   }
   PeriphCommonClock_Config(); //外设初始化
 }
 
//  /**
//    * @brief Peripherals Common Clock Configuration
//    * @retval None
//    */
//  void PeriphCommonClock_Config(void)
//  {
//    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
 
//    /** Initializes the peripherals clock
//    */
//    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
//    PeriphClkInitStruct.PLL3.PLL3M = 5;
//    PeriphClkInitStruct.PLL3.PLL3N = 160;
//    PeriphClkInitStruct.PLL3.PLL3P = 2;
//    PeriphClkInitStruct.PLL3.PLL3Q = 2;
//    PeriphClkInitStruct.PLL3.PLL3R = 10;
//    PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_2;
//    PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
//    PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
//    PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL3;
//    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
//    {
//      Error_Handler();
//    }
//  }



 
 /**
   * @brief Peripherals Common Clock Configuration
   * @retval None
   */
 void PeriphCommonClock_Config(void)
 {
   RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
 
   /** Initializes the peripherals clock
   */
   PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_UART7
                               |RCC_PERIPHCLK_UART4;
   PeriphClkInitStruct.PLL2.PLL2M = 2;
   PeriphClkInitStruct.PLL2.PLL2N = 12;
   PeriphClkInitStruct.PLL2.PLL2P = 2;
   PeriphClkInitStruct.PLL2.PLL2Q = 2;
   PeriphClkInitStruct.PLL2.PLL2R = 2;
   PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
   PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
   PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
   PeriphClkInitStruct.PLL3.PLL3M = 5;
   PeriphClkInitStruct.PLL3.PLL3N = 160;
   PeriphClkInitStruct.PLL3.PLL3P = 2;
   PeriphClkInitStruct.PLL3.PLL3Q = 2;
   PeriphClkInitStruct.PLL3.PLL3R = 10;
   PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_2;
   PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
   PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
   PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_PLL2;
   PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL3;
   if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
   {
     Error_Handler();
   }
 }