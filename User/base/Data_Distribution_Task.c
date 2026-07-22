
/*------------------------------------------------------------------------------
 * @file    DATA_DISTRIBUTION_TASK.C
 * @author  FIRMWARE TEAM 
 * @date    2024/04/09 09:45:11
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
#define LOG_TAG            "task.DataDistribution"
#define LOG_LVL            LOG_LVL_INFO
#include <ulog.h>


static Data_Distribution_t _Distribution = {0};  //数据分发结构体

rt_uint8_t  dataDistribution_task_stack[DATA_DISTRIBUTION_TASK_SIZE];
struct rt_thread dataDistribution_tcb;  //数据分发任务分发句柄

extern struct rt_messagequeue downLD_mq;       //下行数据流


struct rt_ringbuffer* _data_ringbuf = RT_NULL;   //环形缓冲区
/*----------------------------------------------------------------------------*/

static int32_t Address_Translation(CanData_t can,uint32_t *_tr_addr)
{
    uint32_t tr_addr;
    if(can.addr<MAX_BASE && can.addr >=DATA_BASE)   //数据块地址转换
    {
        uint32_t _maxAddr = (uint32_t)&ramdata._unuse;
        LOG_D("RAMData MaxAddr:%x",_maxAddr);
        // 数据块中读写没有特别操作
        tr_addr =  ((can.addr-DATA_BASE)*4 + (DATA_BASE+OFFSET_BASE))+RAM2_BASE;
        // 判断地址是否超过数据块的最大地址
        if(tr_addr >= _maxAddr)  //访问了非法地址
        {
            LOG_E("RAMData Access illegal address");
            return -1;
        }
    }
    else if(can.addr<DATA_BASE && can.addr >=CONFIG_BASE)   //配置块地址转换
    {
        uint32_t _maxAddr = (uint32_t)&ramcof._unuse;
        LOG_D("RAMCof MaxAddr:%x",_maxAddr);
        // 配置块中读写没有特别操作
       tr_addr =  ((can.addr-CONFIG_BASE)*4 + CONFIG_BASE+OFFSET_BASE)+RAM2_BASE;
       if(tr_addr >= _maxAddr)  //访问了非法地址
       {
           LOG_E("RAMCof Access illegal address");
           return -1;
       }

    }
    else if(can.addr<FLASH_RAM_BASE && can.addr >=ERROR_BASE)   //错误块地址转换
    {
        //错误块需要进行特别的操作所以需要特殊的处理过程 
        tr_addr =  ((can.addr-ERROR_BASE)*4 + ERROR_BASE)+RAM2_BASE;
    }
    else if(can.addr<CONFIG_BASE && can.addr >=FLASH_RAM_BASE)
    {
        //Flash模式需要进行特别的操作所以需要特殊的处理过程 
        tr_addr =  ((can.addr-FLASH_RAM_BASE)*4 + FLASH_RAM_BASE)+RAM2_BASE;
    }
    else if(can.addr<ERROR_BASE && can.addr >=CONTROL_BASE)   //控制块地址转换
    {
        uint32_t _maxAddr = (uint32_t)&ramctl._unuse;
        LOG_D("RAMCtl MaxAddr:%x",_maxAddr);
        tr_addr =  ((can.addr-CONTROL_BASE)*4 + CONTROL_BASE)+RAM2_BASE;
        if(tr_addr >= _maxAddr)  //访问了非法地址
        {
            LOG_E("RAMCtl Access illegal address");
            return -1;
        }
    }
    else if(can.addr<CONTROL_BASE && can.addr >=0)  //FLASH模式中加载地址使用
    {
        tr_addr =  (can.addr*4)+RAM2_BASE;
    }
    else    //非法地址
    {
		LOG_E("Access illegal address");
        return -1; 
    }

    *_tr_addr = tr_addr;

    //这里读写下位机就直接操作 默认上位机不会进行非法访问    
    //判断读写
    if(can.dir == 0x01) //写操作
    {

        // 原子操作写32位
        // ATOMIC_WRITE(tr_addr,can.data);
       App_Com_WriteRam((uint32_t *)tr_addr,can.data);
       if(can.addr<FLASH_RAM_BASE && can.addr >=ERROR_BASE)    //写操作错误块需要特殊处理
       {
           // 判断地址是否合法
           if(can.addr == ERROR_BASE)
           {
               if(can.data == 0)   // 加载上一页方法
               {

                   // memcpy(ramerr.errorbuf,bsp_error_getPrevious(),sizeof(ramerr.errorbuf));
               }
               else if(can.data == 1)  // 加载下一页方法
               {
                   // memcpy(ramerr.errorbuf,bsp_error_getNext(),sizeof(ramerr.errorbuf));
               }
               else    //非法操作
               {
                   return -1;

               }
           }
           else
           {
               LOG_E("错误块操作非法地址写\r\n");
               return -1;
           }
       }
       can.addr = BSWAP_16(can.addr);
       can.data = BSWAP_32(can.data);
       if(ramctl.bootmode !=1)
            Data_Distribution_SendMsg((uint8_t*)&can , 8);
    }
    else if (can.dir == 0x02) //读操作
    {
        // can.data = *((uint32_t*)(tr_addr));
        // 原子操作读32位
//        can.data =  __LDREXW((__IO uint32_t *)(tr_addr));
        can.data = rt_hw_atomic_load((rt_atomic_t*)tr_addr);
        if(can.addr<ERROR_BASE && can.addr >=CONTROL_BASE) //控制块的读操作全部需要返回0
        {
            can.data = 0;
        }
        else if(can.addr<CONFIG_BASE && can.addr >=ERROR_BASE)  //错误块
        {
           if(can.addr == ERROR_BASE)
           {
                App_Com_WriteRam((uint32_t *)tr_addr,bsp_error_list_pop()); //先写
                can.data = rt_hw_atomic_load((rt_atomic_t*)tr_addr);
           }
        }
        // 将读的数据返回
        // 1.大小端转换
        LOG_D("raw addr:%x\r\n",can.addr);
        LOG_D("raw data:%x\r\n",can.data);
        can.addr = BSWAP_16(can.addr);
        can.data = BSWAP_32(can.data);
        LOG_D("translation addr:%x\r\n",can.addr);
        LOG_D("translation data:%x\r\n",can.data);    

        // app_send_data((uint8_t*)&can , 8);
        Data_Distribution_SendMsg((uint8_t*)&can , 8);
    }
    else
    {
        // 非法指令
    }
	return 0;
}
// void Data_Distribution_Task(void *parameter)
// {
//     CanData_t candata;
//     uint32_t tr_addr;
//     OS_ERR  err;
//     while (1)
//     {
//        /* 从消息队列中接收消息 */
//        if (rt_mq_recv(&downLD_mq, &candata, sizeof(candata), RT_WAITING_FOREVER) == RT_EOK)
//        {
//             //1.进行大小端转换
//             LOG_D("raw addr:%x",candata.addr);
//             LOG_D("raw data:%x",candata.data);
//             candata.addr = BSWAP_16(candata.addr);
//             candata.data = BSWAP_32(candata.data);
//             LOG_D("translation addr:%x\r\n",candata.addr);
//             LOG_D("translation data:%x\r\n",candata.data);
//             switch (candata.id)
//             {
//                 case CURRENT_MODULE_ID: //当前模块需要根据自己模块进行定义
//                 {
//                     // 进行地址翻译后处理数据
//                     if(Address_Translation(candata,&tr_addr) == -1) //非法指令
//                     {
//                         candata.addr = BSWAP_16(candata.addr);
//                         candata.data = BSWAP_32(candata.data);
//                         candata.data = 0XFDFDFDFD;
//                         //无论是读还是写指令 非法指令数据段返回0XFEFE
//                         Data_Distribution_SendMsg((uint8_t*)&candata , 8);
//                         break;
//                     }
//                     //该功能块用来将命令发送到具体的功能块
//                     if(candata.dir == 1)        //只有写操作才会发送给各个任务
//                     {
//                         LOG_D("Current Access Address:%x",tr_addr-RAM2_BASE);
//                         candata.addr = tr_addr; //这里将地址转换为ram配置表寄存器
//                         //指令正确进行的操作
//                         switch (ramcof.config) {
//                             case NORMAL_MODE:   //正常模式
//                             {
//                               OSTaskQPost(&Normal_TaskTCB,(void *)&candata,\
//                                   (OS_MSG_SIZE)8,OS_OPT_POST_FIFO,&err);
//                             }
//                                 break;
//                             case FLASH_MODE:    //Flash模式
//                             {

//                               OSTaskQPost(&Flash_TaskTCB,(void *)&candata,\
//                                   (OS_MSG_SIZE)8,OS_OPT_POST_FIFO,&err);
//                             }
//                                 break;
//                             case PROJECT_MODE:  //工程模式
//                             {

//                               OSTaskQPost(&Engineering_TaskTCB,(void *)&candata,\
//                                   (OS_MSG_SIZE)8,OS_OPT_POST_FIFO,&err);
//                             }
//                                 break;
//                             default:
//                                 LOG_E("The current module is in undefined mode");
//                                 break;

//                         }


//                     }
//                     break;
//                 }
								
// 								 break;
// #ifdef  USE_PEDAL_MODE 
//                 case PEDAL_MODULE_CtlID:
//                 {
                    
//                     candata.addr = BSWAP_16(candata.addr);
//                     candata.data = BSWAP_32(candata.data);

//                     if (ramdata._cassette_state._pen_is_connect)    //踏板连接后才转发
//                     {
//                         Can_Task_SendMsg((uint8_t*)&candata,8); //转发踏板数据
//                     }
//                 }
//                 break;
//                 case PEDAL_MODULE_ID:
//                 {
//                     OSTaskQPost(&Pedal_TaskTCB,(void *)&candata,\
//                         (OS_MSG_SIZE)8,OS_OPT_POST_FIFO,&err);
//                 }
//                 break;
// #endif

//                 case EIGER_IV_MODULE_ID:
//                 {
//                     candata.addr = BSWAP_16(candata.addr);
//                     candata.data = BSWAP_32(candata.data);
//                     if (ramdata._cassette_state._iv_is_connect) //IV杆连接后才转发
//                     {
//                         Can_IV_Task_SendMsg((uint8_t*)&candata,8); //转发IV杆数据
//                     }
//                 }
//                 break;
//                 default:
//                     break;
//             }
//        }

//     }
// }


void Data_Distribution_Task(void *parameter)
{
    CanData_t *candata = rt_calloc(1,4);
    CanData_t _revcandata;
    uint32_t tr_addr;
    OS_ERR  err;
    uint8_t _len = sizeof(_revcandata);
    while (1)
    {
       /* 从消息队列中接收消息 */
       if (rt_mq_recv(&downLD_mq, &_revcandata, sizeof(_revcandata), RT_WAITING_FOREVER) == RT_EOK)
       {
            // 接收到消息后处理逻辑
            rt_ringbuffer_put(_data_ringbuf,(uint8_t*)&_revcandata,_len);  //将数据放入环形缓冲区
            rt_ringbuffer_get(_data_ringbuf,(uint8_t*)candata,_len);

            //1.进行大小端转换
            LOG_D("raw addr:%x",candata->addr);
            LOG_D("raw data:%x",candata->data);
            candata->addr = BSWAP_16(candata->addr);
            candata->data = BSWAP_32(candata->data);
            LOG_D("translation addr:%x\r\n",candata->addr);
            LOG_D("translation data:%x\r\n",candata->data);

        

            switch (candata->id)
            {
                case CURRENT_MODULE_ID: //当前模块需要根据自己模块进行定义
                {
                    // 进行地址翻译后处理数据
                    if(Address_Translation(*candata,&tr_addr) == -1) //非法指令
                    {
                        candata->addr = BSWAP_16(candata->addr);
                        candata->data = BSWAP_32(candata->data);
                        candata->data = 0XFDFDFDFD;
                        //无论是读还是写指令 非法指令数据段返回0XFEFE
                        Data_Distribution_SendMsg((uint8_t*)candata , 8);
                        break;
                    }
                    //该功能块用来将命令发送到具体的功能块
                    if(candata->dir == 1)        //只有写操作才会发送给各个任务
                    {
                        LOG_D("Current Access Address:%x",tr_addr-RAM2_BASE);
                        candata->addr = tr_addr; //这里将地址转换为ram配置表寄存器
                        //指令正确进行的操作
                        switch (ramcof.config) {
                            case NORMAL_MODE:   //正常模式
                            {
                              OSTaskQPost(&Normal_TaskTCB,(void *)candata,\
                                  (OS_MSG_SIZE)8,OS_OPT_POST_FIFO,&err);
                            }
                                break;
                            case FLASH_MODE:    //Flash模式
                            {

                              OSTaskQPost(&Flash_TaskTCB,(void *)candata,\
                                  (OS_MSG_SIZE)8,OS_OPT_POST_FIFO,&err);
                            }
                                break;
                            case PROJECT_MODE:  //工程模式
                            {

                              OSTaskQPost(&Engineering_TaskTCB,(void *)candata,\
                                  (OS_MSG_SIZE)8,OS_OPT_POST_FIFO,&err);
                            }
                                break;
                            default:
                                LOG_E("The current module is in undefined mode");
                                break;

                        }
                    }
                    break;
                }
								
				break;
#ifdef  USE_PEDAL_MODE 
                case PEDAL_MODULE_CtlID:
                {
                    candata->addr = BSWAP_16(candata->addr);
                    candata->data = BSWAP_32(candata->data);
										Can_Task_SendMsg((uint8_t*)candata,8); //转发踏板数据
                }
                break;
                case PEDAL_MODULE_ID:
                {
                    OSTaskQPost(&Pedal_TaskTCB,(void *)candata,\
                        (OS_MSG_SIZE)8,OS_OPT_POST_FIFO,&err);
                }
                break;
#endif
                case EIGER_IV_MODULE_ID:
                {
                    candata->addr = BSWAP_16(candata->addr);
                    candata->data = BSWAP_32(candata->data);
                    if (ramdata._cassette_state._iv_is_connect) //IV杆连接后才转发
                    {
                        Can_IV_Task_SendMsg((uint8_t*)candata,8); //转发IV杆数据
                    }
                }
                break;
                default:
                    break;
            }
       }

    }
}


static struct rt_mutex _data_tx_mutex;
/**
 * @description: 数据分发任务发送消息 供其它任务使用
 * @param {uint8_t*}_buf
 * @param {uint8_t}_size
 * @return {void}
 */
void Data_Distribution_SendMsg(uint8_t* _buf,uint8_t _size)
{
    RT_ASSERT(_Distribution._send); //检查是否设置了发送函数
    rt_mutex_take(&_data_tx_mutex,RT_WAITING_FOREVER);
    _Distribution._send(_buf,_size);
    rt_mutex_release(&_data_tx_mutex);
}





/**
 * @description: 设置数据分发任务通信模式
 * @param {uint8_t} _mode    COM_USE_CAN、COM_USE_ZIGBEE
 * @return {void}
 */
void Data_Distribution_CtlMode(uint8_t _mode)
{
    rt_base_t _level;
    
    _level=rt_hw_interrupt_disable();      //关闭中断
    rt_mutex_init(&_data_tx_mutex, "data_tx", RT_IPC_FLAG_FIFO);
    switch(_mode)
    {
        case COM_USE_CAN:
        {
            _Distribution._mode = COM_USE_CAN;
            _Distribution._send = Can_Task_SendMsg;
        }
        break;
        case COM_USE_ZIGBEE:
        {
            _Distribution._mode = COM_USE_ZIGBEE;
            _Distribution._send = NULL;
        }
        break;
        case COM_USE_BOOT:
        {
            _Distribution._mode = COM_USE_BOOT;
            _Distribution._send = Shell_Boot_SendMsg;
        }
        break;
        case COM_USE_TCP:
        {
            _Distribution._mode = COM_USE_TCP;
            _Distribution._send = tcp_server_default_Send;
        }
        break;
        default:
        {
            _Distribution._mode = COM_USE_CAN;
            _Distribution._send = Can_Task_SendMsg;
        }
        break;
    }
    rt_hw_interrupt_enable(_level);
}

// /**
//  * @description: 数据分发任务初始化
//  * @param {uint8_t} _ch
//  * @return {int} 0:OK -1 Fail
//  */
// int Data_Distribution_Init(void)
// {
//     rt_err_t res;
//     /*step1 set communication mode*/
//     Data_Distribution_CtlMode(COM_USE_TCP); //默认使用can进行通信

//     /* 创建数据接收线程 */

//     res =  rt_thread_init(&dataDistribution_tcb,
//                         "Data_Distribution_Task",
//                         Data_Distribution_Task,
//                         RT_NULL,
//                         dataDistribution_task_stack,
//                         DATA_DISTRIBUTION_TASK_SIZE,
//                         DATA_DISTRIBUTION_TASK_PRIO,
//                         10);
//     RT_ASSERT(res == RT_EOK);
//     rt_thread_startup(&dataDistribution_tcb);
//     LOG_D("Data_Distribution_Task INIT OK!!");
// 	return 0;
// }
// INIT_APP_EXPORT(Data_Distribution_Init);

/**
 * @description: 数据分发任务初始化
 * @param {uint8_t} _ch
 * @return {int} 0:OK -1 Fail
 */
int Data_Distribution_Init(void)
{
    rt_err_t res;
    /*step1 set communication mode*/
    Data_Distribution_CtlMode(COM_USE_TCP); //默认使用can进行通信
    
    _data_ringbuf =  rt_ringbuffer_create(512); 
    RT_ASSERT(_data_ringbuf !=RT_NULL);
    /* 创建数据接收线程 */

    res =  rt_thread_init(&dataDistribution_tcb,
                        "Data_Distribution_Task",
                        Data_Distribution_Task,
                        RT_NULL,
                        dataDistribution_task_stack,
                        DATA_DISTRIBUTION_TASK_SIZE,
                        DATA_DISTRIBUTION_TASK_PRIO,
                        10);
    RT_ASSERT(res == RT_EOK);
    rt_thread_startup(&dataDistribution_tcb);
    LOG_D("Data_Distribution_Task INIT OK!!");
	return 0;
}
INIT_APP_EXPORT(Data_Distribution_Init);

