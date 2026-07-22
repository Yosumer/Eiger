/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-04 13:20:44
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-06-30 10:44:14
 * @FilePath: \EigeV1.0.01\libraries\HAL_Drivers\drv_soft_spi.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-6-14      solar        first version
 */

#ifndef __DRV_SOFT_SPI__
#define __DRV_SOFT_SPI__

#include <rthw.h>
#include <rtdevice.h>
#include <spi-bit-ops.h>

/* stm32 soft spi config */
struct stm32_soft_spi_config
{
    rt_uint8_t sck;
    rt_uint8_t mosi;
    rt_uint8_t miso;
    const char *bus_name;
};

/* stm32 soft spi dirver */
struct stm32_soft_spi
{
    struct rt_spi_bit_obj spi;
    struct stm32_soft_spi_config *cfg;
};

#ifdef BSP_USING_SOFT_SPI1
#define SOFT_SPI1_BUS_CONFIG                                    \
    {                                                       \
        .sck = BSP_S_SPI1_SCK_PIN,                          \
        .mosi = BSP_S_SPI1_MOSI_PIN,                        \
        .miso = BSP_S_SPI1_MISO_PIN,                        \
        .bus_name = "sspi1",                                \
    }
#endif /* BSP_USING_SOFT_SPI1 */
#ifdef BSP_USING_SOFT_SPI2
#define SOFT_SPI2_BUS_CONFIG                                    \
    {                                                       \
        .sck = BSP_S_SPI2_SCK_PIN,                          \
        .mosi = BSP_S_SPI2_MOSI_PIN,                        \
        .miso = BSP_S_SPI2_MISO_PIN,                        \
        .bus_name = "sspi2",                                \
    }
#endif /* BSP_USING_SOFT_SPI2 */


#ifdef BSP_USING_SOFT_SPI3
#define SOFT_SPI3_BUS_CONFIG                                    \
    {                                                       \
        .sck = BSP_S_SPI3_SCK_PIN,                          \
        .mosi = BSP_S_SPI3_MOSI_PIN,                        \
        .miso = BSP_S_SPI3_MISO_PIN,                        \
        .bus_name = "sspi3",                                \
    }
#endif /* BSP_USING_SOFT_SPI2 */

rt_err_t rt_hw_soft_spi_device_attach(const char *bus_name, const char *device_name, const char *pin_name);
int rt_soft_spi_init(void);

#endif /* __DRV_SOFT_SPI__ */
