/*------------------------------------------------------------------------------
 * @file    BSP_L6470_MOTOR.C
 * @author  FIRMWARE TEAM 
 * @date    2024/05/29 17:55:55
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

/* Files includes  -----------------------------------------------------------*/
#include "bsp_l6470_motor.h"
#include "drv_spi.h"
#include "drv_gpio.h"
#include <rtdevice.h>
#define LOG_TAG            "drv.l6470"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>

#define PIN_PORT(pin) ((uint8_t)(((pin) >> 4) & 0xFu))
#define PIN_NO(pin) ((uint8_t)((pin) & 0xFu))
#define PIN_STPORT(pin) ((GPIO_TypeDef *)(GPIOA_BASE + (0x400u * PIN_PORT(pin))))
#define PIN_STPIN(pin) ((uint16_t)(1u << PIN_NO(pin)))
#ifdef BSP_USING_L6470

uint8_t bsp_l6470_motor_gethardError(void *handler);
uint8_t bsp_l6470_motor_gethardbusy(void *handler);
void spi_write_read(void* handler, uint8_t *tx_buf,uint8_t tx_len, uint8_t *rx_buf,uint8_t rx_len);
static _spiHandler  __spihander= {
    .handler = NULL,
    .get_busy = bsp_l6470_motor_gethardbusy,
    .get_flag = bsp_l6470_motor_gethardError,
    .spi_write_read = spi_write_read
};

static  rt_mutex_t _lock=RT_NULL;
/*----------------------------------------------------------------------------*/
static MotorParameterData_t _motorParms = {
    .motorvoltage = 24.0,
    .fullstepsperrevolution = 200,
    .phasecurrent = 2.5,
    .phasevoltage = 5.0,
    .speed = 1000.0,
    .acc = 1200.0,
    .dec = 1200.0,
    .maxspeed = 800.0,
    .minspeed = 0.0,
    .fsspd = 15.244,
    .kvalhold = 3.13,
    .kvalrun = 3.13,
    .kvalacc = 3.13,
    .kvaldec = 3.13,
    .intspeed = 644.326,
    .stslp = 0.01068,
    .fnslpacc = 0.01526,
    .fnslpdec = 0.01526,
    .kterm = 1,
    .ocdth = 1 * 375 * 1.00,
    .stallth = 31.25 * 1.00,
    .step_sel = MICROSTEP_1_128,
    // .alarmen = L6470_OVERCURRENT|L6470_THERMAL_WARNING|L6470_WRONG_OR_NON_PERFORMABLE_COMMAND,
    .alarmen = L6470_THERMAL_WARNING|L6470_WRONG_OR_NON_PERFORMABLE_COMMAND,
    .config = 0x001B,
};


/**
 * @description: 底层SPI读写函数
 * @param {motor_l6470 *} _me
 * @return {*}
 */
static void spi_write_read(void* handler, uint8_t *tx_buf,uint8_t tx_len, uint8_t *rx_buf,uint8_t rx_len)
{
    struct motor_l6470_device *_dev = (struct motor_l6470_device *)handler;
     rt_err_t result;
     result = rt_mutex_take(_lock, RT_WAITING_FOREVER);
     if(result == RT_EOK)
     {
        for(uint8_t i=0;i<tx_len;i++)
        {
            struct rt_spi_message _sendmsg;
            _sendmsg.send_buf   = tx_buf+i;
            _sendmsg.recv_buf   = rx_buf+i;
            _sendmsg.length     = 1;
            _sendmsg.cs_take    = 1;
            _sendmsg.cs_release = 1;
            _sendmsg.next       = RT_NULL;
            rt_spi_transfer_message(_dev->_device, &_sendmsg);
        }
     }
     rt_mutex_release(_lock);
}
/**
 * @description: 向底层注册spi操作句柄
 * @param {motor_l6470 *} _me
 * @return {*}
 */
static void bsp_l6470_registerHandler(struct motor_l6470 * _me)
{
    __spihander.handler = &_me->dev;
    L6470_RegisterSpiHandler(&__spihander);
}
/**
 * @description: 创建一个L6470电机对象
 * @param {char} *spi_bus_name      spi 总线名字
 * @param {char} *spi_dev_name      spi 设备名字
 * @param {rt_base_t} cs_pin        cs 引脚使用pin 框架
 * @param {rt_base_t} flag_pin      flag 引脚使用pin 框架
 * @param {rt_base_t} reset_pin     reset 引脚使用pin 框架
 * @param {rt_base_t} busy_pin      busy 引脚使用pin 框架
 * @return {*}
 */
struct motor_l6470 * bsp_l6470_motor_create(const char *spi_bus_name,const char *spi_dev_name,rt_base_t cs_pin,rt_base_t flag_pin,rt_base_t reset_pin,rt_base_t busy_pin)
{
    struct motor_l6470 * _dev;
    RT_ASSERT(spi_bus_name);
    RT_ASSERT(spi_dev_name);

    _dev = rt_calloc(1,sizeof(struct motor_l6470));
    if(NULL == _dev)
        LOG_E("Create L6470 memory Faild!");
    /* attach the device to spi bus*/
    struct rt_spi_device *spi_device;
    spi_device = (struct rt_spi_device *)rt_malloc(sizeof(struct rt_spi_device));
    RT_ASSERT(spi_device != RT_NULL);

    GPIO_TypeDef *cs_port = (GPIO_TypeDef *)PIN_STPORT(cs_pin);
    uint16_t _pin = PIN_STPIN(cs_pin);
    rt_err_t result = rt_hw_spi_device_attach(spi_bus_name, spi_dev_name, cs_port, _pin);
    if (result != RT_EOK)
    {
        LOG_E("%s attach to %s faild, %d\n", spi_dev_name, spi_bus_name, result);
    }
    _dev->dev._device = (struct rt_spi_device*)rt_device_find(spi_dev_name);
    if (_dev->dev._device== RT_NULL)
    {
        LOG_E("spi device %s not found!\r\n", spi_dev_name);
        return RT_NULL;
    }
    struct rt_spi_configuration cfg;
    cfg.data_width = 8;
    cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_3 | RT_SPI_MSB;
    cfg.max_hz = 4 * 1000 * 1000; /* 5M */

    rt_spi_configure(_dev->dev._device, &cfg);

    // 初始化互斥量
    if (_lock == RT_NULL)   //第一次初始化
    {
        _lock = rt_mutex_create("l6474_mut",RT_IPC_FLAG_FIFO);
    }
    
    //配置外部IO
    _dev->dev.busy_gpio_pin = busy_pin;
    _dev->dev.reset_gpio_pin = reset_pin;
    _dev->dev.flag_gpio_pin = flag_pin;
    _dev->dev.cs_gpio_pin = cs_pin;



    rt_pin_mode(reset_pin, PIN_MODE_OUTPUT);
    //复位芯片
    bsp_l6470_motor_resetDev(_dev);
    rt_pin_mode(busy_pin, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(flag_pin, PIN_MODE_INPUT_PULLUP);

    //配置默认电机参数
    _dev->motorparameter = &_motorParms;
    _dev->handler = (StepperMotorDriverHandle_t *)rt_calloc(1, sizeof(StepperMotorDriverHandle_t));
    extern StepperMotorCommand_t L6470Command;
    _dev->handler->Command = &L6470Command;
    _dev->handler->DaisyChainPosition = 0;
    //配置电机默认参数
    bsp_l6470_motor_config(_dev,_dev->motorparameter);
    return _dev;

}
/**
 * @description: 配置L6470全部电机参数
 * @param {motor_l6470 *} _me
 * @param {MotorParameterData_t} *_motorParam
 * @return {*}
 */
void bsp_l6470_motor_config(struct motor_l6470 * _me,MotorParameterData_t *_motorParam)
{
    RT_ASSERT(_me);
    RT_ASSERT(_lock);
    rt_err_t result;
    result = rt_mutex_take(_lock, RT_WAITING_FOREVER);
    // 注册底层SPI传输处理句柄
    if(result == RT_EOK)
    {
        bsp_l6470_registerHandler(_me);
        L6470_Config(_me->handler,_motorParam);
    }
    rt_mutex_release(_lock);
        
}
/**
 * @description: 设置L6470驱动速度、加速度、减速度，该配置在使用GOTO、Move时有用。
 * @param {motor_l6470 *} _me
 * @param {float} _speed    
 * @param {float} _acc
 * @param {float} _dec
 * @return {*}
 */
void bsp_l6470_motor_setRunData(struct motor_l6470 * _me,float _speed,float _acc,float _dec)
{
    RT_ASSERT(_me);
    RT_ASSERT(_lock);
    rt_err_t result;
    result = rt_mutex_take(_lock, RT_WAITING_FOREVER);
    // 注册底层SPI传输处理句柄
    if(result == RT_EOK)
    {
        // 注册底层SPI传输处理句柄
        bsp_l6470_registerHandler(_me);
        _me->info.acc = _acc;
        _me->info.dec = _dec;
        _me->info.speed = _speed;
        _me->handler->Command->SetParam(_me->handler->DaisyChainPosition,L6470_ACC_ID,Step_s2_2_Acc(_acc));
        _me->handler->Command->SetParam(_me->handler->DaisyChainPosition,L6470_DEC_ID,Step_s2_2_Dec(_dec));
        _me->handler->Command->SetParam(_me->handler->DaisyChainPosition,L6470_DEC_ID,Step_s_2_MaxSpeed(_speed));
    }
    rt_mutex_release(_lock);

}
/**
 * @description: 设置电机以设定速度运动
 * @param {motor_l6470 *} _me
 * @param {motor_l6470_dir} _dir  CW:Reverse direction、CWW:Forward direction
 * @param {float} _speed  step/s
 * @return {*}
 */
void bsp_l6470_motor_run(struct motor_l6470 * _me,motor_l6470_dir _dir,float _speed)
{
    RT_ASSERT(_me);
    RT_ASSERT(_lock);
    rt_err_t result;
    result = rt_mutex_take(_lock, RT_WAITING_FOREVER);
    // 注册底层SPI传输处理句柄
    if(result == RT_EOK)
    {
        _me->info.dir = _dir;
        // 注册底层SPI传输处理句柄
        bsp_l6470_registerHandler(_me);
        _me->handler->Command->Run(_me->handler->DaisyChainPosition,(eL6470_DirId_t)_dir,Step_s_2_Speed(_speed));
    }
    rt_mutex_release(_lock);
}
/**
 * @description: 设置电机以设置方向运行固定步数
 * @param {motor_l6470 *} _me
 * @param {motor_l6470_dir} _dir    CW:Reverse direction、CWW:Forward direction
 * @param {uint32_t} step
 * @return {*}
 */
void bsp_l6470_motor_move(struct motor_l6470 * _me,motor_l6470_dir _dir,uint32_t step)
{
    RT_ASSERT(_me);
    RT_ASSERT(_lock);
    rt_err_t result;
    result = rt_mutex_take(_lock, RT_WAITING_FOREVER);
    
    // 注册底层SPI传输处理句柄
    if(result == RT_EOK)
    {
        _me->info.dir = _dir;    
        // 注册底层SPI传输处理句柄
        bsp_l6470_registerHandler(_me);
        _me->handler->Command->Move(_me->handler->DaisyChainPosition,(eL6470_DirId_t)_dir,step);
    }
    rt_mutex_release(_lock);
}
/**
 * @description: 设置电机运行到绝对位置
 * @param {motor_l6470 *} _me
 * @param {uint32_t} _abspos
 * @return {*}
 */
void bsp_l6470_motor_goto(struct motor_l6470 * _me,uint32_t _abspos)
{
    RT_ASSERT(_me);
    RT_ASSERT(_lock);
    rt_err_t result;
    result = rt_mutex_take(_lock, RT_WAITING_FOREVER);
    
    // 注册底层SPI传输处理句柄
    if(result == RT_EOK)
    {
        _me->info.setcoor = _abspos;      
        // 注册底层SPI传输处理句柄
        bsp_l6470_registerHandler(_me);
        _me->handler->Command->GoTo(_me->handler->DaisyChainPosition,_abspos);
    }
    rt_mutex_release(_lock);
}
/**
 * @description: 电机软停止、不保持力矩
 * @param {motor_l6470 *} _me
 * @return {*}
 */
void bsp_l6470_motor_softHiZ(struct motor_l6470 * _me)
{
    RT_ASSERT(_me);
    RT_ASSERT(_lock);
    rt_err_t result;
    result = rt_mutex_take(_lock, RT_WAITING_FOREVER);
    
    // 注册底层SPI传输处理句柄
    if(result == RT_EOK)
    { 
        // 注册底层SPI传输处理句柄
        bsp_l6470_registerHandler(_me);
        _me->handler->Command->SoftHiZ(_me->handler->DaisyChainPosition);
    }
    rt_mutex_release(_lock);
}
/**
 * @description: 电机软停止、保持力矩
 * @param {motor_l6470 *} _me
 * @return {*}
 */
void bsp_l6470_motor_softStop(struct motor_l6470 * _me)
{
    RT_ASSERT(_me);
    RT_ASSERT(_lock);
    rt_err_t result;
    result = rt_mutex_take(_lock, RT_WAITING_FOREVER);
    
    // 注册底层SPI传输处理句柄
    if(result == RT_EOK)
    { 
        // 注册底层SPI传输处理句柄
        bsp_l6470_registerHandler(_me);
        _me->handler->Command->SoftStop(_me->handler->DaisyChainPosition);
    }
    rt_mutex_release(_lock);
}

/**
 * @description: 电机硬停止、不保持力矩
 * @param {motor_l6470 *} _me
 * @return {*}
 */
void bsp_l6470_motor_hardHiZ(struct motor_l6470 * _me)
{
    RT_ASSERT(_me);
    RT_ASSERT(_lock);
    rt_err_t result;
    result = rt_mutex_take(_lock, RT_WAITING_FOREVER);
    
    // 注册底层SPI传输处理句柄
    if(result == RT_EOK)
    { 
        // 注册底层SPI传输处理句柄
        bsp_l6470_registerHandler(_me);
        _me->handler->Command->HardHiZ(_me->handler->DaisyChainPosition);
    }
    rt_mutex_release(_lock);
}
/**
 * @description: 电机硬停止、保持力矩
 * @param {motor_l6470 *} _me
 * @return {*}
 */
void bsp_l6470_motor_hardStop(struct motor_l6470 * _me)
{
    RT_ASSERT(_me);
    RT_ASSERT(_lock);
    rt_err_t result;
    result = rt_mutex_take(_lock, RT_WAITING_FOREVER);
    
    // 注册底层SPI传输处理句柄
    if(result == RT_EOK)
    { 
        // 注册底层SPI传输处理句柄
        bsp_l6470_registerHandler(_me);
        _me->handler->Command->SoftStop(_me->handler->DaisyChainPosition);
    }
    rt_mutex_release(_lock);
}
/**
 * @description: 电机复位绝对位置寄存器
 * @param {motor_l6470 *} _me
 * @return {*}
 */
void bsp_l6470_motor_resetPos(struct motor_l6470 * _me)
{
    RT_ASSERT(_me);
    RT_ASSERT(_lock);
    rt_err_t result;
    result = rt_mutex_take(_lock, RT_WAITING_FOREVER);
    
    // 注册底层SPI传输处理句柄
    if(result == RT_EOK)
    { 
        // 注册底层SPI传输处理句柄
        bsp_l6470_registerHandler(_me);
        _me->handler->Command->ResetPos(_me->handler->DaisyChainPosition);
    }
    rt_mutex_release(_lock);
}
/**
 * @description: 复位L6470芯片
 * @param {motor_l6470 *} _me
 * @return {*}
 */
void bsp_l6470_motor_resetDev(struct motor_l6470 * _me)
{
    if(NULL == _me)
    {
        LOG_E("motor_l6470 dev is NULL");
    }
    rt_pin_write(_me->dev.reset_gpio_pin, PIN_LOW);
    rt_thread_delay(500);
    rt_pin_write(_me->dev.reset_gpio_pin, PIN_HIGH);
}

/**
 * @description: 
 * @return one if there is a busy chip, otherwise zero
 */
uint8_t bsp_l6470_motor_gethardbusy(void *handler)
{
    struct motor_l6470_device * _me = (struct motor_l6470_device *)handler;
    return (!rt_pin_read(_me->busy_gpio_pin));
}
/**
 * @description: 
 * @return one if there is a error chip, otherwise zero
 */
uint8_t bsp_l6470_motor_gethardError(void *handler) 
{
    struct motor_l6470_device * _me = (struct motor_l6470_device *)handler;
    return (!rt_pin_read(_me->flag_gpio_pin));
}
/**
 * @description: 获取电机运行状态
 * @param {motor_l6470 *} _me
 * @return {*}
 */
motor_l6470_state bsp_l6470_motor_getState(struct motor_l6470 * _me)
{
    RT_ASSERT(_me);
    RT_ASSERT(_lock);
    rt_err_t result;
    motor_l6470_state _state;
    result = rt_mutex_take(_lock, RT_WAITING_FOREVER);
    // 注册底层SPI传输处理句柄
    if(result == RT_EOK)
    { 
        // 注册底层SPI传输处理句柄
        bsp_l6470_registerHandler(_me);
        if (_me->handler->Command->GetErrorFlag()) {
            _state = MOTOR_ERROR;
            _me->info.statusRe= (_me->handler->Command->GetStatus(_me->handler->DaisyChainPosition));
        } 
        else 
        {
            if (_me->handler->Command->GetBusy())
            {
                _state = (motor_l6470_state) (_me->handler->Command->CheckStatusRegisterFlag(_me->handler->DaisyChainPosition,MOT_STATUS_ID));
            }
            else
                _state = MOTOR_IDLE;
        }
    }
    rt_mutex_release(_lock);
    return _state;
}
/**
 * @description: 获取电机绝对位置
 * @param {motor_l6470 *} _me
 * @return {*}
 */
rt_int32_t  bsp_l6470_motor_getCoor(struct motor_l6470 * _me)
{
    RT_ASSERT(_me);
    RT_ASSERT(_lock);
    rt_err_t result;
    rt_int32_t _coor;
    result = rt_mutex_take(_lock, RT_WAITING_FOREVER);
    
    // 注册底层SPI传输处理句柄
    if(result == RT_EOK)
    { 
        // 注册底层SPI传输处理句柄
        bsp_l6470_registerHandler(_me);
        _coor = _me->handler->Command->GetParam(_me->handler->DaisyChainPosition,L6470_ABS_POS_ID);
    }
    rt_mutex_release(_lock);

    return _coor;
}

void  bsp_l6470_motor_setPos(struct motor_l6470 * _me, int pos)
{
    RT_ASSERT(_me);
    RT_ASSERT(_lock);
    rt_err_t result;
    rt_int32_t _coor;
    result = rt_mutex_take(_lock, RT_WAITING_FOREVER);
    
    // 注册底层SPI传输处理句柄
    if(result == RT_EOK)
    { 
        // 注册底层SPI传输处理句柄
        bsp_l6470_registerHandler(_me);
        _me->handler->Command->SetParam(_me->handler->DaisyChainPosition,L6470_ABS_POS_ID,pos);
    }
    rt_mutex_release(_lock);

}
/**
 * @description: 获取电机当前的运行速度
 * @param {motor_l6470 *} _me
 * @return {*}
 */
float bsp_l6470_motor_getSpeed(struct motor_l6470 * _me)
{
    RT_ASSERT(_me);
    RT_ASSERT(_lock);
    rt_err_t result;
    float _speed;
    result = rt_mutex_take(_lock, RT_WAITING_FOREVER);
    // 注册底层SPI传输处理句柄
    if(result == RT_EOK)
    { 
        // 注册底层SPI传输处理句柄
        bsp_l6470_registerHandler(_me);
        _speed = Speed_2_Step_s(_me->handler->Command->GetParam(_me->handler->DaisyChainPosition,L6470_SPEED_ID));
    }
    rt_mutex_release(_lock);
    return _speed;
}
/**
 * @description: 获取电机运行方向
 * @param {motor_l6470 *} _me
 * @return   CW:Reverse direction、CWW:Forward direction
 */
motor_l6470_dir bsp_l6470_motor_getDir(struct motor_l6470 * _me)
{
    RT_ASSERT(_me);
    RT_ASSERT(_lock);
    rt_err_t result;
    motor_l6470_dir _dir;
    result = rt_mutex_take(_lock, RT_WAITING_FOREVER);
    // 注册底层SPI传输处理句柄
    if(result == RT_EOK)
    { 
        // 注册底层SPI传输处理句柄
        bsp_l6470_registerHandler(_me);
        _dir = (motor_l6470_dir)_me->handler->Command->CheckStatusRegisterFlag(_me->handler->DaisyChainPosition,DIR_ID);
    }
    rt_mutex_release(_lock);

    return _dir;
}
#endif
