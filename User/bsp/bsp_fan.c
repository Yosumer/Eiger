/*------------------------------------------------------------------------------
 * @file    BSP_FAN.C
 * @author  FIRMWARE TEAM 
 * @date    2024/07/02 17:00:49
 * @brief   THIS FILE PROVIDES ALL THE FIRMWARE FUNCTIONS.
 * -----------------------------------------------------------------------------
 * @attention 
 
 * THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE 
 * CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE 
 * TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR 
 * CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH 
 * HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN 
 * CONNECTION WITH PRODUCTS MADE BY CUSTOMERS. 
 * 
 * <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2> 
------------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion  ------------------------------------*/
#include <stdint.h>
#include <board.h>
#include "bsp_fan.h"

/* Files includes  -----------------------------------------------------------*/


#define _ASSERT(x)         RT_ASSERT(x)
#define _DEFAULT_RUN_TEMP   32.0
#define MAX_OUT             100
#define MIN_OUT             0
/*----------------------------------------------------------------------------*/
void bsp_fan_Init(struct _bsp_fan_t *_fan, uint32_t _freq, struct _fan_ops *_ops)
{
        _ASSERT(_fan);
        _ASSERT(_ops);
        _fan->_ops = _ops;
        _fan->_set_freq = _freq;
        _fan->_set_temp = _DEFAULT_RUN_TEMP;
        _fan->_enable = 0;  
        // 初始化PID参数
        PID_Constructor(&_fan->_pid);
        // 设置PID积分限幅
        _fan->_pid.SetIntegralPrm(&_fan->_pid,MAX_OUT,MIN_OUT);
        // 3.设置默认PID参数
        _fan->_pid.SetCtrlPrm(&_fan->_pid,4.5,0.5,0);
        // 4.设置最小最大输出
        _fan->_minout = MIN_OUT;
        _fan->_maxout = MAX_OUT;
        _fan->_ops->fan_init(); //初始化底层硬件

}
void bsp_fan_SetCtlPid(struct _bsp_fan_t *_fan,float  kp,  float  ki,  float  kd)
{
    _ASSERT(_fan);
    _fan->_pid.SetCtrlPrm(&_fan->_pid,kp,ki,kd);
}
void bsp_fan_SetTargetTemp(struct _bsp_fan_t *_fan, float _temp)
{
    _ASSERT(_fan);
    _fan->_set_temp = _temp;
}
float bsp_fan_GetTemp(struct _bsp_fan_t *_fan)
{
    _ASSERT(_fan);
    return _fan->_temp_r;
}
uint16_t bsp_fan_GetRmp(struct _bsp_fan_t *_fan)
{
    _ASSERT(_fan);
    _fan->_rmp_r = (1.0/((1.0/_fan->_set_freq)*(float)_fan->_ops->get_raw_cnt())) *60;
    return _fan->_rmp_r;
}
void bsp_fan_Handler(struct _bsp_fan_t *_fan)
{
    _ASSERT(_fan);
    if(_fan->_enable)
    {
        // 1.获取当前温度
        _fan->_temp_r = _fan->_ops->get_temp();
        // 2.获取转速
        _fan->_rmp_r = (1.0/((1.0/_fan->_set_freq)*(float)_fan->_ops->get_raw_cnt())) *60;

        // 3.PID计数
        _fan->_error = _fan->_temp_r - _fan->_set_temp;
        _fan->_out = _fan->_pid.PID(&_fan->_pid,_fan->_error );
        if(_fan->_out>=_fan->_maxout)
            _fan->_out = _fan->_maxout;
        if(_fan->_out<=_fan->_minout)
            _fan->_out = _fan->_minout;
        _fan->_ops->set_pwm(_fan->_out);
    }
}
void bsp_fan_Enable(struct _bsp_fan_t *_fan)
{
    _ASSERT(_fan);
    _fan->_enable =1;
	
}
void bsp_fan_Disable(struct _bsp_fan_t *_fan)
{
    _ASSERT(_fan);
    _fan->_enable =0;
    _fan->_ops->set_pwm(0);     //关闭
}