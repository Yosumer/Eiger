/*------------------------------------------------------------------------------
 * @file    BSP_DIATHERMY.C
 * @author  FIRMWARE TEAM
 * @date    2024/07/12 15:39:08
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
#define _BSP_DIATHERMY_C_

/* Files includes  -----------------------------------------------------------*/
#include "bsp_diathermy.h"
#include "bsp_timer1_pwm.h"  //V1.1使用定时器1
// #include "bsp_timer8_pwm.h"     //V2.0使用定时器8
#include "drv_dac_mcp4725.h"
#include "bsp_adc.h"
#include <math.h>
#include <rtdevice.h>
#define LOG_TAG            "drv.dia"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>
/*----------------------------------------------------------------------------*/
#define DAC_EN_PIN      "PF.2"
#define PWM_EN_PIN      "PE.10"
#define DAC_DEV_NAME    MCP4725_DAC_NAME
#define DAC_DEV_CHANNEL  1

#define OUT_PUT_MAXADC		  (2294)

#define DEFAULT_DIA_PWR_MAX		  10.0f
#define DEFAULT_DIA_FREQ

rt_base_t _dac_en_pin;
rt_base_t _pwm_en_pin;
rt_dac_device_t  _dac_dev;

bsp_dia_t    _dia_instance;

static void  _bsp_dia_enableIO(void *_ptr)
{
    bsp_dia_t *_this = (bsp_dia_t *)_ptr;
    rt_pin_write(_dac_en_pin, 1);
    rt_pin_write(_pwm_en_pin, 1);
    if(_this != RT_NULL)
        _this->_enable = 1;
}

static void _bsp_dia_disableIO(void *_ptr)
{
    bsp_dia_t *_this = (bsp_dia_t *)_ptr;
    rt_pin_write(_pwm_en_pin, 0);
    rt_pin_write(_dac_en_pin, 0);
    if(_this != RT_NULL)
        _this->_enable = 0;
}
void bsp_dia_test_io(uint8_t _io,uint8_t _en)
{
    if(_io == 0)
    {
        rt_pin_write(_dac_en_pin, _en);
    }
    else if(_io == 1)
    {
        rt_pin_write(_pwm_en_pin, _en);
    }
}
static void _bsp_dia_SetDac(uint16_t _val)
{
    rt_dac_write(_dac_dev, DAC_DEV_CHANNEL, _val);
}

static uint16_t _bsp_dia_getVol_ADC()
{
    return bsp_get_valve(V_ADC_CH);
}

static uint16_t _bsp_dia_getCur_ADC()
{
    return bsp_get_valve(I_ADC_CH);
}
static void _bsp_drv_init(uint16_t _freq)
{
    // 1.初始化IO
    _dac_en_pin = rt_pin_get(DAC_EN_PIN);
    rt_pin_mode(_dac_en_pin, PIN_MODE_OUTPUT);
    _pwm_en_pin = rt_pin_get(PWM_EN_PIN);
    rt_pin_mode(_pwm_en_pin, PIN_MODE_OUTPUT);
    _bsp_dia_disableIO(RT_NULL);   //关闭输出
    // 2.初始化DAC
    _dac_dev = (rt_dac_device_t)rt_device_find(DAC_DEV_NAME);
    if (_dac_dev == RT_NULL)
    {
        LOG_E("dac sample run failed! can't find %s device!\n", DAC_DEV_NAME);
    }
    // 3.使能DAC
    rt_dac_enable(_dac_dev, DAC_DEV_CHANNEL);
    // 4.初始化PWM
    bsp_timer1_pwm_Init(_freq);        //      v1.1版本的pwm产生通道
    //  bsp_timer8_pwm_Init();          //      v2.0版本的pwm产生通道
}


int bsp_dia_check_out_range(bsp_dia_t *_this)
{
    if((_this->_cal->_outMax >= 3.0f) && (_this->_cal->_outMax <= 13.0f) )
        return 1;
    return 0;
}

int bsp_dia_check_freq_range(bsp_dia_t *_this)
{
    if((_this->_cal->_diaFreq >= 1400) && (_this->_cal->_diaFreq <= 1600) )
        return 1;
    return 0;
}




void bsp_dia_init(bsp_dia_t *_this, bsp_dia_ops *_ops, CalibrationPara _vcal, CalibrationPara _ical, CalibrationPara _outcal, float _outMax, uint16_t freq)
{
    RT_ASSERT(_this);
    RT_ASSERT(_ops);
    _this->_ops = _ops;
    if(0 == bsp_isCalParamEmpty(&_this->_cal->_vcal))
        _this->_cal->_vcal = _vcal;

    if(0 == bsp_isCalParamEmpty(&_this->_cal->_ical))
        _this->_cal->_ical = _ical;

    if(0 == bsp_isCalParamEmpty(&_this->_cal->_outcal))
        _this->_cal->_outcal = _outcal;

    if(0 == bsp_dia_check_out_range(_this))
        _this->_cal->_outMax = _outMax;

    if(0 == bsp_dia_check_freq_range(_this))
        _this->_cal->_diaFreq = freq;

    _this->_ops->init(_this->_cal->_diaFreq );
}


void bsp_dia_setPower(bsp_dia_t *_this, float _power)
{
    RT_ASSERT(_this);

    if(_power == 0)
    {
        _this->_info._setpow = 0;
        _this->_ops->disable(_this); //关闭功能输出
    }
    else
    {
        if(_this->_info._setpow != _power)
        {
            _power = _power * _this->_cal->_outMax / DEFAULT_DIA_PWR_MAX;

            _this->_info._dacval = bsp_abstractLineFuncBeta_GetOut(_power, &_this->_cal->_outcal);

            if(_this->_info._dacval > OUT_PUT_MAXADC)
                _this->_info._dacval = OUT_PUT_MAXADC;

            _this->_ops->set_dac(_this->_info._dacval);
            _this->_info._setpow = _power;
        }

        _this->_ops->enable(_this);     //使能输出
    }
}

void bsp_dia_setDac(bsp_dia_t *_this, uint16_t _dacvalve)
{
    RT_ASSERT(_this);
    _this->_ops->set_dac(_dacvalve);    
}
void bsp_dia_set_Vol_Calib(bsp_dia_t *_this, CalibrationPara _vcal)
{
    RT_ASSERT(_this);
    rt_base_t _lev =  rt_hw_interrupt_disable();
    _this->_cal->_vcal = _vcal;
    rt_hw_interrupt_enable(_lev);
}
void bsp_dia_set_Curr_Calib(bsp_dia_t *_this, CalibrationPara _ical)
{
    RT_ASSERT(_this);
    rt_base_t _lev =  rt_hw_interrupt_disable();
    _this->_cal->_ical = _ical;
    rt_hw_interrupt_enable(_lev);
}

void bsp_dia_set_Out_Calib(bsp_dia_t *_this, CalibrationPara _outcal)
{
    RT_ASSERT(_this);
    rt_base_t _lev =  rt_hw_interrupt_disable();
    _this->_cal->_outcal = _outcal;
    rt_hw_interrupt_enable(_lev);
}

bsp_dia_info bsp_dia_getInfo(bsp_dia_t *_this)
{
    RT_ASSERT(_this);
    return _this->_info;
}

void __bsp_diathermy_Task(void *_handler)
{
    bsp_dia_t *_this = (bsp_dia_t *)_handler;

    while (1)
    {
        if(_this->_enable)
        {
            _this->_info._current = bsp_abstractLineFunc_GetOut(_this->_ops->get_curr(), &_this->_cal->_ical) / 1000;
            _this->_info._vol = bsp_abstractLineFunc_GetOut(_this->_ops->get_vol(), &_this->_cal->_vcal);
            _this->_info._res = (_this->_info._vol) / (_this->_info._current);
            _this->_info._calpow = (_this->_info._current * _this->_info._vol);
        }

        rt_thread_delay(1);
    }
}


void set_dia_freq(uint16_t freq)
{
    _dia_instance._cal->_diaFreq = freq;
}


#include "ErigerConfig.h"

void save_dia_para(void)
{
    struct Flash_Sensor  flash_para;
    memcpy(&flash_para, (uint8_t *)_dia_instance._cal, sizeof(struct Flash_Sensor) - sizeof(uint32_t));
    flash_para._unuse = 0x55AA;
    Flash_Task_Write("sensor", (uint8_t *)&flash_para, sizeof(struct Flash_Sensor));
}

static bsp_dia_ops  _ops = {
    .init		= _bsp_drv_init,
    .disable 	= _bsp_dia_disableIO,
    .enable 	= _bsp_dia_enableIO,
    .get_curr 	= _bsp_dia_getCur_ADC,
    .get_vol 	= _bsp_dia_getVol_ADC,
    .set_dac 	= _bsp_dia_SetDac
};

static CalibrationPara _vC = {
    .A = 0.0141,
    .B = -1.3555
};
static CalibrationPara _iC = {
    .A = 0.2743,
    .B = -27.886
};

static CalibrationPara _oC = {
    .A = 8.9161377f,
    .B = -241.583984f,
    .C = 1695.06641f
};

/*
*********************************************************************************************************
*	函 数 名: bsp_dia_default
*	功能说明: 设定默认的电凝参数 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_dia_default(void)
{
	bsp_dia_init(&_dia_instance, &_ops, _vC, _iC, _oC, DEFAULT_DIA_PWR_MAX, 1500);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_dia_register
*	功能说明: 绑定校准参数 
*	形    参: instance 指向校准参数的指针
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_dia_register(dia_cal_param *instance)
{
	_dia_instance._cal = instance;
}

int _bsp_diathermy_test()
{
    bsp_dia_default();
    rt_thread_t _dia_task = rt_thread_create("_dia_task", __bsp_diathermy_Task, &_dia_instance, 512, 29, 10);

    if(RT_NULL == _dia_task)
        LOG_E("Create _dia_task faild");
    rt_thread_startup(_dia_task);
}
INIT_APP_EXPORT(_bsp_diathermy_test);



