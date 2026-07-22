#include "bsp_ad7730.h"
#define LOG_TAG            "drv.ad7730"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>

/* 位操作宏定义 */
#define BIT_SET(reg, bit)     ((reg) |= (1UL << (bit)))  // 设置指定位置为1
#define BIT_CLEAR(reg, bit)   ((reg) &= ~(1UL << (bit))) // 清除指定位置为0
#define BIT_READ(reg, bit)    (((reg) >> (bit)) & 1UL)   // 读取指定位置的值
#define BIT_TOGGLE(reg, bit)  ((reg) ^= (1UL << (bit)))  // 反转指定位置的值


// 定义AD7730相关的寄存器地址
#define AD7730_REG_COMM 0x00
#define AD7730_REG_STATUS 0x00 
#define AD7730_REG_MODE 0x02
#define AD7730_REG_FILTER 0x03
#define AD7730_REG_DATA 0x01
#define AD7730_REG_DAC 0x04
#define AD7730_REG_OFFSET 0x05
#define AD7730_REG_GAIN 0x06


#define SIGNAL_WRITE_CMD 0X00
#define SIGNAL_READ_CMD 0X01
#define CONTINUOUS_READ_CMD 0X02
#define CONTINUOUS_STOP_READ_CMD 0X03 


static void __bsp_ad7730_lower_write(struct ad7730_spi_device * dev, uint8_t reg, uint8_t *data , uint8_t len)
{

    rt_err_t result;
    uint8_t cmdcom = ((reg) &= ~(1UL << 7)); //设置要写的寄存器地址
    static struct rt_spi_message _sendcomm,_sendmsg;
    result = rt_mutex_take(dev->_lock, RT_WAITING_FOREVER);
    if(result == RT_EOK)
    {
        _sendcomm.send_buf   = &cmdcom;
        _sendcomm.recv_buf   = RT_NULL;
        _sendcomm.length     = 1;
        _sendcomm.cs_take    = 1;
        _sendcomm.cs_release = 0;
        _sendcomm.next       = &_sendmsg; //这里配置是为了选择通道

        _sendmsg.send_buf   = data;
        _sendmsg.recv_buf   = RT_NULL;
        _sendmsg.length     = len;
        _sendmsg.cs_take    = 0;
        _sendmsg.cs_release = 0;
        _sendmsg.next       = RT_NULL;
        rt_spi_transfer_message(dev->device, &_sendcomm);
    }
    rt_mutex_release(dev->_lock);
}

static void __bsp_ad7730_lower_write_data(struct ad7730_spi_device * dev, uint8_t *data, uint8_t len)
{
    rt_err_t result;
    static struct rt_spi_message _sendcomm;
    result = rt_mutex_take(dev->_lock, RT_WAITING_FOREVER);
    if(result == RT_EOK)
    {
        _sendcomm.send_buf   = data;
        _sendcomm.recv_buf   = RT_NULL;
        _sendcomm.length     = len;
        _sendcomm.cs_take    = 1;
        _sendcomm.cs_release = 0;
        _sendcomm.next       = RT_NULL; //这里配置是为了选择通道
        rt_spi_transfer_message(dev->device, &_sendcomm);
    }
    rt_mutex_release(dev->_lock);
}

static void __bsp_ad7730_lower_read_data(struct ad7730_spi_device * dev, uint8_t *data, uint8_t len)
{
    rt_err_t result;
    static struct rt_spi_message _sendcomm;
    result = rt_mutex_take(dev->_lock, RT_WAITING_FOREVER);
    if(result == RT_EOK)
    {
        _sendcomm.send_buf   = RT_NULL;
        _sendcomm.recv_buf   = data;
        _sendcomm.length     = len;
        _sendcomm.cs_take    = 1;
        _sendcomm.cs_release = 0;
        _sendcomm.next       = RT_NULL; //这里配置是为了选择通道
        rt_spi_transfer_message(dev->device, &_sendcomm);
    }
    rt_mutex_release(dev->_lock);
}

static void __bsp_ad7730_lower_read(struct ad7730_spi_device * dev,uint8_t _readmode, uint8_t reg, uint8_t *data, uint8_t len)
{
    rt_err_t result;
    uint8_t cmdcom = (((reg) &= ~(1UL << 7))|(_readmode<<4)); //设置要读的寄存器地址

    static struct rt_spi_message _sendcomm,_sendmsg;
    result = rt_mutex_take(dev->_lock, RT_WAITING_FOREVER);
    if(result == RT_EOK)
    {
        _sendcomm.send_buf   = &cmdcom;
        _sendcomm.recv_buf   = RT_NULL;
        _sendcomm.length     = 1;
        _sendcomm.cs_take    = 1;
        _sendcomm.cs_release = len>0?0:1;
        _sendcomm.next       = len>0?&_sendmsg:RT_NULL; //这里配置是为了选择通道

        _sendmsg.send_buf   = RT_NULL;
        _sendmsg.recv_buf   = data;
        _sendmsg.length     = len;
        _sendmsg.cs_take    = 0;
        _sendmsg.cs_release = 1;
        _sendmsg.next       = RT_NULL;
        rt_spi_transfer_message(dev->device, &_sendcomm);
    }
    rt_mutex_release(dev->_lock);
}

struct ad7730_spi_device * bsp_ad7730_create(const char *spi_bus_name,const char *spi_dev_name,uint8_t (*is_ready)(),GPIO_TypeDef *cs_gpiox, uint16_t cs_gpio_pin)
{
    RT_ASSERT(spi_bus_name);
    RT_ASSERT(spi_dev_name);
    RT_ASSERT(is_ready);
    // 创建spi设备
    struct ad7730_spi_device *dev = (struct ad7730_spi_device *)rt_malloc(sizeof(struct ad7730_spi_device));
    if (dev == RT_NULL)
    {
        rt_kprintf("malloc ad7730 device failed!\n");
        return RT_NULL;
    }
    rt_memset(dev, 0, sizeof(struct ad7730_spi_device));
    //挂载设备到总线上
    rt_err_t result = rt_hw_spi_device_attach(spi_bus_name, spi_dev_name, cs_gpiox, cs_gpio_pin);
    if (result != RT_EOK)
    {
        LOG_E("rt_spi_bus_attach_device!");
        return RT_NULL;
    }
    //寻找spi设置
    dev->device = (struct rt_spi_device *)rt_device_find(spi_dev_name);
    if (dev->device == RT_NULL)
    {
        LOG_E("spi device %s not found!\r\n", spi_dev_name);
        return RT_NULL;
    }
    // 配置SPI
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = RT_SPI_MASTER |RT_SPI_MODE_1 | RT_SPI_MSB;   //AD7730使用SPI模式1，MSB模式
        cfg.max_hz = 2 * 1000 *1000;
        result = rt_spi_configure(dev->device, &cfg);
        if (result!= RT_EOK)
        {
            LOG_E("configure spi device %s failed! res = %d\r\n", spi_dev_name, result);
            return RT_NULL;
        }
        
    }
    //初始化锁
    dev->_lock = rt_mutex_create("ad7730_lock",RT_IPC_FLAG_FIFO);
    // rt_mutex_init(&dev->_lock,"ad7730_lock",RT_IPC_FLAG_FIFO);
    dev->_get_ready = is_ready;
    return dev;
}
// 向AD7730写入数据
void bsp_ad7730_set_mode(struct ad7730_spi_device *_dev,uint16_t _mode)
{
    RT_ASSERT(_dev);
    uint8_t _data[2] = {(uint8_t)(_mode>>8),_mode&0xff};
    __bsp_ad7730_lower_write(_dev,AD7730_REG_MODE,(uint8_t *)&_data,2);
}




//这个函数可以设置AD7730的采集速率
void bsp_ad7730_set_filter(struct ad7730_spi_device *_dev,uint32_t _filter)
{
    RT_ASSERT(_dev);
    uint8_t _data[3] = {(uint8_t)(_filter>>16),(uint8_t)(_filter>>8),_filter&0xff};
    __bsp_ad7730_lower_write(_dev,AD7730_REG_FILTER,(uint8_t *)&_data,3);
}
int32_t bsp_ad7730_readraw(struct ad7730_spi_device *_dev)
{
    RT_ASSERT(_dev);
    uint8_t _data[3] = {0}; //这里使用16位的ADC
    bsp_ad7730_set_mode(ad7730,ad7730->_mode);  //设置单次读
    if (_dev->_get_ready())   //数据有更新
    {
        __bsp_ad7730_lower_read(_dev,SIGNAL_READ_CMD,AD7730_REG_DATA,(uint8_t *)&_data,3);
        // __bsp_ad7730_lower_read_data(_dev,(uint8_t *)&_data,3);
        int32_t _rawdata = (_data[0]<<16)|(_data[1]<<8)|(_data[2]);
        // if (_rawdata & 0x800000)    //负数处理
        // {
        //     _rawdata |= 0xff0000; 
        // }
        _dev->_rawdata = _rawdata;
    }
    return _dev->_rawdata;
}

uint8_t bsp_ad7730_get_status(struct ad7730_spi_device *_dev)
{
    RT_ASSERT(_dev);
    uint8_t _data[1] = {0}; //这里使用16位的ADC
    __bsp_ad7730_lower_read(_dev,SIGNAL_READ_CMD,AD7730_REG_STATUS,(uint8_t *)&_data,1);
    return _data[0];
    
}

#define 	AD6470_READY_PIN	GET_PIN(G,12)  //退盒按键
#define     AD6470_RST_PIN      GET_PIN(B,3)  //复位按键	
static uint8_t bsp_ad7730_get_ready()
{
    return !(rt_pin_read(AD6470_READY_PIN));
    // uint8_t _status = bsp_ad7730_get_status(ad7730);
    // if (_status & 0x80)
    // {
    //     return 0;
    // }
    // return 1;
}



struct ad7730_spi_device*  ad7730;
int bsp_ad7730_init(void)
{
    rt_pin_mode(AD6470_READY_PIN,PIN_MODE_INPUT);	//初始化退盒按键
    rt_pin_mode(AD6470_RST_PIN,PIN_MODE_OUTPUT);



    ad7730 =  bsp_ad7730_create(AD7730_SPI,"spi10",bsp_ad7730_get_ready,GPIOG,GPIO_PIN_10);
    rt_pin_write(AD6470_RST_PIN,1);	//复位
    rt_thread_mdelay(10);
    rt_pin_write(AD6470_RST_PIN,0);	//复位
    rt_thread_mdelay(10);
    rt_pin_write(AD6470_RST_PIN,1);	//复位
    rt_thread_mdelay(10);
    //配置滤波寄存器
    ad7730->_filter = 0x200110;
    bsp_ad7730_set_filter(ad7730,0x200110); //1.200HZ OUTPUT 2.FAST ENABLE 3.CHOP ENABLE
    //配置DAC寄存器
    uint8_t _dac= 0x23;
    __bsp_ad7730_lower_write(ad7730,AD7730_REG_DAC,(uint8_t *)&_dac,1);

    //内部满量程校准
    uint16_t _mode = ((0xA1<<8)|(0xB0&0XFF)) ;   //1.0xA0 满量程校准 +单极性 + 16位寄存器，2.5v+0~80mv 校准
    bsp_ad7730_set_mode(ad7730,_mode);
    while (!ad7730->_get_ready())   //等待校准完成
    {
        rt_thread_delay(10);
    }
    LOG_D("ad7730  fullscale calibration finished.");
    // 内部零量程校准
    _mode = ((0x81<<8)|(0x80&0XFF)) ;   //1.0xA0 满量程校准 +单极性 + 16位寄存器，2.5v+0~10mv 校准
    bsp_ad7730_set_mode(ad7730,_mode);
    while (!ad7730->_get_ready())   //等待校准完成
    {
        rt_thread_delay(10);
    }
    LOG_D("ad7730  zero-scale calibration finished.");
    // 配置工作模式
    // _mode = ((0x21<<8)|(0x80&0xff)); //连续读,单极性,参考电压为5v 范围0~10mv
    // _mode = ((0x41<<8)|(0xB0&0xff)); //单次读,单极性,参考电压为5v 范围0~80mv
    _mode = ((0x41<<8)|(0x90&0xff)); //单次读,单极性,参考电压为5v 范围0~10mv
    ad7730->_mode = _mode;
    bsp_ad7730_set_mode(ad7730,_mode);

    uint8_t _data[2];
    // 读取模式寄存器
   __bsp_ad7730_lower_read(ad7730,SIGNAL_READ_CMD,AD7730_REG_MODE,(uint8_t *)_data,2);


    // 设置单次读
    
    // 设置连续读
    // __bsp_ad7730_lower_read(ad7730,CONTINUOUS_READ_CMD,AD7730_REG_DATA,RT_NULL,0);
    // uint8_t cmdcom = (((AD7730_REG_DATA) & ~(1UL << 7))|(CONTINUOUS_READ_CMD<<4)); //设置要读的寄存器地址
    // __bsp_ad7730_lower_write_data(ad7730,&cmdcom,1);
}
// INIT_BOARD_EXPORT(bsp_ad7730_init);
