#ifndef __BSP_VITCTL_H
#define __BSP_VITCTL_H

#include <stdint.h>

int Vitctl_Init(void (*func)(uint8_t _index));

void Vitctl_SetCutrate(int cutrate);    // 100 ~2000

void Vitctl_SetDutyCycle(int dutyCycle); //
void Vitctl_Stop(void);


#endif // !__BSP_VITCTL_H


