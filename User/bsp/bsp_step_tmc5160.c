#include "rtthread.h"
#include "rtdevice.h"
#include "bsp_step_tmc5160.h"
#include "tmc5160.h"
#define LOG_TAG            "drv.tmc5610"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>
#define DEFAULT_TMC_MOTOR_INDEX     0
// #define USE_BREAK_PIN_ENABLE     

#ifdef USE_BREAK_PIN_ENABLE

    #define   BREAK_ENA_PIN_NAME      "PG.2"
    rt_base_t _break_pin;
    #define TMC5160_CTL_BREAK(X) rt_pin_write(_break_pin,X==0?PIN_LOW:PIN_HIGH);     //使能芯片电源

#endif

bsp_step_motor_t  tmc5160_motor;
bsp_step_tmc_conf tmc5160_cof = {
    ._base = {
        ._spr = 51200,
        ._espr = 51200,
        // ._dmax = 10000,
        // ._amax = 10000,
        ._dmax = 50000,
        ._amax = 50000,
        // ._vmax = 268435,
//        ._vmax = 400000,	
			._vmax = 200000,	
    },
    // .a1 = 0,
    // .d1 = 10000,
    .a1 = 1000,
    .d1 = 10000,
    .rampmode = TMC5160_MODE_VELPOS,
    .vstart = 0,
    .vstop = 10,
    .v1 = 0,
};




static void _bsp_step_tmc5160_Config(void *_me, bsp_step_motor_conf *_cof);
static uint8_t _bsp_step_tmc5160_Init(void *_me);
static void _bsp_step_tmc5160_Rotate(void *_me,int32_t velocity);
static void _bsp_step_tmc5160_Stop(void *_me);
static void _bsp_step_tmc5160_MoveTo(void *_me,int32_t position);
static void _bsp_step_tmc5160_MoveBy(void *_me,int32_t step);
static void _bsp_step_tmc5160_SetPos(void *_me,int32_t _pos);
static uint32_t _bsp_step_tmc5160_Getstatus(void *_me);
static int32_t _bsp_step_tmc5160_getvelocity(void *_me);
static int32_t _bsp_step_tmc5160_getPos(void *_me);
static int32_t _bsp_step_tmc5160_getEncoder(void *_me);
static void _bsp_step_tmc5160_task(void *_this);


bsp_step_motor_ops  _tmc_ops = {
    .moveBy = _bsp_step_tmc5160_MoveBy,
    .config = _bsp_step_tmc5160_Config,
    .get_encode = _bsp_step_tmc5160_getEncoder,
    .get_position = _bsp_step_tmc5160_getPos,
    .get_status = _bsp_step_tmc5160_Getstatus,
    .get_velocity = _bsp_step_tmc5160_getvelocity,
    .moveTo = _bsp_step_tmc5160_MoveTo,
    .rotate = _bsp_step_tmc5160_Rotate,
    .set_position = _bsp_step_tmc5160_SetPos,
    .stop = _bsp_step_tmc5160_Stop,
    .task = _bsp_step_tmc5160_task,
    .init = _bsp_step_tmc5160_Init
};


void _bsp_step_tmc5160_App_Task(void *p)
{
    while (1)
    {

        bsp_step_motor_Task(&tmc5160_motor);
        rt_thread_delay(100);
    }
    
}


int _bsp_step_tmc5160_App_Init()
{
	memcpy(tmc5160_motor.name,"tmc5160",sizeof(tmc5160_motor.name));
    
    
    bsp_step_motor_Init(&tmc5160_motor,&_tmc_ops);
    bsp_step_motor_Config(&tmc5160_motor,(bsp_step_motor_conf *)&tmc5160_cof);
    // rt_thread_t _tmc =rt_thread_create("_tmc_task",_bsp_step_tmc5160_App_Task,RT_NULL,1024,28,10);
    // if(RT_NULL ==_tmc)
    //     LOG_E("Create _tmc_task faild");
    // rt_thread_startup(_tmc);
}

INIT_ENV_EXPORT(_bsp_step_tmc5160_App_Init);



// 配置位置模式正反都能转的话需要配置 
// 1 D1寄存器与减速寄存器一样 
// 2 STOP 需要大需要1


// 配置编码器需要下面步骤
// 1 配置寄存器0X56  1024   配置为十进制模式
// 2 配置寄存器0x58  为 794432 计算逻辑 256*200/编码器线数  （256*200/5000）*10000 = 102400 



static uint8_t _bsp_step_tmc5160_Init(void *_me)
{
 #ifdef USE_BREAK_PIN_ENABLE
    _break_pin=rt_pin_get(BREAK_ENA_PIN_NAME);
    rt_pin_mode(_break_pin,PIN_MODE_OUTPUT);
    
#endif

    if(TMC5160_init(DEFAULT_TMC_MOTOR_INDEX)) //TMC5160 初始化成功
    {
        // 配置编码器
        writeRegister(DEFAULT_TMC_MOTOR_INDEX,TMC5160_ENCMODE,1024);
        // writeRegister(DEFAULT_TMC_MOTOR_INDEX,TMC5160_ENC_CONST,794432);//1000线配置
        writeRegister(DEFAULT_TMC_MOTOR_INDEX,TMC5160_ENC_CONST,136672);//5000线配置
        // 657,760
        // _bsp_step_tmc5160_Config((bsp_step_motor_conf *)&tmc5160_cof);
        #ifdef USE_BREAK_PIN_ENABLE
                TMC5160_CTL_BREAK(1);
        #endif
        return 1;
    }
    else
    {
        LOG_E("TMC5160_init faild");
        return 0;
    }
}

static void _bsp_step_tmc5160_Config(void *_me,bsp_step_motor_conf *_cof)
{
    bsp_step_tmc_conf *_tmc_cof = (bsp_step_tmc_conf *)_cof;
    SAP(5,DEFAULT_TMC_MOTOR_INDEX,_tmc_cof->_base._amax);
    SAP(17,DEFAULT_TMC_MOTOR_INDEX,_tmc_cof->_base._dmax);
    SAP(15,DEFAULT_TMC_MOTOR_INDEX,_tmc_cof->a1);
    SAP(16,DEFAULT_TMC_MOTOR_INDEX,_tmc_cof->v1);
    SAP(18,DEFAULT_TMC_MOTOR_INDEX,_tmc_cof->d1);
    SAP(19,DEFAULT_TMC_MOTOR_INDEX,_tmc_cof->vstart);
    SAP(20,DEFAULT_TMC_MOTOR_INDEX,_tmc_cof->vstop);
}
static void _bsp_step_tmc5160_Rotate(void *_me,int32_t velocity)
{
    RT_UNUSED(_me);
    #ifdef USE_BREAK_PIN_ENABLE
        TMC5160_CTL_BREAK(1);
    #endif
    rotate(DEFAULT_TMC_MOTOR_INDEX,velocity);
}
static void _bsp_step_tmc5160_Stop(void *_me)
{
    RT_UNUSED(_me);
    stop(DEFAULT_TMC_MOTOR_INDEX);
    // #ifdef USE_BREAK_PIN_ENABLE
    //     TMC5160_CTL_BREAK(0);
    // #endif
}
static void _bsp_step_tmc5160_MoveTo(void *_me,int32_t position)
{
    RT_UNUSED(_me);
    // 设置速度
    SAP(4,DEFAULT_TMC_MOTOR_INDEX,tmc5160_cof._base._vmax);
    moveTo(DEFAULT_TMC_MOTOR_INDEX,position);
}
static void _bsp_step_tmc5160_MoveBy(void *_me,int32_t step)
{
    RT_UNUSED(_me);
    int32_t _step = step;
    // 设置速度
    SAP(4,DEFAULT_TMC_MOTOR_INDEX,tmc5160_cof._base._vmax);
    moveBy(DEFAULT_TMC_MOTOR_INDEX,&_step);
}
static void _bsp_step_tmc5160_SetPos(void *_me,int32_t _pos)
{
    int32_t _step = _pos;
    _bsp_step_tmc5160_Stop(_me);   //先停止
    SAP(1,DEFAULT_TMC_MOTOR_INDEX,_step);   //清空实际位置
    SAP(0,DEFAULT_TMC_MOTOR_INDEX,_step);   //清空目标位置
    SAP(209,DEFAULT_TMC_MOTOR_INDEX,_step);  //清空编码器值
}

static uint32_t _bsp_step_tmc5160_Getstatus(void *_me)
{   
    RT_UNUSED(_me);
    int32_t _status;
    readRegister(DEFAULT_TMC_MOTOR_INDEX, TMC5160_DRVSTATUS,  &_status);
    return _status;
}

static int32_t _bsp_step_tmc5160_getvelocity(void *_me)
{
    RT_UNUSED(_me);
    int32_t _v;
    getMeasuredSpeed(DEFAULT_TMC_MOTOR_INDEX,&_v);
    return _v;
}

static int32_t _bsp_step_tmc5160_getPos(void *_me)
{
    RT_UNUSED(_me);
    int32_t _pos;
    GAP(1,DEFAULT_TMC_MOTOR_INDEX,&_pos);
    return _pos;
}

static int32_t _bsp_step_tmc5160_getEncoder(void *_me)
{
    RT_UNUSED(_me);
    int32_t _encoder;
    (GAP(209,DEFAULT_TMC_MOTOR_INDEX,&_encoder));
    return _encoder;
}

static void _bsp_step_tmc5160_task(void *_this)
{
    UNUSED(_this);
    periodicJob(rt_tick_get_millisecond());
}
