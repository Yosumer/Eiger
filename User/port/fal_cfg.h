/*** 
 * @Author: sum 476328608@qq.com
 * @Date: 2024-04-22 11:19:34
 * @LastEditors: sum 476328608@qq.com
 * @LastEditTime: 2024-05-28 14:18:21
 * @FilePath: \Fluid module 模拟IOP 开放FPUc:\RT-ThreadStudio\workspace\Template\User\port\fal_cfg.h
 * @Description: 
 * @
 * @Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */

/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-5      SummerGift   first version
 */

#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

#include <rtthread.h>
#include <board.h>

 #define RT_APP_PART_ADDR    0X08020000      //APP 首地址
//#define RT_APP_PART_ADDR    0X08000000      //APP 首地址
#define FLASH_SIZE_GRANULARITY_16K   (4 * 16 * 1024)
#define FLASH_SIZE_GRANULARITY_64K   (64 * 1024)
#define FLASH_SIZE_GRANULARITY_128K  (7 * 128 * 1024)
#define FLASH_SIZE_GRANULARITY_4096K  (4 * 1024 * 1024)

#define STM32_FLASH_START_ADRESS_16K  STM32_FLASH_START_ADRESS
#define STM32_FLASH_START_ADRESS_64K  (STM32_FLASH_START_ADRESS_16K + FLASH_SIZE_GRANULARITY_16K)
#define STM32_FLASH_START_ADRESS_128K (STM32_FLASH_START_ADRESS_64K + FLASH_SIZE_GRANULARITY_64K)

// extern const struct fal_flash_dev stm32_onchip_flash_16k;
// extern const struct fal_flash_dev stm32_onchip_flash_64k;
extern const struct fal_flash_dev stm32_onchip_flash_128k;
#ifdef  BSP_USING_SPI_E2PROM 
extern struct fal_flash_dev e2prom_flash;
    /* flash device table */
    #define FAL_FLASH_DEV_TABLE                                          \
    {                                                                    \
        &stm32_onchip_flash_128k,                                        \
        &e2prom_flash,                                                    \
    }																	

    /* ====================== Partition Configuration ========================== */
    #ifdef FAL_PART_HAS_TABLE_CFG

    /* partition table */
    #define FAL_PART_TABLE                                                                                                     \
    {                                                                                                                          \
        {FAL_PART_MAGIC_WROD, "app",        "onchip_flash_128k", 0 , FLASH_SIZE_GRANULARITY_128K, 0}, \
        {FAL_PART_MAGIC_WROD, "e2prom",     "exflash_64k", 0 , FLASH_SIZE_GRANULARITY_64K, 0}, \
    }																							

    #endif /* FAL_PART_HAS_TABLE_CFG */
#endif  /*BSP_USING_SPI_E2PROM*/

#ifdef  BSP_USING_SPI_FLASH 
    #define NOR_FLASH_DEV_NAME             FAL_USING_NOR_FLASH_DEV_NAME
    extern struct fal_flash_dev nor_flash0;
    /* flash device table */
    #define FAL_FLASH_DEV_TABLE                                          \
    {                                                                    \
        &stm32_onchip_flash_128k,                                        \
        &nor_flash0,                                                    \
    }																	

    /* ====================== Partition Configuration ========================== */
    #ifdef FAL_PART_HAS_TABLE_CFG

    /* partition table */
    #define FAL_PART_TABLE                                                                                                     \
    {                                                                                                                          \
        {FAL_PART_MAGIC_WROD, "app",        "onchip_flash_128k", 0 , FLASH_SIZE_GRANULARITY_128K, 0}, \
        {FAL_PART_MAGIC_WROD, "fm_area",     NOR_FLASH_DEV_NAME, 0 , 1 * 1024 * 1024, 0},            \
        {FAL_PART_MAGIC_WROD, "df_area",     NOR_FLASH_DEV_NAME, 1 * 1024 * 1024 , 1 * 1024 * 1024, 0},            \
        {FAL_PART_MAGIC_WROD, "filesystem", NOR_FLASH_DEV_NAME, 2 * 1024 * 1024, 1 * 1024 * 1024, 0},\
        {FAL_PART_MAGIC_WROD, "dev_param", NOR_FLASH_DEV_NAME, 3 * 1024 * 1024, 64 * 1024, 0},\
        {FAL_PART_MAGIC_WROD, "fdb_kv", NOR_FLASH_DEV_NAME, ((3 * 1024)+64) * 1024, 480 * 1024, 0},\
        {FAL_PART_MAGIC_WROD, "fdb_ts", NOR_FLASH_DEV_NAME, ((3 * 1024)+544) * 1024, 480 * 1024, 0},\
    }																							
    #endif /* FAL_PART_HAS_TABLE_CFG */

#endif

#endif /* _FAL_CFG_H_ */
