#include "rtthread.h"
#include <rtdevice.h>
#include "board.h"
#include "bsp_step_l6470.h"
#include "bsp_l6470_motor.h"

#include "bsp_abstractEncode.h"
#include "bsp_encode_Timer8.h"
#define LOG_TAG            "drv.Ml6470"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>



#define  ENCODER_SPR            (5000)          //使用编码器的当前脉冲数 


bsp_step_motor_t  l6470_motor;
bsp_step_l6470_conf l6470_cof = {
    ._base = {
        ._spr = 25600,  // 128细分
        // ._espr = 360*4,  //第一代编码器一圈360
        ._espr = ENCODER_SPR*4,    //5000千线编码器

        //  ._dmax = 800,   // step/s   对于L6470来说转速就是基于原始脉冲来说的
        //  ._amax = 800,
       ._dmax = 2000,   // step/s   对于L6470来说转速就是基于原始脉冲来说的
       ._amax = 1000,
        ._vmax = 1000,  
    },
    ._motorVol = 24.0,  //电压24v
    ._kvalhold = 30.08,
    // ._kvalhold = 25.56,
    ._kvalrun = 3.52,
    ._kvalacc = 1.2,
    ._kvaldec = 1.2,
    ._kterm = 1.03125, 
    ._intspeed = 2.58,
    ._stslp = 0,
//    ._fnslpacc = 0.00153,
	._fnslpacc = 0.00300,
    // ._fnslpdec = 0.00153,
    ._fnslpdec =  0.00300,
    // ._ocdth = 1 * 500 * 1.00,
    // ._stallth = 500 *1.00,
    ._ocdth = 1 * 1200 * 1.00,
    ._stallth = 1200 *1.00,

    ._alarmen = L6470_OVERCURRENT|L6470_THERMAL_WARNING|L6470_WRONG_OR_NON_PERFORMABLE_COMMAND,
    ._stepmode = MICROSTEP_1_128,

    .spi_bus_name = "spi3",
    .spi_dev_name = "spi30",
    .busy_pin = GET_PIN(D, 3),
    .cs_pin = GET_PIN(D, 2),
    .flag_pin = GET_PIN(D, 1),
    .reset_pin = GET_PIN(I, 3),
};



static void _bsp_step_l6470_Config(void *_me, bsp_step_motor_conf *_cof);
static uint8_t _bsp_step_l6470_Init(void *_me);
static void _bsp_step_l6470_Rotate(void *_me,int32_t velocity);
static void _bsp_step_l6470_Stop(void *_me);
static void _bsp_step_l6470_MoveTo(void *_me,int32_t position);
static void _bsp_step_l6470_MoveBy(void *_me,int32_t step);
static void _bsp_step_l6470_SetPos(void *_me,int32_t _pos);
static uint32_t _bsp_step_l6470_Getstatus(void *_me);
static int32_t _bsp_step_l6470_getvelocity(void *_me);
static int32_t _bsp_step_l6470_getPos(void *_me);
static int32_t _bsp_step_l6470_getEncoder(void *_me);
static void _bsp_step_l6470_task(void *_this);


bsp_step_motor_ops  _l6470_ops = {
    .moveBy = _bsp_step_l6470_MoveBy,
    .config = _bsp_step_l6470_Config,
    .get_encode = _bsp_step_l6470_getEncoder,
    .get_position = _bsp_step_l6470_getPos,
    .get_status = _bsp_step_l6470_Getstatus,
    .get_velocity = _bsp_step_l6470_getvelocity,
    .moveTo = _bsp_step_l6470_MoveTo,
    .rotate = _bsp_step_l6470_Rotate,
    .set_position = _bsp_step_l6470_SetPos,
    .stop = _bsp_step_l6470_Stop,
    .task = _bsp_step_l6470_task,
    .init = _bsp_step_l6470_Init
};



static void _bsp_step_l6470_Config(void *_me, bsp_step_motor_conf *_cof)
{
    struct motor_l6470 *_motor = (struct motor_l6470 *)(((bsp_step_motor_t *)_me)->instance);
    if(RT_NULL == _motor)
    {
        LOG_E("L6470 Motor Config Faild, Please Init First!!");
        return;
    }
    
    bsp_step_l6470_conf *_l6470_cof = (bsp_step_l6470_conf *)_cof;
    MotorParameterData_t _l6470data = {
        .motorvoltage = _l6470_cof->_motorVol,
        .fullstepsperrevolution = 200,
        .phasecurrent = 2.5,
        .phasevoltage = 5.0,
        .speed = _l6470_cof->_base._vmax,
        .acc =_l6470_cof->_base._amax,
        .dec = _l6470_cof->_base._dmax,
        .maxspeed = _l6470_cof->_base._vmax,
        .minspeed = 0.0,
        .fsspd = 1000,
        .kvalhold = _l6470_cof->_kvalhold,
        .kvalrun =  _l6470_cof->_kvalrun,
        .kvalacc = _l6470_cof->_kvalacc,
        .kvaldec = _l6470_cof->_kvaldec,
        .intspeed = _l6470_cof->_intspeed,
        .stslp = _l6470_cof->_stslp,
        .fnslpacc = _l6470_cof->_fnslpacc,
        .fnslpdec =  _l6470_cof->_fnslpdec,
        .kterm = (uint8_t)_l6470_cof->_kterm,
        .ocdth =  _l6470_cof->_ocdth,
        .stallth = _l6470_cof->_stallth,
        .step_sel = _l6470_cof->_stepmode,
        // .alarmen = L6470_OVERCURRENT|L6470_THERMAL_WARNING|L6470_WRONG_OR_NON_PERFORMABLE_COMMAND,
        .alarmen = _l6470_cof->_alarmen,
        // .config = 0x703B,
        .config = 0x001B,
    };
    bsp_l6470_motor_config(_motor,&_l6470data);
}
static uint8_t _bsp_step_l6470_Init(void *_me)
{
    abstractEncoder_t *encoder = rt_calloc(1, sizeof(abstractEncoder_t));
    hw_inter_t *_hw_encoder = rt_calloc(1, sizeof(hw_inter_t));
    Encoder_conf_t *_econf = rt_calloc(1, sizeof(Encoder_conf_t));
    if (_hw_encoder == RT_NULL || encoder == RT_NULL || _econf == RT_NULL)
    { 
        LOG_E("bsp_l6470_motor_create Encoder not allocate  memory");
        return 0;
    }
    
    bsp_step_motor_t *_motor = (bsp_step_motor_t *)_me;


    bsp_step_l6470_conf *_cof = (bsp_step_l6470_conf*) (_motor->cof);
        //配置定时器指针
    _hw_encoder->_Init = bsp_Encoder_Timer8_Init;
    _hw_encoder->_setCounter = bsp_Encoder_Timer8_SetCount;
    _hw_encoder->_getCounter = bsp_Encoder_Timer8_GetCount;
    _hw_encoder->_getDir = bsp_Encoder_Timer8_GetDir;
    _cof->_encoder = encoder;
    
        // 初始化编码器默认配置
    _econf->_time = 100; //100ms
    _econf->_resolution = 60000;
    _econf->_Counting_mode = COUNTING_MODE_4;
    // _econf->_scale = 0.55556;        // 电机一圈200个脉冲 / 编码器一圈360
    _econf->_scale = (float)((200.0/(float)ENCODER_SPR));        // 电机一圈200个脉冲 / 编码器一圈5000
    bsp_abstractEncode_Init(_cof->_encoder,_hw_encoder,_econf);
    struct motor_l6470 * _l6470_dev = bsp_l6470_motor_create(_cof->spi_bus_name,_cof->spi_dev_name,_cof->cs_pin,_cof->flag_pin,_cof->reset_pin,_cof->busy_pin);
    if (RT_NULL == _l6470_dev)
    {
        LOG_E("bsp_l6470_motor_create failed");
        return 0;
    }
    _motor->instance = _l6470_dev;
    return 1;
}
static void _bsp_step_l6470_Rotate(void *_me,int32_t velocity)
{
    struct motor_l6470 *_motor = (struct motor_l6470 *)(((bsp_step_motor_t *)_me)->instance);
    if(RT_NULL == _motor)
    {
        LOG_E("L6470 Motor Rotate Faild, Please Init First!!");
        return;
    }
    if(velocity == 0)
    {
        _bsp_step_l6470_Stop(_me);
    }
    else
    {
        motor_l6470_dir _dir = (velocity>0)?CW:CWW;
        if (velocity<0)
        {
            velocity = -velocity;
        }
        bsp_l6470_motor_run(_motor,_dir,velocity);
    }
}
static void _bsp_step_l6470_Stop(void *_me)
{
    struct motor_l6470 *_motor = (struct motor_l6470 *)(((bsp_step_motor_t *)_me)->instance);
    if(RT_NULL == _motor)
    {
        LOG_E("L6470 Motor Stop Faild, Please Init First!!");
        return;
    }
    bsp_l6470_motor_hardHiZ(_motor);
    bsp_l6470_motor_hardStop(_motor);
    bsp_l6470_motor_softStop(_motor);
    abstractEncoder_t *_encoder = ( abstractEncoder_t *)(((struct _bsp_step_l6470_conf *)(((bsp_step_motor_t *)_me)->cof))->_encoder);
    if(RT_NULL != _encoder)
    {
        bsp_abstractEncode_Reset(_encoder);
    }
}
static void _bsp_step_l6470_MoveTo(void *_me,int32_t position)
{
    struct motor_l6470 *_motor = (struct motor_l6470 *)(((bsp_step_motor_t *)_me)->instance);
    if(RT_NULL == _motor)
    {
        LOG_E("L6470 Motor MoveTo Faild, Please Init First!!");
        return;
    }
    bsp_l6470_motor_goto(_motor,position);
}
static void _bsp_step_l6470_MoveBy(void *_me,int32_t step)
{
    struct motor_l6470 *_motor = (struct motor_l6470 *)(((bsp_step_motor_t *)_me)->instance);
    if(RT_NULL == _motor)
    {
        LOG_E("L6470 Motor MoveBy Faild, Please Init First!!");
        return;
    }
    bsp_l6470_motor_move(_motor,(step>=0)?CW:CWW,abs(step));
}
static void _bsp_step_l6470_SetPos(void *_me,int32_t _pos)
{
    struct motor_l6470 *_motor = (struct motor_l6470 *)(((bsp_step_motor_t *)_me)->instance);
    if(RT_NULL == _motor)
    {
        LOG_E("L6470 Motor SetPos Faild, Please Init First!!");
        return;
    }
    bsp_l6470_motor_setPos(_motor,_pos);

}
static uint32_t _bsp_step_l6470_Getstatus(void *_me)
{
    struct motor_l6470 *_motor = (struct motor_l6470 *)(((bsp_step_motor_t *)_me)->instance);
    if(RT_NULL == _motor)
    {
        LOG_E("L6470 Motor Getstatus Faild, Please Init First!!");
        return MOTOR_ERROR;
    }
    return (uint32_t)bsp_l6470_motor_getState(_motor);
}
static int32_t _bsp_step_l6470_getvelocity(void *_me)
{
    struct motor_l6470 *_motor = (struct motor_l6470 *)(((bsp_step_motor_t *)_me)->instance);
    if(RT_NULL == _motor)
    {
        LOG_E("L6470 Motor getvelocity Faild, Please Init First!!");
        return 0;
    }
    return (uint32_t)bsp_l6470_motor_getSpeed(_motor);
}
static int32_t _bsp_step_l6470_getPos(void *_me)
{
    struct motor_l6470 *_motor = (struct motor_l6470 *)(((bsp_step_motor_t *)_me)->instance);
    if(RT_NULL == _motor)
    {
        LOG_E("L6470 Motor getPos Faild, Please Init First!!");
        return 0;
    }
    return (uint32_t)bsp_l6470_motor_getCoor(_motor);
}
static int32_t _bsp_step_l6470_getEncoder(void *_me)
{
    abstractEncoder_t *_encoder = ( abstractEncoder_t *)(((struct _bsp_step_l6470_conf *)(((bsp_step_motor_t *)_me)->cof))->_encoder);
    if(RT_NULL == _encoder)
    {
        LOG_E("L6470 Motor getEncoder Faild, Please Init First!!");
        return 0;
    }
    return bsp_abstractEncode_GetCount(_encoder);

}
static void _bsp_step_l6470_task(void *_this)
{
    abstractEncoder_t *_encoder = ( abstractEncoder_t *)(((struct _bsp_step_l6470_conf *)(((bsp_step_motor_t *)_this)->cof))->_encoder);
    bsp_abstractEncode_Task(_encoder);
}






int _bsp_step_l6470_App_Init()
{
	
    l6470_motor.cof = (bsp_step_motor_conf *)&l6470_cof;
    memcpy(l6470_motor.name,"l6470",sizeof(l6470_motor.name));
    bsp_step_motor_Init(&l6470_motor,&_l6470_ops);
    bsp_step_motor_Config(&l6470_motor,(bsp_step_motor_conf *)&l6470_cof);
	  return 0;
}

INIT_ENV_EXPORT(_bsp_step_l6470_App_Init);

