
/*
 * @Author: sum 476328608@qq.com
 * @Date: 2024-04-07 15:54:08
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-28 17:03:10
 * @FilePath: \Fluid module ???IOP ????FPUc:\RT-ThreadStudio\workspace\Template\User\base\Can_Rx_Task.c
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#include <stdint.h>
#include <rtthread.h>
#include "rtdevice.h"
#include "Can_Rx_Task.h"
#include "Tcp_Client_Task.h"

#define LOG_TAG            "task.can_rx"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>
#define CAN_DEV_NAME       "fdcan1"      /* CAN ?υτ???? */
#define     CAN_RX_TASK_SIZE       1024
#define     CAN_RX_TASK_PRIO       8
static struct rt_semaphore rx_sem;     /* ????????????????? */
static struct rt_mutex _can_tx_mutex; /* CAN ?????????*/
static rt_device_t can_dev;            /* CAN ?υτ???? */

rt_uint8_t  can_task_stack[CAN_RX_TASK_SIZE];
static struct rt_thread can_rx_tcb;  // can??????????


extern struct rt_messagequeue downLD_mq;       //??????????

/* ?????????????? */
static rt_err_t can_rx_call(rt_device_t dev, rt_size_t size)
{
    /* CAN ??????????????§Ψ????????????????????????????? */
    rt_sem_release(&rx_sem);

    return RT_EOK;
}

static void can_rx_thread(void *parameter)
{
    rt_err_t res;
    struct rt_can_msg rxmsg = {0};

    /* ????????????? */
    rt_device_set_rx_indicate(can_dev, can_rx_call);

#ifdef RT_CAN_USING_HDR
    struct rt_can_filter_item items[2] =
    {
		{PEDAL_MODULE_CAN_ID, 0, 0, 0, 0x7ff, -1,}, //???CAN ID????
		{SELF_CAN_ID, 0, 0, 0, 0x7ff, 1,},  //???CAN ID????
    };
    struct rt_can_filter_config cfg = {2, 2, items}; /* ????? 2 ??????? */
    /* ???????????? */
    res = rt_device_control(can_dev, RT_CAN_CMD_SET_FILTER, &cfg);
    RT_ASSERT(res == RT_EOK);
#endif

    while (1)
    {
        /* hdr ?? - 1????????? uselist ??????????? */
        rxmsg.hdr_index = -1;
        /* ???????????????? */
        rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        /* ?? CAN ????????? */
        rt_device_read(can_dev, 0, &rxmsg, sizeof(rxmsg));
        if (rxmsg.data[2] == 0x0E && rxmsg.data[3] == 0x0F)   //OTA???
        {
            OTA_Manger_Task_Down_Rx_Notify(&rxmsg.data[7],1);
        }
        else
        {
            int result;
            /* ?????????????????? */
            if (rxmsg.data[1] == 0xA5)
            {
                result =  tcp_client_no_block_send(rxmsg.data,8);
                if (result != RT_EOK)
                {
                    LOG_D("can send tcp  ERR\n");
                }
                result = rt_mq_send(&downLD_mq, rxmsg.data, 8);
                if (result != RT_EOK)
                {
                    LOG_D("rt_mq_send ERR\n");
                }
            }
            else 
            {
                result = tcp_server_default_Send(rxmsg.data,8);
                if (result == -RT_ERROR)
                {
                    LOG_D("rt_mq_send ERR\n");
                }
            }
        }
        

    }
}
/**
 * @description: CAN????????????
 * @return void
 */
int Can_Task_Init()
{
    rt_err_t res;
    /* ???? CAN ?υτ */
    can_dev = rt_device_find(CAN_DEV_NAME);
    if (!can_dev)
    {
        LOG_D("find %s failed!\n", CAN_DEV_NAME);
        return RT_ERROR;
    }
    /* ????? CAN ????????? */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);

    /*??????????????*/
    rt_mutex_init(&_can_tx_mutex, "can_tx", RT_IPC_FLAG_FIFO);
    /* ???§Ψ????????????? CAN ?υτ */
    res = rt_device_open(can_dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);
    RT_ASSERT(res == RT_EOK);
    /* ?????????????? */
    res =  rt_thread_init(&can_rx_tcb,"can_rx",can_rx_thread,RT_NULL,can_task_stack,CAN_RX_TASK_SIZE,CAN_RX_TASK_PRIO,10);
    RT_ASSERT(res == RT_EOK);
    rt_thread_startup(&can_rx_tcb);  
    LOG_D("CAN_RX_TASK INIT OK!!");
}
INIT_APP_EXPORT(Can_Task_Init);


int Can_Task_SendMsg(uint8_t *_msg,uint8_t _len)
{
    struct rt_can_msg msg = {0};
    rt_size_t  size;
    rt_mutex_take(&_can_tx_mutex,RT_WAITING_FOREVER);
    msg.id = PEDAL_MODULE_CAN_ID;     /*???????????ID*/
    msg.ide = RT_CAN_STDID;     /* ?????? */
    msg.rtr = RT_CAN_DTR;       /* ????? */
    msg.len = _len;                /* ???????? 8 */
    /* ??????? 8 ??????? */
    rt_memcpy(msg.data,_msg,_len);

    /* ?????? CAN ???? */
    size = rt_device_write(can_dev, 0, &msg, sizeof(msg));
    if (size == 0)
    {
        LOG_D("can dev write data failed!\n");
    }
    rt_mutex_release(&_can_tx_mutex);
    return size;
}


int Can_Pedal_Task_OTA_Send(uint8_t *_msg,uint8_t _len)
{
    struct rt_can_msg msg = {0};
    rt_size_t  size;
    rt_mutex_take(&_can_tx_mutex,RT_WAITING_FOREVER);
    msg.id = (0xFF-PEDAL_MODULE_CAN_ID);     /*???????????ID*/
    msg.ide = RT_CAN_STDID;     /* ?????? */
    msg.rtr = RT_CAN_DTR;       /* ????? */
    msg.len = _len;                /* ???????? 8 */
    /* ??????? 8 ??????? */
    rt_memcpy(msg.data,_msg,_len);

    /* ?????? CAN ???? */
    size = rt_device_write(can_dev, 0, &msg, sizeof(msg));
    if (size == 0)
    {
        LOG_D("can dev write data failed!\n");
    }
    rt_mutex_release(&_can_tx_mutex);
    return size;
}