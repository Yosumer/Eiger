#include "bsp_adc128S102.h"
#include "drv_spi.h"
#include "drv_soft_spi.h"
#define LOG_TAG            "drv.adc128S"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>

static void __bsp_adc128S102_lower_read(struct adc128S102_spi_device * dev, uint8_t _ch,uint16_t *data)
{
    rt_err_t result;
    uint16_t sendcmd = _ch<<11;
    static struct rt_spi_message _sendmsg,_sendmsg2;
    result = rt_mutex_take(&dev->lock, RT_WAITING_FOREVER);
    if(result == RT_EOK)
    {
        //send read cmd
        _sendmsg.send_buf   = &sendcmd;
        _sendmsg.recv_buf   = RT_NULL;
        _sendmsg.length     = 1;
        _sendmsg.cs_take    = 1;
        _sendmsg.cs_release = 0;
        _sendmsg.next       = &_sendmsg2; //这里配置是为了选择通道
        

        _sendmsg2.send_buf   = RT_NULL;
        _sendmsg2.recv_buf   = data;
        _sendmsg2.length     = 1;
        _sendmsg2.cs_take    = 0;
        _sendmsg2.cs_release = 1;
        _sendmsg2.next       = RT_NULL;

        rt_spi_transfer_message(dev->_device, &_sendmsg);
    }
    rt_mutex_release(&dev->lock);
}


static void __bsp_adc128S102_lower_contiuteRead(struct adc128S102_spi_device * dev)
{
    rt_err_t result;
    static uint16_t sendch[8] = {0};
    static struct rt_spi_message _sendmsg[3];
    result = rt_mutex_take(&dev->lock, RT_WAITING_FOREVER);
    if (result == RT_EOK)
    {
        // if(dev->_currentCH == 8)
        dev->_currentCH = 0;
        sendch[0] = dev->_currentCH << 10;
        _sendmsg[0].send_buf = &sendch[0];
        _sendmsg[0].recv_buf   = RT_NULL;
        _sendmsg[0].length     = 1;
        _sendmsg[0].cs_take    = 1;
        _sendmsg[0].cs_release = 0;
        _sendmsg[0].next       = &_sendmsg[1]; //这里配置是为了选择通道
        dev->_currentCH++;
        for(uint8_t i = 1 ; i <8 ; i++) {
            sendch[i] = dev->_currentCH << 10;
            dev->_currentCH++;
        }
        _sendmsg[1].send_buf = &sendch[1];
        _sendmsg[1].recv_buf   = dev->_rawdata;
        _sendmsg[1].length     = 7;
        _sendmsg[1].cs_take    = 1;
        _sendmsg[1].cs_release = 0;
        _sendmsg[1].next       = &_sendmsg[2]; //这里配置是为了选择通道


        _sendmsg[2].send_buf = RT_NULL;
        _sendmsg[2].recv_buf   = &dev->_rawdata[7];
        _sendmsg[2].length     = 1;
        _sendmsg[2].cs_take    = 0;
        _sendmsg[2].cs_release = 1;
        _sendmsg[2].next       = RT_NULL;


        rt_spi_transfer_message(dev->_device, &_sendmsg[0]);
    }
    rt_mutex_release(&dev->lock);

}


static void __bsp_adcS102_cs_ctl(void *_dev ,uint8_t _en)

{
    RT_ASSERT(_dev);
    struct adc128S102_spi_device * dev = (struct adc128S102_spi_device *)_dev;
    HAL_GPIO_WritePin((GPIO_TypeDef *)dev->cs_port, dev->cs_pin, _en==1?GPIO_PIN_SET:GPIO_PIN_RESET);
}
struct adc128S102_spi_device * bsp_adc128S102_create(const char *spi_bus_name,const char *spi_dev_name,GPIO_TypeDef *cs_gpiox, uint16_t cs_gpio_pin)
{
    RT_ASSERT(spi_bus_name);
    RT_ASSERT(spi_dev_name);

    //

    struct adc128S102_spi_device * _adc128s_device = (struct adc128S102_spi_device *)rt_calloc(1,sizeof(struct adc128S102_spi_device)); 
    if (_adc128s_device == RT_NULL)
    {
        LOG_E("CAN NOT allocate adc128S102_spi_device");
        return RT_NULL;
    }
    rt_memset(_adc128s_device, 0, sizeof(struct adc128S102_spi_device));
    //

    //挂载设备到总线上
    rt_err_t res = rt_hw_spi_device_attach(spi_bus_name, spi_dev_name, cs_gpiox, cs_gpio_pin);
    if (res != RT_EOK)
    {
        LOG_E("rt_spi_bus_attach_device!");
        return RT_NULL;
    }
    
    _adc128s_device->cs_port = cs_gpiox;
    _adc128s_device->cs_pin = cs_gpio_pin;
    _adc128s_device->cs_ctl = __bsp_adcS102_cs_ctl;
    _adc128s_device->cs_ctl(_adc128s_device,1); //默认拉高CS

    _adc128s_device->_device = (struct rt_spi_device*)rt_device_find(spi_dev_name);  //寻找设备
    if (_adc128s_device->_device == RT_NULL)
    {
        LOG_E("spi device %s not found!\r\n", spi_dev_name);
        return RT_NULL;
    }
    // config spi
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 16;
        cfg.mode = RT_SPI_MASTER |RT_SPI_MODE_3 | RT_SPI_MSB;
        cfg.max_hz = 2 * 1000 *1000;
        res = rt_spi_configure(_adc128s_device->_device, &cfg);
        if (res!= RT_EOK)
        {
            LOG_E("configure spi device %s failed! res = %d\r\n", spi_dev_name, res);
            return RT_NULL;
        }
    }
    rt_mutex_init(&_adc128s_device->lock, "adc128_mut", RT_IPC_FLAG_FIFO);
    return _adc128s_device;
    
}
uint16_t bsp_adc128S102_immediatelyReadCh(struct adc128S102_spi_device *dev,uint8_t _ch)   //立即读取
{
    uint16_t data;
    dev->cs_ctl(dev,1); //拉高CS 停止当前的转换
    __bsp_adc128S102_lower_read(dev, _ch, &data);
    dev->_currentCH = _ch;
    dev->_rawdata[_ch] = data;
    return data;
}
uint16_t bsp_adc128S102_ReadCh(struct adc128S102_spi_device *dev,uint8_t _ch)              //从缓存中读取
{
    RT_ASSERT(dev);
    return dev->_rawdata[_ch];
} 
uint16_t* bsp_adc128S102_ReadAllCh(struct adc128S102_spi_device *dev)
{
    RT_ASSERT(dev);
    return (uint16_t *)dev->_rawdata;
}

struct adc128S102_spi_device *adc128s_device =RT_NULL;

void  bsp_adc128S102_task()
{
    // __bsp_adc128S102_lower_contiuteRead(adc128s_device);
    bsp_adc128S102_immediatelyReadCh(adc128s_device,adc128s_device->_currentCH);
    adc128s_device->_currentCH++;
    if (adc128s_device->_currentCH==8)
    {
        adc128s_device->_currentCH = 0;
    }
    
}


int  bsp_adc128S102_init()
{
    adc128s_device = bsp_adc128S102_create(ADC128S_SPI, "adc128",GPIOA,GPIO_PIN_15);
    if(adc128s_device == RT_NULL) {
        LOG_E("adc128S102 create failed");
        return -1;
    }
    adc128s_device->_currentCH = 0;
    return 0;

}
// INIT_BOARD_EXPORT(bsp_adc128S102_init);