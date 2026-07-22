
#ifndef BSP_STEP_TMC5160_H 
#define BSP_STEP_TMC5160_H 

#include "bsp_step_motor.h"


#define DRAINED_MAX_SPEED       60 //cc/min    //150000

#define SPR_TMC5160             7000 

typedef struct _bsp_step_tmc_conf
{
    bsp_step_motor_conf _base;
    uint32_t vstart;
    uint32_t a1;
    uint32_t v1;
    uint32_t d1;
    uint32_t vstop;
    uint32_t rampmode;
}bsp_step_tmc_conf;

extern bsp_step_motor_t  tmc5160_motor;
extern bsp_step_tmc_conf tmc5160_cof;
#endif // BSP_STEP_TMC5160_H