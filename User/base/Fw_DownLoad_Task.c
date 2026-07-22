/*------------------------------------------------------------------------------
 * @file    FW_DOWNLOAD_TASK.C
 * @author  FIRMWARE TEAM 
 * @date    2024/04/17 15:28:43
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
#include "Fw_DownLoad_Task.h"

#include <ymodem.h>
#include <fal.h>
#define LOG_TAG            "task.Download"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>


OS_TCB Fw_down_TaskTCB;
CPU_STK Fw_down_TASK_STK[FW_DOWNLOAD_TASK_STK_SIZE];

#define DEFAULT_DOWNLOAD_PART           "fm_area"

static char* recv_partition = DEFAULT_DOWNLOAD_PART;
static size_t update_file_total_size, update_file_cur_size;
static const struct fal_partition * dl_part = RT_NULL;

rt_device_t can_ymodem_d;
static struct rt_semaphore rx_sem;

void _ymodem_task(void *p_arg);
static rt_err_t can_ymodem_input(rt_device_t dev, rt_size_t size);
static enum rym_code ymodem_on_begin(struct rym_ctx *ctx, rt_uint8_t *buf, rt_size_t len)
{
    char *file_name, *file_size;

    /* calculate and store file size */
    file_name = (char *)&buf[0];
    file_size = (char *)&buf[rt_strlen(file_name) + 1];
    update_file_total_size = atol(file_size);
    rt_kprintf("Ymodem file_size:%d\n", update_file_total_size);

    update_file_cur_size = 0;

    /* Get download partition information and erase download partition data */
    if ((dl_part = fal_partition_find(recv_partition)) == RT_NULL)
    {
        LOG_E("Firmware download failed! Partition (%s) find error!", recv_partition);
        return RYM_CODE_CAN;
    }

    if (update_file_total_size > dl_part->len)
    {
        LOG_E("Firmware is too large! File size (%d), '%s' partition size (%d)", update_file_total_size, recv_partition, dl_part->len);
        return RYM_CODE_CAN;
    }

    LOG_I("Start erase. Size (%d)", update_file_total_size);

    /* erase DL section */
    if (fal_partition_erase(dl_part, 0, update_file_total_size) < 0)
    {
        LOG_E("Firmware download failed! Partition (%s) erase error!", dl_part->name);
        return RYM_CODE_CAN;
    }

    return RYM_CODE_ACK;
}

static enum rym_code ymodem_on_data(struct rym_ctx *ctx, rt_uint8_t *buf, rt_size_t len)
{
    /* write data of application to DL partition  */
    if (fal_partition_write(dl_part, update_file_cur_size, buf, len) < 0)
    {
        LOG_E("Firmware download failed! Partition (%s) write data error!", dl_part->name);
        return RYM_CODE_CAN;
    }

    update_file_cur_size += len;

    return RYM_CODE_ACK;
}
/*----------------------------------------------------------------------------*/
// int Fw_down_Task_Init(void)
// {
// 	OS_ERR err;
//     // 固件下载子任务
//     OSTaskCreate( (OS_TCB     *) &Fw_down_TaskTCB,
//                 (CPU_CHAR   *) "Fw_down_Task",
//                 (OS_TASK_PTR ) Fw_down_Task,
//                 (void       *) 0, 
//                 (OS_PRIO     ) FW_DOWNLOAD_TASK_PRIO, 
//                 (CPU_STK    *) &Fw_down_TASK_STK[0],
//                 (CPU_STK_SIZE) FW_DOWNLOAD_TASK_STK_SIZE / 10, 
//                 (CPU_STK_SIZE) FW_DOWNLOAD_TASK_STK_SIZE, 
//                 (OS_MSG_QTY  ) 50, 
//                 (OS_TICK     ) 0, 
//                 (void       *) 0, 
//                 (OS_OPT      ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP), 
//                 (OS_ERR     *) &err);
//     can_ymodem_d = rt_device_find("can_ymodem");
//     if(can_ymodem_d == RT_NULL)
//     {
//         LOG_E("find dev can_ymodem faild");
//         return RT_ERROR;
//     } 
//     // rt_sem_init(&rx_sem,"can_ymode_sem",0,RT_IPC_FLAG_FIFO);
//     // rt_device_set_rx_indicate(can_ymodem_d,can_ymodem_input);
// 	return 0;
// }
// INIT_APP_EXPORT(Fw_down_Task_Init);
/* 接收数据回调函数 */
static rt_err_t can_ymodem_input(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(&rx_sem);

    return RT_EOK;
}
// void Fw_down_Task(void *p_arg)
// {
//     uint32_t *data = NULL;
//     uint8_t _bytes[4];
//     uint8_t _byte;
// 	OS_MSG_SIZE msg_size_t;
// 	OS_ERR err;
// 	CPU_TS ts_t;
// 	while(1)
// 	{
// //        data = OSTaskQPend(0,OS_OPT_PEND_BLOCKING,&msg_size_t,&ts_t,&err);		                    
// //        _byte = (uint8_t)*data;
// //        bsp_can_ymodem_recv(&_byte,1);

//         //4字节处理
//         // data = OSTaskQPend(0,OS_OPT_PEND_BLOCKING,&msg_size_t,&ts_t,&err);	
//         // *data = BSWAP_32(*data);
//         // memcpy(_bytes,data,4);

// //		uint32_t _mask = *data & 0x55AA5500;
// //        if(_mask == 0x55AA5500)
// //        {
// //            if(_bytes[0] ==0x01)
// //            {
// ////                    bsp_can_ymodem_recv(&_bytes[0],1);
// //            }
// //            else if(_bytes[0] ==0x02)
// //            {
// ////                bsp_can_ymodem_recv(&_bytes[0],1);
// //            }
// //            else if(_bytes[0] ==0x04)
// //            {
// ////                bsp_can_ymodem_recv(&_bytes[0],1);
// //            }
// //                else if(_bytes[0] ==0x18)
// //            {
// ////                bsp_can_ymodem_recv(&_bytes[0],1);
// //            }					
// //            else
// ////                    bsp_can_ymodem_recv(_bytes,4);
// //        }
// //        else
// //        {
// ////            bsp_can_ymodem_recv(_bytes,4);
// //        }
// 	} 
// }

void _ymodem_task(void *p_arg)
{
    struct rym_ctx rctx;
    LOG_I("Warning: Ymodem has started! This operator will not recovery.\n");
    LOG_I("Please select the ota firmware file and use Ymodem to send.\n");
    can_ymodem_d = rt_device_find("can_ymodem");
    if(can_ymodem_d == RT_NULL)
    {
        LOG_E("find dev can_ymodem faild");
        return;
    } 
    if (!rym_recv_on_device(&rctx, can_ymodem_d, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                            ymodem_on_begin, ymodem_on_data, NULL, RT_TICK_PER_SECOND*20))
    {
			
        ramctl.bootmode = 0;		//退出Boot模式
        rt_kprintf("Download firmware to flash success.\n");              
        
    }
    else
    {
        rt_kprintf("Download firmware to flash fail.\n");
    } 
}

void can_ymode_ota(uint8_t argc, char **argv)
{
    static CanData_t _data = {
        .dir = 0x01,
        .id = CURRENT_MODULE_ID,
    };
    uint8_t _ch;
    ramcof.config = PROJECT_MODE;   //设置当前设备模式为工程模式
    _data.addr = 0X0E0E;
    _data.data = BSWAP_32(1);
    //将数据发送到数据处理任务
    int result;
    /* 发送消息到消息队列中 */
    result = rt_mq_send_wait(&downLD_mq, &_data, 8,1000);
    if (result != RT_EOK)
    {
        LOG_D("rt_mq_send ERR\n");
    }
    _data.addr = BSWAP_16(BOOTLOADER_INSTRUCTION);
    Data_Distribution_CtlMode(COM_USE_BOOT);
    while (1)
    {
        _ch = finsh_getchar(); //读数据
        _data.data = BSWAP_32(_ch);
        rt_mq_send(&downLD_mq, &_data, 8);
        rt_thread_delay(1);
    }
}
MSH_CMD_EXPORT(can_ymode_ota, Use Y-MODEM to download the firmware);
