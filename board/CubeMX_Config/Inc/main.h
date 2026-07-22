/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define VIT_VALVE_ENA_Pin GPIO_PIN_3
#define VIT_VALVE_ENA_GPIO_Port GPIOE
#define VIT_VALVE_ENB_Pin GPIO_PIN_4
#define VIT_VALVE_ENB_GPIO_Port GPIOE
#define VIT_VALVE_PWM1_Pin GPIO_PIN_5
#define VIT_VALVE_PWM1_GPIO_Port GPIOE
#define VIT_VALVE_PWM2_Pin GPIO_PIN_6
#define VIT_VALVE_PWM2_GPIO_Port GPIOE
#define DAC_SDA_Pin GPIO_PIN_0
#define DAC_SDA_GPIO_Port GPIOF
#define DAC_SCL_Pin GPIO_PIN_1
#define DAC_SCL_GPIO_Port GPIOF
#define DIA_RF_V_ENABLE_Pin GPIO_PIN_2
#define DIA_RF_V_ENABLE_GPIO_Port GPIOF
#define VACCUM_PRESSURE_Pin GPIO_PIN_3
#define VACCUM_PRESSURE_GPIO_Port GPIOF
#define RF_V_ADC_Pin GPIO_PIN_4
#define RF_V_ADC_GPIO_Port GPIOF
#define RF_I_ADC_Pin GPIO_PIN_5
#define RF_I_ADC_GPIO_Port GPIOF
#define REF_ADC_Pin GPIO_PIN_8
#define REF_ADC_GPIO_Port GPIOF
#define VIT_PRESSURE2_Pin GPIO_PIN_9
#define VIT_PRESSURE2_GPIO_Port GPIOF
#define VIT_PRESSURE1_Pin GPIO_PIN_10
#define VIT_PRESSURE1_GPIO_Port GPIOF
#define MCU_ETH_RESET_Pin GPIO_PIN_2
#define MCU_ETH_RESET_GPIO_Port GPIOC
#define ADC_PUMP_Pin GPIO_PIN_2
#define ADC_PUMP_GPIO_Port GPIOH
#define L_RHEOSTAT_ADC_Pin GPIO_PIN_4
#define L_RHEOSTAT_ADC_GPIO_Port GPIOH
#define R_RHEOSTAT_ADC_Pin GPIO_PIN_5
#define R_RHEOSTAT_ADC_GPIO_Port GPIOH
#define L_LIGHT_ADC_Pin GPIO_PIN_3
#define L_LIGHT_ADC_GPIO_Port GPIOA
#define R_LIGHT_ADC_Pin GPIO_PIN_4
#define R_LIGHT_ADC_GPIO_Port GPIOA
#define RBG2_PWM_Pin GPIO_PIN_5
#define RBG2_PWM_GPIO_Port GPIOA
#define Temp_ADC_Pin GPIO_PIN_6
#define Temp_ADC_GPIO_Port GPIOA
#define RF_B_Pin GPIO_PIN_8
#define RF_B_GPIO_Port GPIOE
#define RF_A_Pin GPIO_PIN_9
#define RF_A_GPIO_Port GPIOE
#define RF_EN_Pin GPIO_PIN_10
#define RF_EN_GPIO_Port GPIOE
#define SPI4_CS_Pin GPIO_PIN_6
#define SPI4_CS_GPIO_Port GPIOH
#define FLASH_WRITE_Pin GPIO_PIN_7
#define FLASH_WRITE_GPIO_Port GPIOH
#define PUMP_PWR_ENA_Pin GPIO_PIN_9
#define PUMP_PWR_ENA_GPIO_Port GPIOH
#define FAN1_PWM_Pin GPIO_PIN_10
#define FAN1_PWM_GPIO_Port GPIOH
#define FAN1_SEN_Pin GPIO_PIN_11
#define FAN1_SEN_GPIO_Port GPIOH
#define SPI2_NSS_Pin GPIO_PIN_12
#define SPI2_NSS_GPIO_Port GPIOH
#define PED_PWR_EN_Pin GPIO_PIN_8
#define PED_PWR_EN_GPIO_Port GPIOD
#define TB_RST_Pin GPIO_PIN_9
#define TB_RST_GPIO_Port GPIOD
#define TB_IN_Pin GPIO_PIN_10
#define TB_IN_GPIO_Port GPIOD
#define IV_PWR_EN_Pin GPIO_PIN_11
#define IV_PWR_EN_GPIO_Port GPIOD
#define R_MOTOR_PWM1_Pin GPIO_PIN_12
#define R_MOTOR_PWM1_GPIO_Port GPIOD
#define R_MOTOR_PWM2_Pin GPIO_PIN_13
#define R_MOTOR_PWM2_GPIO_Port GPIOD
#define L_MOTOR_PWM1_Pin GPIO_PIN_14
#define L_MOTOR_PWM1_GPIO_Port GPIOD
#define L_MOTOR_PWM2_Pin GPIO_PIN_15
#define L_MOTOR_PWM2_GPIO_Port GPIOD
#define STEPPER_ENA_Pin GPIO_PIN_2
#define STEPPER_ENA_GPIO_Port GPIOG
#define SW2_Pin GPIO_PIN_3
#define SW2_GPIO_Port GPIOG
#define INFUSION_VALVE_STATE_Pin GPIO_PIN_4
#define INFUSION_VALVE_STATE_GPIO_Port GPIOG
#define INFSUION_VALVE_EN_Pin GPIO_PIN_5
#define INFSUION_VALVE_EN_GPIO_Port GPIOG
#define REFLUEX_VALVE_EN_Pin GPIO_PIN_6
#define REFLUEX_VALVE_EN_GPIO_Port GPIOG
#define REFLUEX_VALVE_STATE_Pin GPIO_PIN_7
#define REFLUEX_VALVE_STATE_GPIO_Port GPIOG
#define FAN1_PWMC6_Pin GPIO_PIN_6
#define FAN1_PWMC6_GPIO_Port GPIOC
#define FAN1_SENSE_Pin GPIO_PIN_7
#define FAN1_SENSE_GPIO_Port GPIOC
#define FAN2_SENSE_Pin GPIO_PIN_8
#define FAN2_SENSE_GPIO_Port GPIOC
#define FAN2_PWM_Pin GPIO_PIN_9
#define FAN2_PWM_GPIO_Port GPIOC
#define IV_RST_Pin GPIO_PIN_8
#define IV_RST_GPIO_Port GPIOA
#define IV_IN_Pin GPIO_PIN_9
#define IV_IN_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_0
#define LED_GPIO_Port GPIOI
#define MOV_SW6_Pin GPIO_PIN_1
#define MOV_SW6_GPIO_Port GPIOI
#define PLS1_Pin GPIO_PIN_2
#define PLS1_GPIO_Port GPIOI
#define SPI3_CS_Pin GPIO_PIN_15
#define SPI3_CS_GPIO_Port GPIOA
#define MOV_SW1_Pin GPIO_PIN_12
#define MOV_SW1_GPIO_Port GPIOC
#define PHACO_IN_Pin GPIO_PIN_0
#define PHACO_IN_GPIO_Port GPIOD
#define PHACO_E2_SO_SI_Pin GPIO_PIN_1
#define PHACO_E2_SO_SI_GPIO_Port GPIOD
#define PHACO_E2_CLK_Pin GPIO_PIN_2
#define PHACO_E2_CLK_GPIO_Port GPIOD
#define PHACO_E2_CS_Pin GPIO_PIN_3
#define PHACO_E2_CS_GPIO_Port GPIOD
#define SW1_Pin GPIO_PIN_5
#define SW1_GPIO_Port GPIOD
#define CASSETTE_RELEASE_Pin GPIO_PIN_6
#define CASSETTE_RELEASE_GPIO_Port GPIOD
#define SPI1_CS_Pin GPIO_PIN_10
#define SPI1_CS_GPIO_Port GPIOG
#define ADC_READY_Pin GPIO_PIN_12
#define ADC_READY_GPIO_Port GPIOG
#define TEMP_SCL_Pin GPIO_PIN_4
#define TEMP_SCL_GPIO_Port GPIOI
#define TEMP_SDA_Pin GPIO_PIN_5
#define TEMP_SDA_GPIO_Port GPIOI
#define TEMP_ALERT_Pin GPIO_PIN_6
#define TEMP_ALERT_GPIO_Port GPIOI
#define PLS_Pin GPIO_PIN_7
#define PLS_GPIO_Port GPIOI

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
