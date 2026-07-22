
/*
 * @Author: sum 476328608@qq.com
 * @Date: 2024-04-07 15:54:08
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-05-29 15:35:04
 * @FilePath: \Fluid module ???IOP ????FPUc:\RT-ThreadStudio\workspace\Template\User\base\Can_Rx_Task.c
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#include <stdint.h>
#include <rtthread.h>
#include "rtdevice.h"

#include "ErigerConfig.h"
#include "Can_Rx_Task.h"
#include "Can_IV_Task.h"
#define LOG_TAG            "tsk.can_iv"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>
#define CAN_IV_DEV_NAME       "fdcan2"      
#define     CAN_IV_RX_TASK_SIZE       512
#define     CAN_IV_RX_TASK_PRIO       10
static struct rt_semaphore rx_iv_sem;     /* ????????????????? */
static struct rt_mutex _can_iv_tx_mutex; /* CAN ?????????*/
static rt_device_t can_iv_dev;            /* CAN ?őô???? */

rt_uint8_t  can_iv_task_stack[CAN_IV_RX_TASK_SIZE];
static struct rt_thread can_iv_rx_tcb;  // can??????????


extern struct rt_messagequeue downLD_mq;       //??????????

/* ?????????????? */
static rt_err_t can_iv_rx_call(rt_device_t dev, rt_size_t size)
{
    /* CAN ??????????????§Ř????????????????????????????? */
    rt_sem_release(&rx_iv_sem);

    return RT_EOK;
}

static void can_iv_rx_thread(void *parameter)
{
    rt_err_t res;
    struct rt_can_msg rxmsg = {0};

    /* ????????????? */
    rt_device_set_rx_indicate(can_iv_dev, can_iv_rx_call);

#ifdef RT_CAN_USING_HDR
    struct rt_can_filter_item items[2] =
    {
        {IV_MODULE_CAN_ID, 0, 0, 0, 0x7ff, -1,}, //???CAN ID????
        {SELF_CAN_ID, 0, 0, 0, 0x7ff, 1,},  //???CAN ID????
    };
    struct rt_can_filter_config cfg = {2, 2, items}; /* ????? 2 ??????? */
    /* ???????????? */
    res = rt_device_control(can_iv_dev, RT_CAN_CMD_SET_FILTER, &cfg);
    RT_ASSERT(res == RT_EOK);
#endif

    while (1)
    {
        /* hdr ?? - 1????????? uselist ??????????? */
        rxmsg.hdr_index = -1;
        /* ???????????????? */
        rt_sem_take(&rx_iv_sem, RT_WAITING_FOREVER);
        /* ?? CAN ????????? */
        rt_device_read(can_iv_dev, 0, &rxmsg, sizeof(rxmsg));

        if (rxmsg.data[2] == 0x0E && rxmsg.data[3] == 0x0F)   //OTA的数据
        {
            OTA_Manger_Task_Down_Rx_Notify(&rxmsg.data[7],1);
        }
		else if(rxmsg.data[1] == 0xe5 )
		{
			if(0x01 == rxmsg.data[0])
			{
				rt_mq_send(&downLD_mq, rxmsg.data, 8);
				
				//loop back
				Can_IV_Task_SendMsg(rxmsg.data ,8);
			}
			else if(0x02 == rxmsg.data[0])
			{
				uint32_t *ack_data = (uint32_t *)&ramdata.mode;
				uint16_t addr_offset = rxmsg.data[2]<<8 | rxmsg.data[3];
				if(addr_offset >= DATA_BASE)
				{
					uint32_t my_data = *(ack_data + (addr_offset - DATA_BASE));
					rxmsg.data[4] = my_data>>24;
					rxmsg.data[5] = my_data>>16;
					rxmsg.data[6] = my_data>>8;
					rxmsg.data[7] = my_data;
					
					Can_IV_Task_SendMsg(rxmsg.data ,8);				
				}
				
			}			
					
		}		
        else{
            int result;

            result = tcp_server_default_Send(rxmsg.data,8); //直接将数据发送给服务器
            if (result == -RT_ERROR)
            {
                LOG_D("rt_mq_send ERR\n");
            }
        }

        

    }
}
/**
 * @description: CAN????????????
 * @return void
 */
int Can_IV_Task_Init()
{
    rt_err_t res;
    /* ???? CAN ?őô */
    can_iv_dev = rt_device_find(CAN_IV_DEV_NAME);
    if (!can_iv_dev)
    {
        LOG_D("find %s failed!\n", CAN_IV_DEV_NAME);
        return RT_ERROR;
    }
    /* ????? CAN ????????? */
    rt_sem_init(&rx_iv_sem, "rx_iv_sem", 0, RT_IPC_FLAG_FIFO);

    /*??????????????*/
    rt_mutex_init(&_can_iv_tx_mutex, "can_iv_tx", RT_IPC_FLAG_FIFO);
    /* ???§Ř????????????? CAN ?őô */
    res = rt_device_open(can_iv_dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);
    RT_ASSERT(res == RT_EOK);
    /* ?????????????? */
    res =  rt_thread_init(&can_iv_rx_tcb,"can_iv_rx",can_iv_rx_thread,RT_NULL,can_iv_task_stack,CAN_IV_RX_TASK_SIZE,CAN_IV_RX_TASK_PRIO,10);
    RT_ASSERT(res == RT_EOK);
    rt_thread_startup(&can_iv_rx_tcb);  
    LOG_D("Can_IV_Task_Init INIT OK!!");
}
INIT_APP_EXPORT(Can_IV_Task_Init);


int Can_IV_Task_SendMsg(uint8_t *_msg,uint8_t _len)
{
#if 1
    struct rt_can_msg msg = {0};
    rt_size_t  size;
    rt_mutex_take(&_can_iv_tx_mutex,RT_WAITING_FOREVER);
    msg.id = IV_MODULE_CAN_ID;     /*???????????ID*/
    msg.ide = RT_CAN_STDID;     /* ?????? */
    msg.rtr = RT_CAN_DTR;       /* ????? */
    msg.len = _len;                /* ???????? 8 */
    /* ??????? 8 ??????? */
    rt_memcpy(msg.data,_msg,_len);

    /* ?????? CAN ???? */
    size = rt_device_write(can_iv_dev, 0, &msg, sizeof(msg));
    if (size == 0)
    {
        LOG_D("can_iv_dev write data failed!\n");
    }
    rt_mutex_release(&_can_iv_tx_mutex);
    return size;
#else
    return 0;
#endif
}

void Can_IV_Task_Control(uint32_t bottle_height)
{
    static CanData_t _data = {
        .id = EIGER_IV_MODULE_ID,
        .dir = 0X01,
        .addr = BSWAP_16(0X1001),   // IV 杆瓶高配置

    };
    _data.data = BSWAP_32(bottle_height);
    Can_IV_Task_SendMsg((uint8_t*)&_data,sizeof(_data));
}

int Can_IV_Task_OTA_Send(uint8_t *_msg,uint8_t _len)
{
    struct rt_can_msg msg = {0};
    rt_size_t  size;
    rt_mutex_take(&_can_iv_tx_mutex,RT_WAITING_FOREVER);
    msg.id = (0xFF-IV_MODULE_CAN_ID);     /*???????????ID*/
    msg.ide = RT_CAN_STDID;     /* ?????? */
    msg.rtr = RT_CAN_DTR;       /* ????? */
    msg.len = _len;                /* ???????? 8 */
    /* ??????? 8 ??????? */
    rt_memcpy(msg.data,_msg,_len);

    /* ?????? CAN ???? */
    size = rt_device_write(can_iv_dev, 0, &msg, sizeof(msg));
    if (size == 0)
    {
        LOG_D("can_iv_dev write data failed!\n");
    }
    rt_mutex_release(&_can_iv_tx_mutex);
    return size;
}
