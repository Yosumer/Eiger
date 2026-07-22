#ifndef __BSP_TIMER2_PWM_H
#define __BSP_TIMER2_PWM_H
#include <stdint.h>






void bsp_timer2_pwm_Init();
void bsp_timer2_Start_PWM_CH1_DMA(uint32_t *buf,uint16_t _len);
#endif // !__BSP_TIMER2_PWM_H
