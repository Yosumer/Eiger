/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-18 10:47:56
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-03-24 19:06:40
 * @FilePath: \EigeV1.0.01\User\bsp\adc128S102\bsp_adc128S102.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __BSP_ADC128S102_H
#define  __BSP_ADC128S102_H
/* Files includes  -----------------------------------------------------------*/
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdint.h>

#define  SIGNAL_SAMPLE_MODE             0   //单通道采样模式
#define  CONTINUED_SAMPLE_MODE          1   //连续采样模式    

#define ADC128S_SPI                     RT_ADC128S102_SPI_NAME 

struct adc128S102_spi_device
{
    struct rt_spi_device*  _device; 
    struct rt_mutex         lock;
    void                    *cs_port;
    uint16_t                cs_pin;
    void (*cs_ctl)(void*,uint8_t _en);
    uint16_t               _rawdata[8];         // 8个通道的
    uint8_t                _mode;               // 当前采样模式  0:
    uint8_t                _currentCH;          // 当前采样通道
};



struct adc128S102_spi_device * bsp_adc128S102_create(const char *spi_bus_name,const char *spi_dev_name,GPIO_TypeDef *cs_gpiox, uint16_t cs_gpio_pin);
uint16_t bsp_adc128S102_immediatelyReadCh(struct adc128S102_spi_device *dev,uint8_t _ch);   //立即读取
uint16_t bsp_adc128S102_ReadCh(struct adc128S102_spi_device *dev,uint8_t _ch);              //从缓存中读取

extern struct adc128S102_spi_device *adc128s_device;
int  bsp_adc128S102_init();
void  bsp_adc128S102_task();    //把这个任务放在轮训任务中进行调用
#endif // !__BSP_ADC128S102_H