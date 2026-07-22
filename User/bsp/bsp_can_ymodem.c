
/*------------------------------------------------------------------------------
 * @file    BSP_CAN_YMODEM.C
 * @author  FIRMWARE TEAM 
 * @date    2024/04/25 15:34:35
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
#define _BSP_CAN_YMODEM_C_

/* Files includes  -----------------------------------------------------------*/
#include "board.h"
#include "can_ymodem.h"
#include "bsp_can_ymodem.h"
#include "OTA_Manger_Task.h"
#define LOG_TAG            "drv.can_ymodem"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>
CanData_t _pack;
struct rt_can_ymodem_device can_ymodem_device;
/*----------------------------------------------------------------------------*/
static rt_err_t can_pack_send(uint32_t channel, uint8_t *buffer, uint8_t size);
rt_err_t can_pack_send(uint32_t channel,  uint8_t *buffer, uint8_t size)
{
    if(0 == size)
    {
        LOG_E("can pack send size can't be zero!");
        goto __exit;
    }
    OTA_Manger_Task_Down_Rx_Notify(buffer, size);
    return RT_EOK;
__exit:    
    return -RT_ERROR;
}

static int rt_hw_can_ymodem_send(struct rt_can_ymodem_device *can_ymodem,
                                 const void       *buffer,
                                 rt_size_t         size)
{
    can_pack_send(can_ymodem->channel,( uint8_t*)buffer, size);
    return RT_EOK;
} 

void bsp_can_ymodem_recv(const void *buffer, rt_size_t size)
{
    rt_can_ymodem_rx(&can_ymodem_device, buffer, size);
}

static const struct rt_can_ymodem_ops topctrl_can_ymodem_ops =
{
    .puts = rt_hw_can_ymodem_send,
};

int rt_hw_can_ymodem_init(void)
{
    rt_err_t result = 0;
    
    can_ymodem_device.ops = &topctrl_can_ymodem_ops;
    
    can_ymodem_device.channel = CURRENT_MODULE_ID;
    
    result = rt_hw_can_ymodem_register(&can_ymodem_device, "can_ymodem", RT_DEVICE_FLAG_RDWR, RT_NULL);
    
    return result;
}
INIT_DEVICE_EXPORT(rt_hw_can_ymodem_init);