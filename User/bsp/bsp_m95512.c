/*------------------------------------------------------------------------------
 * @file    BSP_M95512.C
 * @author  FIRMWARE TEAM 
 * @date    2024/04/19 11:10:10
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
#include "bsp_m95512.h"
#include "drv_spi.h"
#include "drv_soft_spi.h"
#define LOG_TAG            "drv.m95512"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>


#define WRITE_ENABLE		0x06    //写使能
#define WRITE_DISABLE		0x04    //写失能
#define READ_STATUS_REG		0x05    //读状态寄存器
#define WRITE_STATUS_REG	0x01    //写状态寄存器
#define READ_MEM_ARRAY		0x03    //都内存指令
#define WRITE_MEM_ARRAY		0x02    //写内存指令
#define MUTSTR              "_mut"

/*----------------------------------------------------------------------------*/
/**
 * @description: 创建一个m95512设备
 * @param {char} *spi_bus_name  spi总线名称
 * @param {char} *spi_dev_name  spi设备名称
 * @param {char} *cs_name       cs引脚名称 (使用PIN框架)
 * @return {*}
 */
extern e2prom_m95512_device_t bsp_m95512_create(const char *spi_bus_name,const char *spi_dev_name,GPIO_TypeDef *cs_gpiox, uint16_t cs_gpio_pin)
{
    e2prom_m95512_device_t m95512_dev;
    RT_ASSERT(spi_bus_name);
    RT_ASSERT(spi_dev_name);
    m95512_dev = rt_calloc(1,sizeof(struct e2prom_m95512_device));

    //挂载设备到总线上
    /* PIN框架级 rt_pin_get api方式 */
    rt_err_t res = rt_hw_spi_device_attach(spi_bus_name, spi_dev_name, cs_gpiox, cs_gpio_pin);
    if (res != RT_EOK)
    {
        LOG_E("rt_spi_bus_attach_device!");
        return RT_NULL;
    }
    m95512_dev->_device = (struct rt_spi_device*)rt_device_find(spi_dev_name);
    if (m95512_dev->_device == RT_NULL)
    {
        LOG_E("spi device %s not found!\r\n", spi_dev_name);
        return RT_NULL;
    }
    //config spi
    {
        struct rt_spi_configuration _cfg;
        _cfg.data_width = 8;
        _cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB;
        _cfg.max_hz = 16 * 1000 *1000; 
        rt_spi_configure(m95512_dev->_device, &_cfg);
    }
    rt_mutex_init(&m95512_dev->lock, "m95512_mut", RT_IPC_FLAG_FIFO);

    // 设置默认容量大小
    m95512_dev->page_size = FLASH_PAGE_SIZE;
    m95512_dev->size = FLASH_SIZE;
    return m95512_dev;

}
#ifdef RT_USING_SPI_BITOPS
e2prom_m95512_device_t bsp_m95512_create_3_soft_spi(const char *spi_bus_name,const char *spi_dev_name,uint32_t _devicesize,uint16_t _page_size,const char *pin_name)
{
    e2prom_m95512_device_t m95512_dev;
    RT_ASSERT(spi_bus_name);
    RT_ASSERT(spi_dev_name);
    m95512_dev = rt_calloc(1,sizeof(struct e2prom_m95512_device));

    // 设置容量
    m95512_dev->page_size = _page_size;
    m95512_dev->size = _devicesize;

    rt_err_t res = rt_hw_soft_spi_device_attach(spi_bus_name, spi_dev_name, pin_name);
    if (res != RT_EOK)
    {
        LOG_E("rt_spi_bus_attach_device!");
        return RT_NULL;
    }
    m95512_dev->_device = (struct rt_spi_device*)rt_device_find(spi_dev_name);
    if (m95512_dev->_device == RT_NULL)
    {
        LOG_E("spi device %s not found!\r\n", spi_dev_name);
        return RT_NULL;
    }
    //config spi
    {
        struct rt_spi_configuration _cfg;
        _cfg.data_width = 8;
        _cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB | RT_SPI_3WIRE;
        _cfg.max_hz = 10*1000; 
        rt_spi_configure(m95512_dev->_device, &_cfg);
    }
	
    char *_name=  rt_calloc(RT_NAME_MAX,1);
    if(_name == RT_NULL)
    {
        LOG_E("rt_calloc mutex %s failed",spi_dev_name);
        return RT_NULL;
    }
    rt_strcpy(_name,spi_dev_name);
    strcat(_name,MUTSTR);
    rt_mutex_init(&m95512_dev->lock,_name, RT_IPC_FLAG_FIFO);

    rt_base_t _cs_pin = rt_pin_get(pin_name);


//     rt_pin_write(_cs_pin,PIN_LOW);
//		 rt_hw_us_delay(200);
////		rt_thread_delay(30);
//     rt_pin_write(_cs_pin,PIN_HIGH);

	return m95512_dev;
}
#endif
static void bsp_m95512_writeEnable(struct e2prom_m95512_device * _this,uint8_t _en)
{
    rt_err_t result;
    static uint8_t sendmsg = WRITE_ENABLE;
    static struct rt_spi_message _sendmsg;
    result = rt_mutex_take(&_this->lock, RT_WAITING_FOREVER);
    if(_en == 1)
        sendmsg = WRITE_ENABLE;
    else
        sendmsg = WRITE_DISABLE;
    if(result == RT_EOK)
    {
        //send read cmd
        _sendmsg.send_buf   = &sendmsg;
        _sendmsg.recv_buf   = RT_NULL;
        _sendmsg.length     = 1;
        _sendmsg.cs_take    = 1;
        _sendmsg.cs_release = 1;
        _sendmsg.next       = RT_NULL;
        rt_spi_transfer_message(_this->_device, &_sendmsg);
    }
     rt_mutex_release(&_this->lock);
}

void bsp_m95512_Read(struct e2prom_m95512_device * _this,uint16_t addr,uint8_t *buf,uint16_t size)
{
    rt_err_t result;
    static uint8_t sendcmd[3]={READ_MEM_ARRAY,0x00,0x00};
    static struct rt_spi_message _sendmsg,_readmsg;
    result = rt_mutex_take(&_this->lock, RT_WAITING_FOREVER);
    if(result == RT_EOK)
    {
        //send read cmd
        sendcmd[1] = (uint8_t) ((addr & 0xFF00) >> 8);
        sendcmd[2] = (uint8_t) ((addr & 0x00FF));
        _sendmsg.send_buf   = sendcmd;
        _sendmsg.recv_buf   = RT_NULL;
        _sendmsg.length     = 3;
        _sendmsg.cs_take    = 1;
        _sendmsg.cs_release = 0;
        _sendmsg.next       = &_readmsg;

        _readmsg.send_buf   = RT_NULL;
        _readmsg.recv_buf   = buf;
        _readmsg.length     = size;
        _readmsg.cs_take    = 0;
        _readmsg.cs_release = 1;
        _readmsg.next       = RT_NULL;
        rt_spi_transfer_message(_this->_device, &_sendmsg);
    }
    rt_mutex_release(&_this->lock);
}
void bsp_m95512_Write(struct e2prom_m95512_device * _this,uint16_t addr,uint8_t *buf,uint16_t size)
{
    rt_err_t result;
    static uint8_t sendcmd[3]={WRITE_MEM_ARRAY,0x00,0x00};
    static struct rt_spi_message _sendcmdmsg,_sendmsg;
    bsp_m95512_writeEnable(_this,1);
    result = rt_mutex_take(&_this->lock, RT_WAITING_FOREVER);
    if(result == RT_EOK)
    {
        sendcmd[1] = (uint8_t) ((addr & 0xFF00) >> 8);
        sendcmd[2] = (uint8_t) ((addr & 0x00FF));
        _sendcmdmsg.send_buf   = sendcmd;
        _sendcmdmsg.recv_buf   = RT_NULL;
        _sendcmdmsg.length     = 3;
        _sendcmdmsg.cs_take    = 1;
        _sendcmdmsg.cs_release = 0;
        _sendcmdmsg.next       = &_sendmsg;

        _sendmsg.send_buf   = buf;
        _sendmsg.recv_buf   = RT_NULL;
        _sendmsg.length     = size;
        _sendmsg.cs_take    = 0;
        _sendmsg.cs_release = 1;
        _sendmsg.next       = RT_NULL;
        
        rt_spi_transfer_message(_this->_device, &_sendcmdmsg);
    }
    bsp_m95512_writeEnable(_this,0);
    rt_mutex_release(&_this->lock);
    rt_thread_delay(5);
}
void bsp_m95512_ReadPage(struct e2prom_m95512_device * _this,uint16_t pagenum,uint8_t *buf)
{
    bsp_m95512_Read(_this,pagenum*_this->page_size,buf,_this->page_size);
}
void bsp_m95512_WritePage(struct e2prom_m95512_device * _this,uint16_t pagenum,uint8_t *buf)
{
    bsp_m95512_Write(_this,pagenum*_this->page_size,buf,_this->page_size);
}

void bsp_m95512_ContiueWrite(struct e2prom_m95512_device * _this,uint16_t addr,uint8_t *buf,uint16_t size)
{
	uint8_t  _addroffset = addr %_this->page_size;
	uint8_t _fristpagewritesize = (_this->page_size - _addroffset);
	size -= _fristpagewritesize;
	uint16_t _pagesize = (size / _this->page_size);
	uint8_t  _pageoffset = (size % _this->page_size);
	bsp_m95512_Write(_this,addr,buf,_fristpagewritesize);	//写入首页
	buf +=_fristpagewritesize;
	addr+= _fristpagewritesize;
	for(uint16_t i=0;i<_pagesize;i++)
	{
		bsp_m95512_Write(_this,addr,buf,_this->page_size);
		buf+=_this->page_size;
		addr+= _this->page_size;
	}
	if(_pageoffset>0)	//写入尾页
	{
		bsp_m95512_Write(_this,addr,buf,_pageoffset);
	}
}

void bsp_m95512_deinit(struct e2prom_m95512_device * _this)
{
    RT_ASSERT(_this);
    rt_free(_this);
}





#define __is_printc(ch)                ((unsigned int)((ch) - ' ') < 127u - ' ')
#ifdef RT_USING_FINSH
uint8_t TEST_BUFFER[] = "WELCOM TO RTT";
#define SIZE sizeof(TEST_BUFFER)
void m95512(int argc, char *argv[])
{
    static e2prom_m95512_device_t dev = RT_NULL;
    if(argc >1)
    {
        if (!strcmp(argv[1], "probe"))
        {
            if (argc >= 2)
            {
                /* deinit the old device */
                if (dev)
                {
                    bsp_m95512_deinit(dev);
                }
                // dev = at24cxx_init(argv[2], atoi(argv[3]));
                if(argc == 2)
                    dev = bsp_m95512_create("spi2","spi20",GPIOB,GPIO_PIN_12);
                else
                {
                    dev = bsp_m95512_create_3_soft_spi(argv[2],argv[3],256*1024,64,argv[4]);
                    if(dev)
                        rt_kprintf("m95512 probe sucessful");
                    else
                        rt_kprintf("m95512 probe faild");
                }
            }
            else
            {
                rt_kprintf("m95512 probe - probe sensor by given name\n");
            }
        }
        else if (!strcmp(argv[1], "read"))
        {
            if (dev)
            {
                uint8_t testbuffer[50];

                if (argc < 4)
                {
                    bsp_m95512_Read(dev,0,testbuffer,SIZE);
                    rt_kprintf("read m95512 : %s\n", testbuffer);
                }
                else
                {
                    uint16_t addr = strtol(argv[2], NULL, 0);
                    uint16_t size = strtol(argv[3], NULL, 0);
                    uint8_t *data = rt_malloc(size);
                    if (data)
                    {
                        bsp_m95512_Read(dev,addr,data,size);
                        rt_kprintf("Read data success. Start from 0x%08X, size is %ld. The data is:\n", addr,
                                size);
                        rt_kprintf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
                        for (int i = 0; i < size; i += 16)
                        {
                            rt_kprintf("[%08X] ", addr + i);
                            /* dump hex */
                            for (int j = 0; j < 16; j++)
                            {
                                if (i + j < size)
                                {
                                    rt_kprintf("%02X ", data[i + j]);
                                }
                                else
                                {
                                    rt_kprintf("   ");
                                }
                            }
                            /* dump char for hex */
                            for (int j = 0; j < 16; j++)
                            {
                                if (i + j < size)
                                {
                                    rt_kprintf("%c", __is_printc(data[i + j]) ? data[i + j] : '.');
                                }
                            }
                            rt_kprintf("\n");
                        }
                        rt_kprintf("\n");
                        rt_free(data);
                    }
                    else
                    {
                        rt_kprintf("Low memory!\n");
                    }
                }
            }
            else
            {
                rt_kprintf("Please using 'm95512 probe ' first\n");
            }
        }
        else if (!strcmp(argv[1], "write"))
        {
            bsp_m95512_Write(dev, 0, TEST_BUFFER, SIZE);
            rt_kprintf("write ok\n");
        }
        else
        {
            rt_kprintf("Unknown command. Please enter 'm95512' for help\n");
        }
    }
    else
    {
        rt_kprintf("Usage:\n");
        rt_kprintf("m95512 probe               - probe flash spi20 on spi2\n");
        rt_kprintf("m95512 read               - read flash m95512 data\n");
        rt_kprintf("m95512 write              - write flash m95512 data\n");
    }
}
MSH_CMD_EXPORT(m95512, m95512 flash function);
#endif
