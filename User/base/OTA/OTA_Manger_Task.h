

#ifndef OTA_MANGER_TASK_H
#define OTA_MANGER_TASK_H


// 包含必要的头文件
#include <stdint.h>  // 引入stdint.h头文件，包含有有符号整型的定义
#include <ipc/ringbuffer.h>
#include <ipc/completion.h>
#include "includes.h"

#define OTA_CMD_START     0x01    //开始升级
#define OTA_CMD_STOP      0x02    //停止升级


#define OTA_CMD_MODULE_EIGER    0xE5    //Eiger模块
#define OTA_CMD_MODULE_IV       0xE6    //IV杆模块
#define OTA_CMD_MODULE_PEDAL    0x0D    //脚踏模块

#define OTA_BUFF_SIZE   2048


typedef struct _ota_com_interface{
    int32_t (*ota_com_recv)(uint8_t *buf);  //接收数据
    int32_t (*ota_com_send)(uint8_t *buf,uint16_t len);  //发送数据
}ota_com_interface;



typedef struct _ota_manger_handler{
    uint8_t * uprecvbuf;
    uint8_t * downrecvbuf;
    OTA_Task_Cof *_WCof;    //升级配置
    ota_com_interface *_upstreamInterface;  //上层接口
    ota_com_interface *_downstreamInterface;  //下层接口
    struct rt_ringbuffer   *_uprx_txbuffer;     //上层接收缓冲区
    struct rt_ringbuffer   *_downrx_txbuffer;  //下层接收缓冲区
    struct rt_completion   _waitup_completion;  //等待完成
    struct rt_completion   _waitdown_completion;  //等待完成
    struct rt_completion	_up_rx_completion;
    struct rt_completion	_down_rx_completion;//下层接收完成
}ota_manger_handler;


extern ota_com_interface _tcpdownstream_inface;
extern ota_com_interface _ivdownstream_inface;
extern ota_com_interface _pedaldownstream_inface;
int OTA_Manger_Task_Init(void);
int OTA_Manger_Task_DeInit(void);
void OTA_Manger_Task_Set_Upstream_Interface(ota_com_interface *_upstream);
void OTA_Manger_Task_Set_Downstream_Interface(ota_com_interface *_downstream);


void OTA_Manger_Task_Up_Rx_Notify(uint8_t *buf,uint16_t len);
void OTA_Manger_Task_Down_Rx_Notify(uint8_t *buf,uint16_t len);

#endif // OTA_MANGER_TASK_H
