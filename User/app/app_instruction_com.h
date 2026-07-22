#ifndef __APP_INSTRUCT_COMMON_H_
#define __APP_INSTRUCT_COMMON_H_

#include <stdint.h>

#include "app_instruction_opt.h"

#define RAM_CALIB_DATA_ADDR 	(RAM2_BASE + 0x0100*4)

float	Calib_linearFitting(uint32_t n, float *x,float *y, CalibrationPara *p);

int 	polyfit(const float* const dependentValues,
            const float* const independentValues,
            unsigned int		countOfElements,
            unsigned int        order,
            float*      		coefficients);
			
float 	calculate_error(float x[], float y[], int n, float a, float b, float c);	

float 	calculate_Rsquard(float x[], float y[], int n, float a, float b, float c);		

void Flash_DiaParam_init(void);
void Flash_PhacoParam_init(void);

#endif /* __APP_INSTRUCT_COMMON_H_  */
