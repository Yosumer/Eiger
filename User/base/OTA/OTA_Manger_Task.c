#include "OTA_Manger_Task.h"
#include "OTA_Tcp_Client_Task.h"
#include "bsp_can_ymodem.h"
#define LOG_TAG            "base.OTA"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>

ota_manger_handler _ota;
static struct rt_thread _ota_tcb;
static struct rt_thread _ota_down_tcb;
#define     OTA_MANGER_TASK_SIZE       2048
#define     OTA_MANGER_UP_TASK_PRIO       12


#define     OTA_MANGER_DOWN_STACK_SIZE 512
#define     OTA_MANGER_DOWM_TASK_PRIO       (OTA_MANGER_UP_TASK_PRIO+1)

rt_uint8_t  ota_task_stack[OTA_MANGER_TASK_SIZE];

rt_uint8_t  ota_down_task_stack[OTA_MANGER_DOWN_STACK_SIZE];


rt_tcpclient_t  *ota_client = RT_NULL;
void  OTA_Manger_Task_Cmd(uint16_t _addr,uint32_t _data)
{
    if(_addr == (uint16_t)&_ota._WCof->_module)
    {
        if (_data == OTA_CMD_MODULE_EIGER)
        {
            OTA_Manger_Task_Set_Downstream_Interface(&_tcpdownstream_inface);
        }
        else if (_data == OTA_CMD_MODULE_IV)
        {
            OTA_Manger_Task_Set_Downstream_Interface(&_ivdownstream_inface);
        }
        else if (_data == OTA_CMD_MODULE_PEDAL)
        {
            OTA_Manger_Task_Set_Downstream_Interface(&_pedaldownstream_inface); 
        }
        else
        {
            LOG_E("OTA_Manger_Task_Cmd: Module Error");
        }
    }
    else if(_addr == (uint16_t)&_ota._WCof->_cmd)
    {
        if(_data == OTA_CMD_START)
        {
            //清空环形队列
            // OTA_Manger_Task_DeInit();
            _ota.downrecvbuf = rt_malloc(20);
            if (_ota.downrecvbuf == RT_NULL)
            {
                LOG_E("RT_MALLOC DOWNCEVBUF FAILD!!!");
            }
            
            _ota.uprecvbuf = rt_malloc(1100);
            if (_ota.uprecvbuf == RT_NULL)
            {
                LOG_E("RT_MALLOC uprecvbuf FAILD!!!");
            }
            rt_completion_done(&_ota._waitup_completion);  //完成等待
            rt_completion_done(&_ota._waitdown_completion);  //完成等待
        }
        else if (_data == OTA_CMD_STOP)
        {
            OTA_Manger_Task_Up_Rx_Notify(RT_NULL,0);  //通知上层完成升级
            OTA_Manger_Task_Down_Rx_Notify(RT_NULL,0);  //通知下层完成升级
        
        }
    }
}


static int32_t OTA_UPStream_Recv(uint8_t *buf)
{
    rt_completion_wait(&_ota._up_rx_completion,RT_WAITING_FOREVER);  //等待上位机完成
    rt_size_t len= rt_ringbuffer_data_len(_ota._uprx_txbuffer);  //获取数据长度
    rt_size_t get_len = rt_ringbuffer_get(_ota._uprx_txbuffer,buf,len);  //接收数据
    return get_len;  //返回接收数据长度
}

static int32_t OTA_UPStream_Send(uint8_t *buf,uint16_t len)
{
    rt_size_t send_len = rt_tcpclient_send(ota_client,buf,len);  //发送数据
    return send_len;  //返回发送数据长度
}

ota_com_interface _upstream_inface = {
    .ota_com_recv = OTA_UPStream_Recv,  //接收数据
    .ota_com_send = OTA_UPStream_Send,  //发送数据
};

static int32_t OTA_DownStream_Recv(uint8_t *buf)
{
    rt_completion_wait(&_ota._down_rx_completion,RT_WAITING_FOREVER);  //等待下位机完成
    rt_size_t len= rt_ringbuffer_data_len(_ota._downrx_txbuffer);  //获取数据长度
    rt_size_t get_len = rt_ringbuffer_get(_ota._downrx_txbuffer,buf,len);  //接收数据
    return get_len;  //返回接收数据长度
}

static int32_t OTA_TcpDownStream_Send(uint8_t *buf,uint16_t len)
{
    bsp_can_ymodem_recv(buf,len);  //发送数据
    return len;  //返回发送数据长度
}

ota_com_interface _tcpdownstream_inface = {
   .ota_com_recv = OTA_DownStream_Recv,  //接收数据
   .ota_com_send = OTA_TcpDownStream_Send,  //发送数据
};

static int32_t OTA_IVDownStream_Send(uint8_t *buf,uint16_t len)
{
    uint16_t _packet_cnt = len/8;
    uint8_t  _packet_remain = len % 8;   //理论上包的数为8的正数倍 

    for(uint16_t i = 0; i < _packet_cnt; i++) {
        int ret = Can_IV_Task_OTA_Send(&buf[i*8],8);   //发送8的正数
        if (i%20 == 0)
        {
            rt_thread_mdelay(1);  //延时10ms后重新发送
        }
        
    }
    // 发送最后一包
    Can_IV_Task_OTA_Send(&buf[_packet_cnt*8],_packet_remain);
    return len;
}

ota_com_interface _ivdownstream_inface = {
    .ota_com_recv = OTA_DownStream_Recv,  //接收数据
    .ota_com_send = OTA_IVDownStream_Send,  //发送数据
 };


static int32_t OTA_PedalDownStream_Send(uint8_t *buf,uint16_t len)
{
    uint16_t _packet_cnt = len/8;
    uint8_t  _packet_remain = len % 8;   //理论上包的数为8的正数倍 

    for(uint16_t i = 0; i < _packet_cnt; i++) {
        int ret = Can_Pedal_Task_OTA_Send(&buf[i*8],8);   //发送8的正数
        if (i%20 == 0)
        {
            rt_thread_mdelay(1);  //延时10ms后重新发送
        }
    }
    // 发送最后一包
    Can_Pedal_Task_OTA_Send(&buf[_packet_cnt*8],_packet_remain);
    return len;
}


 ota_com_interface _pedaldownstream_inface = {
    .ota_com_recv = OTA_DownStream_Recv,  //接收数据
    .ota_com_send = OTA_PedalDownStream_Send,  //发送数据
 };

//上位机接收数据处理函数
void OTA_Manger_UPStream_Task(void *parameter)
{
_start:
    rt_completion_wait(&_ota._waitup_completion,RT_WAITING_FOREVER);  //等待完成
_tryConnect:
    ota_client = rt_tcpclient_start("192.168.176.11",25091);  //连接服务器
    if(ota_client == RT_NULL)
    {
        LOG_E("OTA Connect Fail!");
        rt_thread_mdelay(100);  //延时100ms后重新连接
        goto _tryConnect;  //连接失败，重新连接
    }
    // 注册回调函数
    rt_tcpclient_attach_rx_cb(ota_client, OTA_Manger_Task_Up_Rx_Notify);  //注册接收回调函数

    // 设置上位机接收发送函数
    OTA_Manger_Task_Set_Upstream_Interface(&_upstream_inface);  //设置上位机接收发送函数
    while (1)
    {
        if(_ota._WCof->_cmd == OTA_CMD_START)
        {
            //1.从上行接口中获取数据
            if (_ota._upstreamInterface)
            {

                int size = _ota._upstreamInterface->ota_com_recv(_ota.uprecvbuf);  //接收数据
                if (_ota._downstreamInterface && size > 0)
                {
                    _ota._downstreamInterface->ota_com_send(_ota.uprecvbuf,size);  //发送数据
                }
            }
        }
        else
        {
            //2.升级中断
            if(_ota._upstreamInterface)
            {
                uint8_t buf[] = "ota abort!";
                _ota._upstreamInterface->ota_com_send(buf,sizeof(buf));
            }
            rt_free(_ota.uprecvbuf);
            if (_ota._uprx_txbuffer!= RT_NULL)
            {
                rt_ringbuffer_destroy(_ota._uprx_txbuffer);
                _ota._uprx_txbuffer = RT_NULL;
            }
            rt_tcpclient_close(ota_client);
            goto _start;
        }
    }
}
//下位机接收数据处理函数
void OTA_Manger_DownStream_Task(void *parameter)
{
_start_down:
        rt_completion_wait(&_ota._waitdown_completion,RT_WAITING_FOREVER);  //等待完成
        while (1)
        {
            if(_ota._WCof->_cmd == OTA_CMD_START)
            {
                //1.从下行接口中获取数据
                if (_ota._downstreamInterface)
                {
                    int size = _ota._downstreamInterface->ota_com_recv(_ota.downrecvbuf);  //接收数据 
                    if (_ota._upstreamInterface && size > 0)
                    {
                        _ota._upstreamInterface->ota_com_send(_ota.downrecvbuf,size);  //发送数据
                    }
                }
            }
            else
            {
                //2.升级中断
                // if(_ota._upstreamInterface)
                // {
                //     uint8_t buf[] = "ota abort!";
                //     _ota._upstreamInterface->ota_com_send(buf,sizeof(buf));
                // }
                rt_free(_ota.downrecvbuf);
                if(_ota._downrx_txbuffer != RT_NULL)
                {
                    rt_ringbuffer_destroy(_ota._downrx_txbuffer);
                    _ota._downrx_txbuffer = RT_NULL;
                }
                goto _start_down;  //中断升级，重新开始
            }

        }
}

int OTA_Manger_Task_Init(void)
{
    _ota._WCof = &ramctl._ota_cof;
    rt_completion_init(&_ota._waitup_completion);  //初始化等待完成
    rt_completion_init(&_ota._waitdown_completion);  //初始化等待完成
    rt_completion_init(&_ota._up_rx_completion);  //初始化等待完成
    rt_completion_init(&_ota._down_rx_completion);  //初始化等待完成
    Normal_Task_RegisterFun(OTA_Manger_Task_Cmd);
    int res =  rt_thread_init(&_ota_tcb,"ota_up",OTA_Manger_UPStream_Task,RT_NULL,ota_task_stack,OTA_MANGER_TASK_SIZE,OTA_MANGER_UP_TASK_PRIO,10);
    RT_ASSERT(res == RT_EOK);
    rt_thread_startup(&_ota_tcb);  
    LOG_D("OTA_Manger_UPStream_Task Start UP!!");
    res =  rt_thread_init(&_ota_down_tcb,"ota_dowm",OTA_Manger_DownStream_Task,RT_NULL,ota_down_task_stack,OTA_MANGER_DOWN_STACK_SIZE,OTA_MANGER_DOWM_TASK_PRIO,10);
    RT_ASSERT(res == RT_EOK);
    rt_thread_startup(&_ota_down_tcb);
    LOG_D("OTA_Manger_DownStream_Task Start UP!!");
}
INIT_APP_EXPORT(OTA_Manger_Task_Init);

int OTA_Manger_Task_DeInit(void)
{
    if(_ota._downrx_txbuffer != RT_NULL)
    {
        rt_ringbuffer_destroy(_ota._downrx_txbuffer);
    }
    if (_ota._uprx_txbuffer!= RT_NULL)
    {
        rt_ringbuffer_destroy(_ota._uprx_txbuffer);
    }

    
}
void OTA_Manger_Task_Set_Upstream_Interface(ota_com_interface *_upstream)
{
    RT_ASSERT(_upstream != RT_NULL);
    _ota._upstreamInterface = _upstream;
}
void OTA_Manger_Task_Set_Downstream_Interface(ota_com_interface *_downstream)
{
    RT_ASSERT(_downstream!= RT_NULL);
    _ota._downstreamInterface = _downstream;
}


void OTA_Manger_Task_Up_Rx_Notify(uint8_t *buf,uint16_t len)
{
    if (_ota._uprx_txbuffer == RT_NULL)
    {
        _ota._uprx_txbuffer = rt_ringbuffer_create(1100);
        if (_ota._uprx_txbuffer == RT_NULL)
        {
            LOG_E("OTA_Manger_Task_Up_Rx_Notify: Create RingBuffer Fail!");
            return;
        }
        
    }
    
    rt_ringbuffer_put(_ota._uprx_txbuffer,buf,len);  //将数据放入环形缓冲区
    rt_completion_done(&_ota._up_rx_completion);  //完成等待
}
void OTA_Manger_Task_Down_Rx_Notify(uint8_t *buf,uint16_t len)
{
    if (_ota._downrx_txbuffer == RT_NULL)
    {
        _ota._downrx_txbuffer = rt_ringbuffer_create(100);
        if (_ota._downrx_txbuffer == RT_NULL)
        {
            LOG_E("OTA_Manger_Task_Down_Rx_Notify: Create RingBuffer Fail!");
            return;
        }
    }
    rt_ringbuffer_put(_ota._downrx_txbuffer,buf,len);  //将数据放入环形缓冲区
    rt_completion_done(&_ota._down_rx_completion);  //完成等待
}


