/*------------------------------------------------------------------------------
 * @file    DIATHERMY_TASK.H
 * @author  SIERRA TEAM 
 * @date    2024/07/18 16:49:40
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
#ifndef __DIATHERMY_TASK_H
#define __DIATHERMY_TASK_H

/* Files includes  -----------------------------------------------------------*/
#include "bsp_diathermy.h"
/* Defines -------------------------------------------------------------------*/


/* Variables -----------------------------------------------------------------*/


typedef union _Dia_tag_t
{
    uint8_t buf[64];
    struct 
    {
        uint8_t     ower[4];        //所属类型
        uint8_t     needle[16];     //针头型号
        uint8_t     type[24];
        uint8_t     sn[20]; 
    }info;
}Dia_tag_t;

typedef struct _Dia_Task_Handpiece_ops
{
    uint8_t (*_get_state)();    //获取连接状态
    void (*_connected)();       //连接操作函数
    void (*_disconnected)();    //断开连接操作函数
    void (*_set_color)(uint32_t _rgb);  //设置rgb灯
    void (*_read_e2prom)(uint8_t *buf,uint16_t _len);
}Dia_Task_Handpiece_ops;

typedef struct _Diathermy_Task_Cof
{
    uint32_t    _mode;          //0x00 不使能   0x01 固定模式   0x02 线性模式
    uint32_t    _detectCon;     //检测连接      0x00 不检测  0x01检测
    uint32_t    _footcof;       //踏板控制      0x00 不可控制   0x01 一段控制   0x03 二段控制   0x05三段控制
                                //0x01000007响应开关1   0x02000007 响应开关2   0x03000007 响应开关3  
    uint32_t    _work_color;            //工作时颜色
    // uint32_t    _port_color;            //端口设置颜色 该接口上位机配置直接控制环形灯
    uint32_t    _set_startValve;        //电凝设置起始值 (0~100)
    uint32_t    _set_valve;             //电凝设置数值 (0~100)
}Diathermy_Task_Cof;

typedef struct _Diathermy_Task_Data
{
    uint32_t        _read_valve;        //电凝反馈值
    uint32_t        _conStatus;         //连接状态
    union           {
        uint32_t    _words[sizeof(Dia_tag_t)/4];
        Dia_tag_t _tag;
    }_tag_info;
    bsp_dia_info    _info;              //   
}Diathermy_Task_Data;


typedef  union 
{
    uint32_t    _percentofPedal;            //踏板百分比
    struct 
    {
        uint16_t    _duty;                  //比列
        uint8_t _set_key;                   //设置的按键
        uint8_t _active_key;                //激活的按键
    }bytes;
}PedalData;

typedef struct _Diathermy_Task_t
{
    uint32_t    _errorCode;
    uint32_t    _portsCon;                  //俩个端口连接状态
    PedalData   _pedal;
    float       _setpower;                  //下发设置功率
    bsp_dia_t   * _dia_handler;             //电凝操作句柄
    Diathermy_Task_Cof  *_cof;
    Diathermy_Task_Data *_data;
    Dia_Task_Handpiece_ops  _handpiece_ops;
}Diathermy_Task_t;

/* Functions ----------------------------------------------------------------*/


int Diathermy_Task_Init(void);
void Diathermy_Task(void *arg);
void Diathermy_Task_SetWcof(Diathermy_Task_Cof *_cof);
void Diathermy_Task_SetData(Diathermy_Task_Data *_data);
void Diathermy_Pedal_Deal(uint16_t _code,uint32_t _data);
void Diathermy_Pedal_Disconnect();
void Diathermy_Task_SetPwr(uint8_t percent);
#endif
