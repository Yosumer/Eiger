/*------------------------------------------------------------------------------
 * @file    HEART_TASK.C
 * @author  FIRMWARE TEAM 
 * @date    2024/05/11 11:20:26
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
// 单指令任务优先级
#define HEART_TASK_PRIO		30
#define HEART_STK_SIZE      256
#
OS_TCB Heart_TaskTCB;
CPU_STK HEART_TASK_STK[HEART_STK_SIZE];
void Heart_Task(void *arg);
/*----------------------------------------------------------------------------*/
int _Heart_task_init()
{
	OS_ERR err;

    ramctl.heartEn  = 1;
	/*初始化单指令任务*/
  	OSTaskCreate( (OS_TCB     *) &Heart_TaskTCB,
                (CPU_CHAR   *) "Heart_Task",
                (OS_TASK_PTR ) Heart_Task,
                (void       *) 0, 
                (OS_PRIO     ) HEART_TASK_PRIO, 
                (CPU_STK    *) &HEART_TASK_STK[0],
                (CPU_STK_SIZE) HEART_STK_SIZE / 10, 
                (CPU_STK_SIZE) HEART_STK_SIZE, 
                (OS_MSG_QTY  ) 10, 
                (OS_TICK     ) 0, 
                (void       *) 0, 
                (OS_OPT      ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP), 
                (OS_ERR     *) &err);
}
INIT_APP_EXPORT(_Heart_task_init);


void Heart_Task(void *arg)
{
    CanData_t _heart = {
        .dir = 0x03,
        .id = CURRENT_MODULE_ID,
    };
    while (1)
    {

        if(ramctl.bootmode != 0x01 && ramctl.heartEn == 1)     //boot模式下不发送
        {
            _heart.addr = BSWAP_16(App_GetDevMode());
            _heart.data = bsp_error_list_getpack();
                            //(电凝默认不检测)
            uint8_t _status = 0 << 7 | Phaco_Task_get_Connection() << 6 |
                            ((ramdata._cassette_state._state)<< 4)|((ramdata._cassette_state._release_sig)<< 3)|
                            ((ramdata._infusion._ivenable)<< 2)|((ramdata._cassette_state._pen_is_connect&Pedal_Task_GetConnectStatus())<< 1); 
            _heart.data |= (_status <<16);

            Data_Distribution_SendMsg((uint8_t*)&_heart,8);
        }
        Delay_ms(HEART_PER_TICK);
    }
    
}