#include <board.h>
#include <rtdevice.h>

#define  BSP_USING_MCP4725
#define  RT_USING_I2C


#if (defined(BSP_USING_MCP4725) && defined(RT_USING_I2C))

#include "drv_dac_mcp4725.h"
#define LOG_TAG            "drv.mcp4725"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>


 
struct drv_dac_mcp4725 _mcp4725_dac;


static rt_err_t _mcp4725_write_adc(struct rt_i2c_bus_device *bus, rt_uint8_t reg, rt_uint16_t _adc)
{
    rt_uint8_t buf[2];
    struct rt_i2c_msg msgs;
    rt_uint32_t buf_size = 2;

    buf[0] = (reg&0xf0)|((0x0f00&_adc)>>8); //cmd
    buf[1] = (0x00ff&_adc);


    msgs.addr = MCP4725_ADDRESS;
    msgs.flags = RT_I2C_WR;
    msgs.buf = buf;
    msgs.len = buf_size;

    /* 调用I2C设备接口传输数据 */
    if (rt_i2c_transfer(bus, &msgs, 1) == 1)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}


static rt_err_t mcp4725_dac_disabled(struct rt_dac_device *device, rt_uint32_t channel)
{
    (void) channel;
    RT_ASSERT(device != RT_NULL);
    struct rt_i2c_bus_device  *dev = device->parent.user_data;
    return _mcp4725_write_adc(dev,MCP4725_CMD_WRITEDAC,0);

}

static rt_err_t mcp4725_dac_enabled(struct rt_dac_device *device, rt_uint32_t channel)
{
    (void) device;
    return RT_EOK;
}


static rt_err_t mcp4725_set_dac_value(struct rt_dac_device *device, rt_uint32_t channel, rt_uint32_t *value)
{
    (void) channel;
    RT_ASSERT(device != RT_NULL);
    struct rt_i2c_bus_device  *dev = device->parent.user_data;
    return _mcp4725_write_adc(dev,MCP4725_CMD_FASTMODE,(uint16_t)(*value));

}

static rt_uint8_t mcp4725_dac_get_resolution(struct rt_dac_device *device)
{
    RT_ASSERT(device != RT_NULL);
    struct rt_i2c_bus_device  *dev;
    dev = device->parent.user_data;
    (void) dev;
    return 12;
}

static const struct rt_dac_ops _dac_ops =
{
    .disabled = mcp4725_dac_disabled,
    .enabled  = mcp4725_dac_enabled,
    .convert  = mcp4725_set_dac_value,
    .get_resolution = mcp4725_dac_get_resolution,
};


static int mcp4725_dac_init(void)
{
    /* 查找I2C总线设备，获取I2C总线设备句柄 */
    _mcp4725_dac.bus = (struct rt_i2c_bus_device *)rt_device_find(MCP4725_IIC_NAME);

    if (_mcp4725_dac.bus == RT_NULL)
    {
        rt_kprintf("can't find %s device!\n", MCP4725_IIC_NAME);
        return -RT_ERROR;
    }
    else
    {
        _mcp4725_write_adc(_mcp4725_dac.bus,MCP4725_CMD_WRITEDAC,0);    //上电DAC默认输出0
    
        /* register dac device */
        if (rt_hw_dac_register(&_mcp4725_dac.stm32_dac_device, MCP4725_DAC_NAME, &_dac_ops, _mcp4725_dac.bus) == RT_EOK)
        {
            LOG_D("%s init success", MCP4725_DAC_NAME);
        }
        else
        {
            LOG_E("%s register failed", MCP4725_DAC_NAME);
            return  -RT_ERROR;
        }

    }
    return RT_EOK;
}
INIT_DEVICE_EXPORT(mcp4725_dac_init);


#endif