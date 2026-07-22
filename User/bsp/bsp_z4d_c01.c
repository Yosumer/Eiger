#include "bsp_z4d_c01.h"
#include "board.h"
#include "bsp_adc.h"
#include <rtthread.h>
#define LOG_TAG            "drv.z4dc01"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>
int bsp_z4dC01_init(bsp_z4dc01 *_this,void (*pwm_fn)(uint8_t _duty),uint16_t (*_adc_fn)())
{
     RT_ASSERT(_this);
    //  RT_ASSERT(pwm_fn);
     RT_ASSERT(_adc_fn);

    _this->_adc_get = _adc_fn;
    _this->_pwm_set = pwm_fn;

    return RT_EOK;

}

uint16_t bsp_z4dC01_read(bsp_z4dc01 *_this)
{
    return _this->_adcdata;
}


TIM_HandleTypeDef htim8 = {0};

static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler =240-1;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 50000-1;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
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
  sConfigOC.OCMode = TIM_OCMODE_PWM2;
  sConfigOC.Pulse = 40000-1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */
  HAL_TIM_MspPostInit(&htim8);

}


void TIM8_CC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM8_CC_IRQn 0 */

  /* USER CODE END TIM8_CC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim8);
  /* USER CODE BEGIN TIM8_CC_IRQn 1 */

  /* USER CODE END TIM8_CC_IRQn 1 */
}


bsp_z4dc01  _z4dc01_1 = { 0};
bsp_z4dc01  _z4dc01_2 = { 0};



uint16_t _bsp_z4dc01_1_get_adc()
{
    return bsp_adc_get_valve(2,0);
}


uint16_t _bsp_z4dc01_2_get_adc()
{
    return bsp_adc_get_valve(2,1);
}



static struct rt_completion completion_tim4_ch3;
static struct rt_completion completion_tim4_ch4;


void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM8)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
        {
            if (_z4dc01_1._status == 1)
            {
                rt_completion_done(&completion_tim4_ch3);
            }
        
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
        {
            if (_z4dc01_2._status == 1)
            {
                rt_completion_done(&completion_tim4_ch4);
            }
        }
    }
    
}

static void _bsp_z4dc01_app1_Task(void *p)
{
    HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_3);
    while (1)
    {
        _z4dc01_1._status = 1;
        HAL_TIM_PWM_Start_IT(&htim8,TIM_CHANNEL_3);
        rt_completion_wait(&completion_tim4_ch3,RT_WAITING_FOREVER);
		rt_thread_delay(5);
        _z4dc01_1._status = 0;
        _z4dc01_1._adcdata = _z4dc01_1._adc_get();
    }
    
}


static void _bsp_z4dc01_app2_Task(void *p)
{
    HAL_TIM_PWM_Start_IT(&htim8, TIM_CHANNEL_4);
    while (1)
    {
        _z4dc01_2._status = 1;
        rt_completion_wait(&completion_tim4_ch4,RT_WAITING_FOREVER);
		rt_thread_delay(5);
        _z4dc01_2._status = 0;
        _z4dc01_2._adcdata = _z4dc01_2._adc_get();
    }
    
}

int bsp_z4dC01_Task_Init(void)
{
	
	MX_TIM8_Init();
	bsp_z4dC01_init(&_z4dc01_1,RT_NULL,_bsp_z4dc01_1_get_adc);
    bsp_z4dC01_init(&_z4dc01_2,RT_NULL,_bsp_z4dc01_2_get_adc);

    rt_completion_init(&completion_tim4_ch3);
    rt_completion_init(&completion_tim4_ch4);
    // HAL_TIM_PWM_Start_IT(&htim4,TIM_CHANNEL_3);
    // HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_4);
    HAL_TIM_Base_Start(&htim8);
    _z4dc01_1._status = 0;
    _z4dc01_2._status = 0;
   rt_thread_t _z4dc01_01 =rt_thread_create("_z4dc01_task",_bsp_z4dc01_app1_Task,RT_NULL,256,12,10);
   if(RT_NULL ==_z4dc01_01)
       LOG_E("Create _task faild");
   rt_thread_startup(_z4dc01_01);

    rt_thread_t _z4dc01_02 =rt_thread_create("_z4dc02_task",_bsp_z4dc01_app2_Task,RT_NULL,256,12,10);
    if(RT_NULL ==_z4dc01_02)
        LOG_E("Create _task faild");
    rt_thread_startup(_z4dc01_02);
}
#if (DECETE_MODE == USE_LIGHT_MODE)
//    INIT_APP_EXPORT(bsp_z4dC01_Task_Init);  //Sensor Task �����ʼ����
#endif