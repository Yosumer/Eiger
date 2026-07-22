
#include <rtthread.h>
#include <board.h>
#include "bsp_vitCtl.h"

#define LOG_TAG            "drv.airctl"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>


#define  VIT_CH1_EN_PIN            GET_PIN(E,3) 
#define  VIT_CH2_EN_PIN            GET_PIN(E,4)


enum VIT_STATE{
    VIT_STATE_STOP = 0,     //玻切停止状态
    VIT_STATE_RUN,          // 玻切运行状态
};


struct  
{
    enum VIT_STATE   state;      //玻切当前状态
    uint8_t     duty;       // 占空比
    uint8_t     i;          //  定时器使用翻转模式，需要进入两次中断才输出一个完整脉冲
    uint16_t    step_delay; // 脉冲间隔
    uint16_t    step_count; // 总脉冲计数器  
    uint16_t    vit_cpm;    //玻切切速
    uint16_t    vit_lastcount;  //上一次的定时器范围 
    float       frequency;  //定时器基础频率
    void      (*vit_enable)(uint8_t en);
    void     (*vit_SwitchDir)(uint8_t index);  //换向时间点
}vitParams = {0};

static int cmp = 1000;

TIM_HandleTypeDef htim15 = {0};

/**
  * @brief TIM15 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM15_Init(void)
{

  /* USER CODE BEGIN TIM15_Init 0 */

  /* USER CODE END TIM15_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM15_Init 1 */

  /* USER CODE END TIM15_Init 1 */
  htim15.Instance = TIM15;
  htim15.Init.Prescaler = 24000-1;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = 0xffff;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_FORCED_ACTIVE;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_OC_ConfigChannel(&htim15, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
  if (HAL_TIM_OC_ConfigChannel(&htim15, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
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
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim15, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM15_Init 2 */
  /* USER CODE END TIM15_Init 2 */
  HAL_TIM_MspPostInit(&htim15);

}



static void _Vitctl_enbale(uint8_t _en)
{
     rt_pin_write(VIT_CH1_EN_PIN,_en>0?PIN_HIGH:PIN_LOW); 
    if (_en)
    {
        __HAL_TIM_SET_COMPARE(&htim15,TIM_CHANNEL_1,1);
        HAL_TIM_OC_Start(&htim15,TIM_CHANNEL_1);
    }
    else
    {
        HAL_TIM_OC_Stop(&htim15, TIM_CHANNEL_1);
    }
    
}


int Vitctl_Init(void (*func)(uint8_t _index))
{

    if(func != NULL)
        vitParams.vit_SwitchDir = func;
    rt_pin_mode(VIT_CH1_EN_PIN,PIN_MODE_OUTPUT);
    rt_pin_mode(VIT_CH2_EN_PIN, PIN_MODE_OUTPUT);
    vitParams.frequency = 10 *1000; //定时器频率为10khz
    vitParams.vit_enable = _Vitctl_enbale;
	// vitParams.duty = 58;
    vitParams.duty = 58;
    vitParams.state = VIT_STATE_STOP;
    MX_TIM15_Init();
//    Vitctl_SetCutrate(cmp);
}
// INIT_ENV_EXPORT(Vitctl_Init);


void Vitctl_Start(void)
{
    rt_pin_write(VIT_CH2_EN_PIN,PIN_HIGH); 
    HAL_TIM_OC_Start_IT(&htim15,TIM_CHANNEL_2);
	  HAL_TIM_Base_Start(&htim15);
    vitParams.state = VIT_STATE_RUN;
    vitParams.vit_enable(1);
}

void Vitctl_Config(int cutrate)
{
    uint32_t tmp = 0;
    
    if (cutrate>0)
    {
        //老的驱动方式 

        // vitParams.vit_cpm = cutrate;
        // vitParams.step_delay = (float)(60.0/vitParams.vit_cpm) * vitParams.frequency;
        // vitParams.step_count = __HAL_TIM_GET_COUNTER(&htim15); 
        // if (vitParams.i == 1)
        // {
        //     tmp = vitParams.step_count + (((float)(vitParams.step_delay*2)*vitParams.duty)/100);     //计算下次的中断
        // }
        // else if (vitParams.i == 0 || vitParams.i == 2)
        // {
        //     tmp = vitParams.step_count + (((float)(vitParams.step_delay*2)*(100-vitParams.duty))/100);     //计算下次的中断
        // }
        // __HAL_TIM_SET_COMPARE(&htim15,TIM_CHANNEL_2,tmp);  
        // if(vitParams.state == VIT_STATE_STOP)
        // { 
        //     Vitctl_Start();
        // }

        //新的驱动方式
        vitParams.vit_cpm = cutrate;
        vitParams.step_delay = (float)(60.0/vitParams.vit_cpm) * vitParams.frequency;
        uint16_t _postive = (((float)(vitParams.step_delay*2)*vitParams.duty)/100);
        uint16_t _negative = (((float)(vitParams.step_delay*2)*(100-vitParams.duty))/100);
        vitParams.step_count = __HAL_TIM_GET_COUNTER(&htim15); 
        uint16_t _runCnt = vitParams.step_count>vitParams.vit_lastcount?vitParams.step_count-vitParams.vit_lastcount:vitParams.step_count+(0xffff-vitParams.vit_lastcount);

        if(vitParams.state == VIT_STATE_RUN)
        {
            if (vitParams.i == 0 || vitParams.i == 2)
            {
                if (_runCnt<_postive)
                {
                    tmp = vitParams.step_count + (_postive-_runCnt);     //计算下次的中断
                }
                else
                {
                    tmp = vitParams.step_count +10; //立马进行换向
                }
            }
            else
            {
                if (_runCnt<_negative)
                {
                    tmp = vitParams.step_count + (_negative-_runCnt);     //计算下次的中断
                }
                else
                {
                    tmp = vitParams.step_count +10; //立马进行换向	
                }
            }
            __HAL_TIM_SET_COMPARE(&htim15,TIM_CHANNEL_2,tmp); 
        }
        else
        {
            //能到这里说明是第一次启动 
            tmp = vitParams.step_count + (((float)(vitParams.step_delay*2)*(100-vitParams.duty))/100);     //计算下次的中断
            __HAL_TIM_SET_COMPARE(&htim15,TIM_CHANNEL_2,tmp);
            Vitctl_Start();
        }
        vitParams.vit_lastcount = vitParams.step_count;

    }
    else
    {
        Vitctl_Stop();
    }
}

void Vitctl_SetCutrate(int cutrate)
{
    Vitctl_Config(cutrate);
}

void Vitctl_SetDutyCycle(int dutyCycle)
{
    vitParams.duty = dutyCycle;

}
void Vitctl_Stop(void)
{
    rt_pin_write(VIT_CH2_EN_PIN,PIN_LOW);
    vitParams.vit_enable(0);
    HAL_TIM_Base_Stop(&htim15);
    HAL_TIM_OC_Stop_IT(&htim15,TIM_CHANNEL_2);
		
    vitParams.step_count = 0;
    vitParams.state = VIT_STATE_STOP;
    vitParams.step_delay = 0;
    vitParams.vit_cpm = 0;
    __HAL_TIM_SET_COMPARE(&htim15,TIM_CHANNEL_2,0);
    __HAL_TIM_SET_COUNTER(&htim15,10);
}



void Vitctl_Handler(void)
{
	uint32_t tmp = 0;
	// 保存新（下）一个延时周期
//    Vitctl_SetCutrate(cmp);
    if (vitParams.state == VIT_STATE_RUN)
    {
        if(vitParams.vit_SwitchDir != NULL)
            vitParams.vit_SwitchDir(vitParams.i);
        vitParams.i ++;
        vitParams.step_count = __HAL_TIM_GET_COUNTER(&htim15); 
        if (vitParams.i == 1)
        {
            tmp = vitParams.step_count + (((float)(vitParams.step_delay*2)*vitParams.duty)/100);     //计算下次的中断
        }
        else if (vitParams.i == 2)
        {
            tmp = vitParams.step_count + (((float)(vitParams.step_delay*2)*(100-vitParams.duty))/100);     //计算下次的中断
            vitParams.i = 0;
        }
        vitParams.vit_lastcount = vitParams.step_count;
        __HAL_TIM_SET_COMPARE(&htim15,TIM_CHANNEL_2,tmp);
    }
}

void TIM15_IRQHandler(void)
{
  /* USER CODE BEGIN TIM15_IRQn 0 */

  /* USER CODE END TIM15_IRQn 0 */
  HAL_TIM_IRQHandler(&htim15);
  /* USER CODE BEGIN TIM15_IRQn 1 */

  /* USER CODE END TIM15_IRQn 1 */
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM15)
    {
        if (__HAL_TIM_GET_IT_SOURCE(&htim15,TIM_IT_CC2) != RESET)
        {
            Vitctl_Handler();
        }
    }
}
