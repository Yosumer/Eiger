

#ifndef BSP_STEP_L6470_H 
#define BSP_STEP_L6470_H 
#include "bsp_step_motor.h"

#define SPR_L6470_MOTOR             200     //默认电机一圈的脉冲数 

typedef struct _bsp_step_l6470_conf
{
    bsp_step_motor_conf _base;
    void *_encoder;     //编码器硬件指针
    uint8_t spi_bus_name[8];
    uint8_t spi_dev_name[8];
    uint32_t cs_pin;
    uint32_t flag_pin;
    uint32_t reset_pin;
    uint32_t busy_pin;
    float               _motorVol;  // 电机驱动电压
    //phase current Control
    float               _kvalhold;  /* Hold duty cycle (torque) in %, range 0 to 99.6% */
    float               _kvalrun;   /* Run duty cycle (torque) in %, range 0 to 99.6% */
    float               _kvalacc;   /* Acceleration duty cycle (torque) in %, range 0 to 99.6% */
    float               _kvaldec;   /* Deceleration duty cycle (torque) in %, range 0 to 99.6% */
    float               _kterm;  /* Thermal compensation param, range 1 to 1.46875 */


    float               _intspeed;           /* Intersect speed settings for BEMF compensation in steps/s, range 0 to 3906 steps/s */
    float               _stslp;                   /* BEMF start slope settings for BEMF compensation in % step/s, range 0 to 0.4% s/step */
    float               _fnslpacc;          /* BEMF final acc slope settings for BEMF compensation in % step/s, range 0 to 0.4% s/step  */
    float               _fnslpdec;          /* BEMF final dec slope settings for BEMF compensation in % step/s, range 0 to 0.4% s/step */

    //other
    float               _ocdth;             /* Overcurrent threshold settings*/
    float               _stallth;           /* Stall threshold settings in mA, range 31.25mA to 4000mA */
    uint8_t               _stepmode;          //细分设置
    uint8_t               _alarmen;           //Alarm settings
}bsp_step_l6470_conf;   




extern bsp_step_motor_t     l6470_motor;
extern bsp_step_l6470_conf  l6470_cof;


#endif