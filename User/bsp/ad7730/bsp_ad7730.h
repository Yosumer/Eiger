/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-21 09:58:38
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-03-25 10:45:55
 * @FilePath: \EigeV1.0.01\User\bsp\ad7730\bsp_ad7730.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%A
 */
// 防止头文件被重复包含
#ifndef BSP_AD7730_H
#define BSP_AD7730_H

// 包含必要的头文件
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdint.h>



#define AD7730_SPI                     RT_AD7730_SPI_NAME  // 定义SPI总线名称 


struct ad7730_spi_device
{
    struct rt_spi_device *device;
    rt_mutex_t _lock;
    uint8_t (*_get_ready)();    //获取就绪信号 
    uint8_t    _status;         //状态寄存器
    uint16_t   _mode;           //当前模式 
    uint32_t   _filter;         //滤波系数
    int32_t    _rawdata;        //原始数据
};





// 函数声明
// 初始化AD7730
struct ad7730_spi_device * bsp_ad7730_create(const char *spi_bus_name,const char *spi_dev_name,uint8_t (*is_ready)(),GPIO_TypeDef *cs_gpiox, uint16_t cs_gpio_pin);
// 向AD7730写入数据
void bsp_ad7730_set_mode(struct ad7730_spi_device *_dev,uint16_t _mode);
//这个函数可以设置AD7730的采集速率
void bsp_ad7730_set_filter(struct ad7730_spi_device *_dev,uint32_t _filter);
int32_t bsp_ad7730_readraw(struct ad7730_spi_device *_dev);


extern struct ad7730_spi_device*  ad7730;

int bsp_ad7730_init(void);
void  bsp_ad7730_task();    //把这个函数放到轮训任务重进行调用
#endif // BSP_AD7730_H
