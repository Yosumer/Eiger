/*------------------------------------------------------------------------------
 * @file    TASK_START.C
 * @author  FIRMWARE TEAM 
 * @date    2024/04/09 10:27:02
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
#include <rtthread.h>
#include "includes.h"
#define LOG_TAG            "task.Start"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>
/*全局变量初始化-------------------------------------------------------------------*/
CAN_CONTROL Control_Block ramctl = {0};           //全局控制块
CAN_ERROR Error_Block ramerr;               //全局错误块
CAN_CONFIG Config_Block  ramcof;            //全局配置块
CAN_DATA Data_Block  ramdata = {0};         //全局数据块
Flash_Block ramflash __attribute__((at(FLASH_ADDR)));     //全局Flash模块
NoInit_Block ramNoInit __attribute__((at(0x2001F100)));

// Flash_Fw flash_fw1;              //flash固件1信息
// Flash_Fw flash_fw2;              //flash固件2信息
// Flash_DeviceInfo flash_devinfo;  //设备信息

//当前版本号
#define APP_VERSION                     "1.0.0"



/*信号量初始化-------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*/

/*消息队列初始化-------------------------------------------------------------------*/
struct rt_messagequeue downLD_mq;       //下行数据流
rt_uint8_t downLD_pool[2048];   //消息队列池

OS_Q      GLOBAL_Msg;                     //全局使能消息队列

/*----------------------------------------------------------------------------*/
static int  AppConfig_Start(void);
static int AppIPC_Init(void);
static int AppTask_Create(void);
static int AppQpcTask_Create(void);



/**
 * @description: APP相关环境初始化
 * @return {*}
 */
int AppConfig_Start(void)
{
    /*硬件相关初始化(手动)-------------------------------------------------------------------*/



    /*-------------------------------------------------------------------------------------*/

    /*参数加载参数配置-------------------------------------------------------------------*/
	ramctl.enable = 0;
	ramdata.mode = NORMAL_MODE;
	ramcof.config = NORMAL_MODE;
	
	/*Version Read*/
	ramdata.fw1.data.FWversion1 = 0x56; //"V"
	ramdata.fw1.data.FWversion2 = 0x1;
	ramdata.fw1.data.FWversion3 = 0x0;
	ramdata.fw1.data.FWversion4 = 0x1;
	ramdata.fw1.data.FWversion5 = 16;
	
  ramNoInit._updateflag = 0;  //进入正常APP后将升级标志位置0
	LOG_D("_updateflag:%d",ramNoInit._updateflag);
    /*-------------------------------------------------------------------------------------*/
}
INIT_PREV_EXPORT(AppConfig_Start);
/**
 * @description: APP任务信号量、消息队列等初始化
 * @return {*}
 */
static int AppIPC_Init(void)
{
    rt_err_t result;
	OS_ERR err;
    result = rt_mq_init(&downLD_mq,
                        "downLD_mq",
                        &downLD_pool[0],             /* 内存池指向 msg_pool */
                        8,                          /* 每个消息的大小是 1 字节 */
                        sizeof(downLD_pool),        /* 内存池的大小是 msg_pool 的大小 */
                        RT_IPC_FLAG_PRIO);       /* 如果有多个线程等待，优先级大小的方法分配消息 */

    if (result != RT_EOK)
    {
        LOG_D("downLD  message queue failed.\n");
        return -1;
    }
	OSQCreate ((OS_Q*		)&GLOBAL_Msg,	
                (CPU_CHAR*	)"GLOBAL Msg",	
                (OS_MSG_QTY	)GLOBALMsg_Q_NUM,	
                (OS_ERR*	)&err);
}
INIT_ENV_EXPORT(AppIPC_Init);


//static void idle_hook(void)
//{
//    /* 在空闲线程的回调函数里喂狗 */
//    rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
////    rt_kprintf("feed the dog!\n ");
//}

/**
 * @description: APP任务初始化
 * @return {*}
 */
static int AppTask_Create(void)
{
	/* 输出当前版本 */
    LOG_I("The current version of App Firmware is %s",APP_VERSION);
}
INIT_APP_EXPORT(AppTask_Create);


// initialize event pools


QF_MPOOL_EL(ErigeEvt) smlPoolSto[100];
/**
 * @description: QPC任务初始化
 * @return {*}
 */
static int AppQpcTask_Create(void)
{

    QF_init(); /* initialize the framework */
    QF_poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));
    // initialize publish-subscribe
    static QSubscrList subscrSto[20*MAX_PUB_SIG];
    QActive_psInit(subscrSto, Q_DIM(subscrSto));

    QF_run(); /* run the QF application */

}
INIT_ENV_EXPORT(AppQpcTask_Create);



