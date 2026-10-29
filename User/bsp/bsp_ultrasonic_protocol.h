/*** 
 * @Author: sum 476328608@qq.com
 * @Date: 2023-10-25 16:41:25
 * @LastEditors: sum 476328608@qq.com
 * @LastEditTime: 2024-07-15 17:00:47
 * @FilePath: \patagonia\light\User\bsp\opt\bsp_ultrasonic_protocol.h
 * @Description: 超声驱动通信协议
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef _BSP_ULTRASONIC_PROTOCOL_H
#define _BSP_ULTRASONIC_PROTOCOL_H
#include <stdint.h>
#define MAX_ULTRASONICPROTOCOL_LENG                    30
#define ULTRASONIC_HEAD                                0X7A            //帧头
#define ULTRASONIC_FIXED1                              0X01            //固定1
#define ULTRASONIC_FIXED2                              0X01            //固定2
#define ULTRASONIC_FIXED3                              0X08            //固定3
#define ULTRASONIC_TAIL                                0X75            //帧尾
typedef struct _ult_ops {
    void (*send_data)(uint8_t *data,uint16_t len);  //发送回调函数
    uint8_t (*read_byte)(void); //接收回调函数
    void (*decode_completion)(void *_this, uint8_t *data,uint8_t len);
}ult_ops;
// 定义状态枚举用于协议解析
typedef enum {
    ULT_IDLE,
    ULT_FIX1,
    ULT_FIX2,
    ULT_START,
    ULT_DATA,
} rev_state;    //协议接收状态
/* 超乳终端电路 错误类型*/
typedef enum{
	ULT_ERR_CRC 		= 1<<0,
	ULT_ERR_MEM 		= 1<<1,
	ULT_ERR_PWR 		= 1<<2,
	ULT_ERR_VOLT 		= 1<<3,
	ULT_ERR_CURR 		= 1<<4,
    ULT_ERR_TIME 		= 1<<5,
    ULT_ERR_FREQ 		= 1<<6,
    ULT_ERR_TURE 		= 1<<7,
    ULT_ERR_PHASE 		= 1<<8,
    ULT_ERR_UNDECTED 	= 1<<9,
    ULT_ERR_EEPROM 		= 1<<10,
    ULT_ERR_SENSOR		= 1<<11,
    ULT_ERR_IMPEDANCE	= 1<<12,
    ULT_ERR_TRANSDUCTOR	= 1<<13,	
    ULT_ERR_NEEDLE 		= 1<<14,	
} recv_error_bit_t;
/* 超乳终端电路  发送帧命令格式*/
typedef enum{
	ULT_CTRL_1 			= 1,
	ULT_CTRL_2 			= 2,
	ULT_CTRL_3 			= 3,
	ULT_CTRL_4 			= 4,
    ULT_CTRL_PORT1      = 0x91,     //切换port1口
    ULT_CTRL_PORT2      = 0X92,     //切换port2口
    ULT_CTRL_CLE_ERROR  = 0XA0,     //清除错误
    ULT_CTRL_CLE_SUMPWR = 0XB0,     //清除累计总功率
} cmd_ctl_t;    //功率控制指令是组合指令

typedef enum{
    ULT_QUERY_R1            = 0x50,        //查询r1值
	ULT_QUERY_C0		    = 0x51,        //查询c0
	ULT_QUERY_FREQ			= 0x52,        //查询频率
	ULT_QUERY_CURR			= 0x53,        //查询电流
	ULT_QUERY_VOLT			= 0x54,        //查询电压
	ULT_QUERY_PWR			= 0x55,        //查询功率
    ULT_QUERY_SUMPWR        = 0x56,        //累计总功率
    ULT_QUERY_VERSION        = 0x57,        //读取版本
    ULT_QUERY_HANDLE_DET_STA = 0x58,       //读取扫频结果
    ULT_QUERY_FPGA_VERSION = 0x59,          //读取FPGA版本
    ULT_QUERY_Long_Qm        = 0x5A, 
    ULT_QUERY_Tor_Qm        = 0x5B, 
    ULT_SET_LONG_FREQ        =0x60,         //设置纵振中心频率 
    ULT_SET_TORSION_FREQ     =0x61,         //设置扭振中心频率 
    ULT_SET_C0               =0x62,         //设置C0 
    ULT_SET_LONG_R1          =0x63,         //设置纵振R1 
    ULT_SET_TORSION_R1       =0x64,         //设置扭振R1           
    ULT_QUERY_PARMS         = 0X70,         //查询参数
    ULT_QUERY_ERROR         = 0X80,          //查询异常
    ULT_DEBUG_SCANFREQ        = 0XC0,          //扫频命令
    ULT_DEBUG_CLR_ERROR       = 0XC1,          //错误清除命令
    ULT_DEBUG_MODE            = 0XC2,          //模式命令
    ULT_DEBUG_MOS_VOLT        = 0XC3,         //设置MOS直流偏置命令
    ULT_DEBUG_FIXED_FREQ      = 0XC4,          //设置dds频率定频工作
    ULT_DEBUG_GAIN            = 0XC5,          //设置增益
    ULT_DEBUG_LENG_SCANFREQ   = 0XC6,          //单纵振扫描
    ULT_DEBUG_TORSION_SCANFREQ  = 0XC7,          //单扭振扫描
	ULT_DEBUG_LONG_C0  = 0XC8,
	ULT_DEBUG_LONG_R1  = 0XC9,
	ULT_DEBUG_LONG_FS  = 0XCA,
	ULT_DEBUG_LONG_QM  = 0XCB,
	ULT_DEBUG_TOR_C0  = 0XCC,
	ULT_DEBUG_TOR_R1  = 0XCD,
	ULT_DEBUG_TOR_FS  = 0XCE,
	ULT_DEBUG_TOR_QM  = 0XCF,
} cmd_query_t;

/* 超乳终端电路  接收帧命令格式*/
typedef enum{
	ULT_RECV_EOK 			= 0x01,
    ULT_RECV_ECOM           = 0X02,
	ULT_RECV_R1				= 0x50,
	ULT_RECV_C0				= 0x51,
	ULT_RECV_FREQ			= 0x52,
	ULT_RECV_CURR			= 0x53,
	ULT_RECV_VOLT			= 0x54,
	ULT_RECV_PWR			= 0x55,		
    ULT_RECV_SUMPWR         = 0X56,
    ULT_RECV_VERSION        = 0x57, 
    ULT_RECV_HANDLE_READY_STA = 0x58, 
    ULT_RECV_FPGA_VERSIONS  = 0x59, 
    ULT_RECV_Long_Qm        = 0x5A, 
    ULT_RECV_Tor_Qm         = 0x5B, 
	ULT_RECV_ERROR			= 0x08,
	ULT_RECV_RESET			= 0xa0,
} recv_cmd_t;
#pragma pack(1)
typedef struct 
{
    uint8_t rxIndex;
    rev_state protocolState;
    uint8_t packflag;   //接收完成标志
    union 
    {
        uint8_t allbyte[MAX_ULTRASONICPROTOCOL_LENG];
        struct 
        {
            uint8_t    head;       //帧头
            uint8_t    fix1;       //固定字节1
            uint8_t    fix2;       //固定字节2
            uint8_t    fix3;       //固定字节3
            uint8_t    cmd;        //第一个字节
        }part;
    }pack;
}ult_recive;
typedef struct {
    union 
    {
        uint16_t   instruct;
        struct 
        {
            uint16_t   range:6;     //扭振幅值百分比配置
            uint16_t   power:6;     //纵振功率百分比配置
            uint16_t   cmd:4;      //高四位为指令
        }ctl;   //功率控制指令相关
    }cmd;
    union 
    {
        uint16_t data[3];               //其他数据
        struct 
        {
            uint16_t   lengthwaysTime;  //纵振时间
            uint16_t   verticalTime;    //扭振时间
            uint16_t   stopTime;        //停机时间 
        }tims;  //功率控制指令相关
    }data;
}ult_cmd;
// 字符	转意后
// 0x7A	0x77 0x7A
// 0x75	0x77 0x75
// 0x77	0x77 0x77
#pragma pack(1)
typedef struct 
{
    union 
    {
        uint8_t sendbyte[MAX_ULTRASONICPROTOCOL_LENG];
        struct 
        {
            uint8_t    head;       //帧头
            uint8_t    fix1;       //固定字节1
            uint8_t    fix2;       //固定字节2
            uint8_t    fix3;       //固定字节3
            ult_cmd        cmd;
        }part;
    }pack;
}ult_send;

typedef struct _ult_info_t
{
    uint16_t    _port;      //当前使用的port
    // uint16_t    _error;     //当前错误
    uint32_t    _error;
    float       _R1;
    float       _C0;
    float       _Freq;
    float       _I;
    float       _V;

    uint32_t      _phacotime;
    float        _CDE;         //累计总功率
    uint32_t     version;           //超乳版本
    uint32_t     _handle_scan_sta;  //手柄扫频状态
    uint32_t     fpga_version;
    float        _LongQm;
    float        _TorQm;

    float       _power;
    float       _sumPower;      //累计总功率
}ult_info_t;


typedef struct {
    ult_info_t  _info;
    uint8_t flag;       //是否使能转义功能 1:使能 0:不使能
    ult_send      sendpack;    //发送协议
    ult_recive    readpack;    //接收协议
    ult_ops *ops;
    uint8_t rawsendbyte[MAX_ULTRASONICPROTOCOL_LENG]; //转义发送缓存区
}ult_protocol_t;


extern ult_protocol_t ult_handler;
void bsp_ultprotocol_Init(ult_protocol_t *_this,ult_ops *_ops,uint8_t flag);
void bsp_ultprotocol_IRQHandler(ult_protocol_t *_this);
void bsp_ultprotocol_Task(ult_protocol_t *_this);
void bsp_ultprotocol_sendConf(ult_protocol_t *_this,ult_cmd _cmd);

uint8_t bsp_ultprotocol_ctl_power1(ult_protocol_t *_this,uint8_t _lpow,uint8_t _vpow,uint16_t _ltime,uint16_t _vtime,uint16_t _stime);
uint8_t bsp_ultprotocol_ctl_stopPwr1(ult_protocol_t *_this);
uint8_t bsp_ultprotocol_ctl_cmd(ult_protocol_t *_this,uint16_t cmd);

uint8_t bsp_ultprotocol_change_Port(ult_protocol_t *_this,uint8_t _port);
uint8_t bsp_ultprotocol_reset_Error(ult_protocol_t *_this);
uint8_t bsp_ultprotocol_reset_Sumpower(ult_protocol_t *_this);
uint16_t bsp_ultprotocol_query_Error(ult_protocol_t *_this);

float bsp_ultprotocol_query_R1(ult_protocol_t *_this);
float bsp_ultprotocol_query_C0(ult_protocol_t *_this);
float bsp_ultprotocol_query_Freq(ult_protocol_t *_this);
float bsp_ultprotocol_query_Current(ult_protocol_t *_this);
float bsp_ultprotocol_query_Volt(ult_protocol_t *_this);
float bsp_ultprotocol_query_Power(ult_protocol_t *_this);
float bsp_ultprotocol_query_SumPower(ult_protocol_t *_this);
uint8_t bsp_ultprotocol_sendcmd(ult_cmd _cmd);
uint8_t bsp_ultprotocol_scan_Freq(ult_protocol_t *_this);
uint32_t bsp_ultprotocol_query_handle_det_ready(ult_protocol_t *_this);
uint8_t bsp_ultprotocol_clear_error(ult_protocol_t *_this);
uint32_t bsp_ultprotocol_query_dspVersion(ult_protocol_t *_this);
uint32_t bsp_ultprotocol_query_fpgaVersion(ult_protocol_t *_this);

#endif