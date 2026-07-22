
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-03-05     whj4674672   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
/* ���� fal ͷ�ļ� */
#include <fal.h>
/* defined the LED0 pin: PB1 */
#define LED0_PIN    GET_PIN(I, 0)

int main(void)
{
    /* set LED0 pin mode to output */
   rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

    while (1)
    {
       rt_pin_write(LED0_PIN, PIN_HIGH);
       rt_thread_mdelay(500);
       rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}


/**
 * Function    ota_app_vtor_reconfig
 * Description Set Vector Table base location to the start addr of app(RT_APP_PART_ADDR).
*/
static int ota_app_vtor_reconfig(void)
{
    #define NVIC_VTOR_MASK   0x3FFFFF80
    /* Set the Vector Table base location by user application firmware definition */
    SCB->VTOR = RT_APP_PART_ADDR & NVIC_VTOR_MASK;

    return 0;
}
INIT_BOARD_EXPORT(ota_app_vtor_reconfig);