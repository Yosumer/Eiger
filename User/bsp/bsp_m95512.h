/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-07-22 09:36:49
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-09-18 14:12:17
 * @FilePath: \01LightModule\light\User\bsp\bsp_m95512.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*------------------------------------------------------------------------------
 * @file    BSP_M95512.H
 * @author  SIERRA TEAM 
 * @date    2024/04/19 10:21:49
 * @brief   THIS FILE PROVIDES ALL THE SIERRA FUNCTIONS.
 * -----------------------------------------------------------------------------
 * @attention 
 
 * THE EXISTING SIERRA IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE 
 * CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE 
 * TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR 
 * CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH 
 * HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN 
 * CONNECTION WITH PRODUCTS MADE BY CUSTOMERS. 
 * 
 * <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2> 
------------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion  ------------------------------------*/
#ifndef __BSP_M95512_H
#define __BSP_M95512_H

/* Files includes  -----------------------------------------------------------*/
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdint.h>
/* Defines -------------------------------------------------------------------*/
#define FLASH_SIZE                  64*1024
#define FLASH_PAGE_SIZE             128


struct e2prom_m95512_device
{
    struct rt_spi_device*  _device;
    struct rt_mutex lock;
    uint32_t    size;          //容量大小
    uint16_t    page_size;
};

typedef struct e2prom_m95512_device* e2prom_m95512_device_t;
/* Variables -----------------------------------------------------------------*/

/* Functions ----------------------------------------------------------------*/
extern e2prom_m95512_device_t bsp_m95512_create(const char *spi_bus_name,const char *spi_dev_name,GPIO_TypeDef *cs_gpiox, uint16_t cs_gpio_pin);
#ifdef RT_USING_SPI_BITOPS
extern e2prom_m95512_device_t bsp_m95512_create_3_soft_spi(const char *spi_bus_name,const char *spi_dev_name,uint32_t _devicesize,uint16_t _page_size,const char *pin_name);
#endif
extern void bsp_m95512_Read(struct e2prom_m95512_device * this,uint16_t addr,uint8_t *buf,uint16_t size);
extern void bsp_m95512_Write(struct e2prom_m95512_device * this,uint16_t addr,uint8_t *buf,uint16_t size);
extern void bsp_m95512_ReadPage(struct e2prom_m95512_device * this,uint16_t pagenum,uint8_t *buf);
extern void bsp_m95512_WritePage(struct e2prom_m95512_device * this,uint16_t pagenum,uint8_t *buf);
void bsp_m95512_ContiueWrite(struct e2prom_m95512_device * _this,uint16_t addr,uint8_t *buf,uint16_t size);

#endif
