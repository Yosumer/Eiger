
#include <rtthread.h>
#include <board.h>
#include "bsp_valve.h"
#include "includes.h"


#define LOG_TAG            "drv.valve"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>




// enum VALVE_CHANNEL {
//     VALVE_IV_BREAK = 0, // IV杆刹车信号
//     VALVE_INFUSION,     // 灌注阀门
//     VALVE_REFLUX,        //回流阀门
//     VALVE_AIR_PUMB,     //隔膜泵控制
//     VALVE_LEFT_BOX,     //积液盒左进
//     VALVE_RIGHT_BOX,    // 积液盒右进  
//     VALVE_MAX
// };

//IV 杆相关IO
#define   IV_PWR_PIN                    GET_PIN(D, 11)  //IV 杆电源
#define   IV_RST_PIN                    GET_PIN(A, 8)  //IV 杆复位

//踏板相关IO
#define   PEN_PWR_PIN                   GET_PIN(D, 8)   //踏板电源检测   
#define   PEN_RST_PIN                   GET_PIN(D, 9)   //踏板复位

#define   INFUSION_PIN_EN               GET_PIN(G, 5)
#define   REFLUX_PIN_EN                 GET_PIN(G, 6)


#define  AIR_PUMB_PWR_EN                GET_PIN(H,9) 


#define  LEFT_BOX_MOTOR_UP             TIM_CHANNEL_4
#define  LEFT_BOX_MOTOR_DOWN           TIM_CHANNEL_3


#define  RIGHT_BOX_MOTOR_UP             TIM_CHANNEL_1
#define  RIGHT_BOX_MOTOR_DOWN           TIM_CHANNEL_2

#define MOTOR_FRQ_ARR                100

TIM_HandleTypeDef htim4 = {0};

extern Data_Block  ramdata;         //全局数据块
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 24-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = MOTOR_FRQ_ARR-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

static void _motor_set_duty(uint32_t _ch, uint8_t _duty)
{
    uint16_t _plus = (_duty*MOTOR_FRQ_ARR)/100;
    __HAL_TIM_SET_COMPARE(&htim4,_ch,_plus);
    HAL_TIM_PWM_Start(&htim4,_ch);
}



int bsp_valve_Init(void)
{
    rt_pin_mode(IV_PWR_PIN,PIN_MODE_OUTPUT);
    rt_pin_mode(IV_RST_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(PEN_PWR_PIN,PIN_MODE_OUTPUT);
    rt_pin_mode(PEN_RST_PIN, PIN_MODE_OUTPUT);

    rt_pin_mode(INFUSION_PIN_EN, PIN_MODE_OUTPUT);
    rt_pin_mode(REFLUX_PIN_EN,  PIN_MODE_OUTPUT);
    // rt_pin_mode(REFLUX_PIN,PIN_MODE_OUTPUT);
    rt_pin_mode(AIR_PUMB_PWR_EN, PIN_MODE_OUTPUT);

    MX_TIM4_Init();
    bsp_valve_ctlAll(0);    //关闭所有阀门

    // bsp_valve_Ctl(VALVE_IV_PWR,1);    //打开IV 杆限制阀门
	// bsp_valve_Ctl(VALVE_AIR_PUMB,1);
}

INIT_ENV_EXPORT(bsp_valve_Init);
void bsp_valve_Ctl(uint8_t ch, uint8_t _ctl)
{
    switch (ch)
    {
        case VALVE_BIT_CTL:
            {
                uint8_t _isenable =  _ctl%2; //取最低位是否使能
                uint8_t _bit = _ctl/2;      //需要操作的阀门
                if (_isenable)
                {
                    BIT_SET(ramdata._pwrstaus,_bit);
                }
                else
                {
                    BIT_CLEAR(ramdata._pwrstaus,_bit);
                }
                rt_base_t _pin = 0;
                if (_bit == 0)
                {
                    _pin = IV_PWR_PIN;
                }
                else if (_bit == 1)
                {
                    _pin = IV_RST_PIN;
                }
                else if (_bit == 2)
                {
                    _pin = PEN_PWR_PIN;
                }
                else if (_bit == 3)
                {
                    _pin = PEN_RST_PIN;
                }
                rt_pin_write(_pin,_isenable);  
            }

            break;
        case VALVE_INFUSION:
            rt_pin_write(INFUSION_PIN_EN,_ctl>0?PIN_LOW:PIN_HIGH); 
            break;
        case VALVE_REFLUX:
            rt_pin_write(REFLUX_PIN_EN,_ctl>0?PIN_LOW:PIN_HIGH); 
            break;
        case VALVE_AIR_PUMB:
            rt_pin_write(AIR_PUMB_PWR_EN,_ctl>0?PIN_HIGH:PIN_LOW);
            break;
        case VALVE_BOX_IN:
            {   
                //左积液盒进
                _motor_set_duty(LEFT_BOX_MOTOR_UP,_ctl);
                // _motor_set_duty(LEFT_BOX_MOTOR_DOWN,0);
                //右积液盒进
                _motor_set_duty(RIGHT_BOX_MOTOR_UP,_ctl);
                // _motor_set_duty(RIGHT_BOX_MOTOR_DOWN,0);
            }
            break;
        case VALVE_BOX_OUT:
            {
                //左积液盒退
                // _motor_set_duty(LEFT_BOX_MOTOR_UP,0);
                _motor_set_duty(LEFT_BOX_MOTOR_DOWN,_ctl);
                //右积液盒退
                // _motor_set_duty(RIGHT_BOX_MOTOR_UP,0);
                _motor_set_duty(RIGHT_BOX_MOTOR_DOWN,_ctl);
            }
            break;
        default:
            break;
    }

    ramdata._valve.buf[ch] = _ctl;
}



uint8_t bsp_valve_read(uint8_t ch)
{
    switch (ch)
    {
        case VALVE_BIT_CTL:
        {
            return ramdata._pwrstaus;   //电源上电状态 0bit: IV杆阀门电源 1:IV杆复位阀门 2bit:踏板电源控制 3bit:踏板复位
        }
        break;
        default:
            return ramdata._valve.buf[ch];
    }
}
void bsp_valve_ctlAll(uint8_t _ctl)
{
    for(uint8_t i = 0; i <VALVE_MAX;i++)
    {
        bsp_valve_Ctl(i,_ctl);
    }
}


void bsp_pwmValve_Ctl(uint8_t ch,uint16_t duty)
{
    switch (ch)
    {
        case MOTOR_LEFT_UP:
            _motor_set_duty(LEFT_BOX_MOTOR_UP,duty);
            break;
        case MOTOR_LEFT_DOWN:
            _motor_set_duty(LEFT_BOX_MOTOR_DOWN,duty);
            break;
            
        case MOTOR_RIGHT_UP:
            _motor_set_duty(RIGHT_BOX_MOTOR_UP,duty);
            break;
            
        case MOTOR_RIGHT_DOWN:
            _motor_set_duty(RIGHT_BOX_MOTOR_DOWN,duty);
            break;

        default:
            break;
    }
}