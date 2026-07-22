#ifndef  __BSP_ENCODE_TIMER8_H
#define __BSP_ENCODE_TIMER8_H

#include <stdint.h>
#include <board.h>
void bsp_Encoder_Timer8_Init();
uint32_t bsp_Encoder_Timer8_GetCount();
uint32_t bsp_Encoder_Timer8_GetDir();
void bsp_Encoder_Timer8_SetCount(uint16_t _count);


#endif // ! __BSP_ENCODE_TIMER8_H
