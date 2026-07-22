/*
 * @Author: sum 476328608@qq.com
 * @Date: 2024-04-12 17:04:49
 * @LastEditors: sum 476328608@qq.com
 * @LastEditTime: 2024-05-11 11:36:55
 * @FilePath: \Fluid module 模拟IOP 开放FPUc:\RT-ThreadStudio\workspace\Template\User\base\App_ComFun.c
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
/*------------------------------------------------------------------------------
 * @file    APP_COMFUN.C
 * @author  FIRMWARE TEAM 
 * @date    2024/04/12 17:07:54
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
#include "includes.h"
#include "finsh.h"

/*----------------------------------------------------------------------------*/
/**
 * @description: 原子操作写 防止在写入的过程中被其他线程打断
 * @param {uint32_t *} _addr 待写入的地址
 * @param {uint32_t} _data  待写入的数据
 * @return {*}
 */
void App_Com_WriteRam(uint32_t * _addr,uint32_t _data)
{
    rt_hw_atomic_store((rt_atomic_t *)_addr,_data);
}

//该函数用来测试使用shell终端测试boot升级
int Shell_Boot_SendMsg(uint8_t *_msg,uint8_t _len)
{
    rt_device_t dev = rt_console_get_device();
    rt_device_write(dev,0,&_msg[7],1);
}
/**
 * @description: 设置设备当前模式
 * @param {uint32_t} _mode 
 * @return {*}
 */
void  App_SetDevMode(uint32_t _mode)
{
    App_Com_WriteRam(&ramdata.mode,_mode);
}
uint16_t  App_GetDevMode()
{
    return ramdata.mode;
}

