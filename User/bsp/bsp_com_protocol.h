/*------------------------------------------------------------------------------
 * @file    BSP_CAP_PROTOCOL.H
 * @author  SIERRA TEAM 
 * @date    2023/12/28 10:03:12
 * @brief   THIS FILE PROVIDES ALL THE SIERRA FUNCTIONS.
 * -----------------------------------------------------------------------------
 * @attention 
 
 * THE EXISTING SIERRA IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE 
 * CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE 
 * TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR 
 * CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH 
 * HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN 
 * CONNECTION WITH PRODUCTS MADE BY CUSTOMERS. 
 * 
 * <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2> 
------------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion  ------------------------------------*/
#ifndef __BSP_COM_PROTOCOL_H
#define __BSP_COM_PROTOCOL_H

/* Files includes  -----------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#define MAX_RX_LENG                                  30              //接收缓冲数组最大长度
#define MAX_TX_LENG                                  MAX_RX_LENG
#define PROTOCOL_HEAD                                0XFA            //帧头
#define PROTOCOL_FIXED1                              0XAF            //固定1
#define PROTOCOL_FIXED2                              0XA5            //固定2
#define PROTOCOL_FIXED3                              0X5A            //固定3
#define PROTOCOL_TAIL                                0X75            //帧尾
typedef struct _protocol_ops {
    void (*send_data)(uint8_t *data,uint16_t len);          //发送函数需要上层应用软件进行配置
    uint8_t (*read_byte)(void);                             //接收回调函数
    void (*decode_completion)(uint8_t *data,uint8_t len);   //接收完成回调函数 
}protocol_com_ops;
// 定义状态枚举用于协议解析
typedef enum {
    PROTOCOL_STATE_IDLE,    //接收空闲状态
    PROTOCOL_STATE_FIX1,    //接收固定字节1状态
    PROTOCOL_STATE_FIX2,    //接收固定字节2状态
    PROTOCOL_STATE_START,   //接收数据开始状态
    PROTOCOL_STATE_REAL_DATA,    //接收数据状态
} protocol_com_state;    //协议接收状态
#pragma pack(1)
typedef struct 
{
    uint8_t rxIndex;
    protocol_com_state protocolState;
    uint8_t packflag;   //接收完成标志
    union 
    {
        uint8_t allbyte[MAX_RX_LENG];
        struct 
        {
            uint8_t    head;            //帧头
            uint8_t    fix1;            //固定字节1
            uint8_t    fix2;            //固定字节2
            uint8_t    fix3;            //固定字节3
            uint8_t    len;        //用户通信协议自定义的第一个数据        
        }part;
    }pack;
}protocol_com_recive;
// 字符	转意后
// 0x7A	0x77 0x7A
// 0x75	0x77 0x75
// 0x77	0x77 0x77
#pragma pack(1)
typedef struct 
{
    union 
    {
        uint8_t sendbyte[MAX_TX_LENG];
        struct 
        {
            uint8_t    head;       //帧头
            uint8_t    fix1;       //固定字节1
            uint8_t    fix2;       //固定字节2
            uint8_t    fix3;       //固定字节3
            uint8_t    len;         //发送数据包的第一个数据
        }part;
    }pack;
}protocol_com_send;

typedef struct _protocol_com_t{
    uint8_t flag;       //是否使能转义功能 1:使能 0:不使能
    protocol_com_send      sendpack;    //发送协议
    protocol_com_recive    readpack;    //接收协议
    protocol_com_ops *ops;
    uint8_t rawsendbyte[MAX_TX_LENG];   //转义发送缓存区
}protocol_com_t;

void bsp_protocol_com_Init(protocol_com_t *_this,uint8_t flag);
void bsp_protocol_com_IRQHandler(protocol_com_t *_this);
void bsp_protocol_com_Task(protocol_com_t *_this);
void bsp_protocol_com_SendConf(protocol_com_t *_this,uint8_t *usedata);
#endif
