/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-04 13:21:02
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-03 17:15:25
 * @FilePath: \EigeV1.0.01\User\port\spi_flash_init.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-27     SummerGift   add spi flash port file
 */

#include <rtthread.h>
#include "spi_flash.h"
#include "spi_flash_sfud.h"
#include "drv_spi.h"
#include "fal.h"

#define FLASH_WRITE_PIN      GET_PIN(H, 7)
static int rt_hw_spi_flash_init(void)
{

    rt_pin_mode(FLASH_WRITE_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(FLASH_WRITE_PIN, PIN_HIGH);
#if defined(BSP_USING_SPI_FLASH)
   __HAL_RCC_GPIOH_CLK_ENABLE();
   rt_hw_spi_device_attach("spi4", "spi40", GPIOH, GPIO_PIN_6);

   if (RT_NULL == rt_sfud_flash_probe(FAL_USING_NOR_FLASH_DEV_NAME, "spi40"))
   {
       return -RT_ERROR;
   };
#endif
    return RT_EOK;
}
INIT_COMPONENT_EXPORT(rt_hw_spi_flash_init);

