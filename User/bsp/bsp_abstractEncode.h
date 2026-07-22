
/*------------------------------------------------------------------------------
 * @file    BSP_ABSTRACTENCODE.H
 * @author  SIERRA TEAM 
 * @date    2024/01/16 11:36:16
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
#ifndef __BSP_ABSTRACTENCODE_H
#define __BSP_ABSTRACTENCODE_H

/* Files includes  -----------------------------------------------------------*/
#include <stdint.h>


/* Defines -------------------------------------------------------------------*/
#define COUNTING_MODE_1             1   //1次计数
#define COUNTING_MODE_2             2   //2次计数
#define COUNTING_MODE_4             4   //4次计数
#define ENCODER_DIR_CW              0   //正转  
#define ENCODER_DIR_CWW             1   //反转
typedef void (*pfunc)();
typedef void (*setCounter)(uint16_t);
typedef uint32_t (*getCounter)();   

typedef struct _hw_inter_t
{
    pfunc       _Init;              //向下的接口 初始化编码器
    setCounter  _setCounter;        //设置定时器计数值
    getCounter  _getCounter;        //获取定时器计数值
    getCounter  _getDir;            //获取定时器运动方向
}hw_inter_t;

typedef struct _Encoder_conf_t
{
    uint8_t     _time;              //轮询间隔 ms
    uint8_t     _Counting_mode;     //一个脉冲计数几次 
    uint16_t    _resolution;        //定时器溢出的cnt
    float       _scale;             //电机一圈/编码器一圈
}Encoder_conf_t;

typedef struct _abstractEncoder_t {
    hw_inter_t *hw_inter;        //底层硬件接口
    Encoder_conf_t cof;         //编码器配置
    uint8_t     dir;            //运动方向
    int16_t      over_count;     //溢出计数
    volatile int32_t     last_count;     //上一时刻总计数值
    volatile int32_t     all_count;      //总脉冲数
    volatile int32_t     pos;            //绝对位置
    volatile float       vol;            //电机速度
}abstractEncoder_t;
/* Variables -----------------------------------------------------------------*/


/* Functions ----------------------------------------------------------------*/
void bsp_abstractEncode_Init(abstractEncoder_t *_this,hw_inter_t *_hw,Encoder_conf_t *_conf);
void bsp_abstractEncode_SetConf(abstractEncoder_t *_this,Encoder_conf_t _cof);
void bsp_abstractEncode_IQRHandler(abstractEncoder_t *_this);
void bsp_abstractEncode_Task(abstractEncoder_t *_this);
void  bsp_abstractEncode_Reset(abstractEncoder_t *_this);
uint8_t bsp_abstractEncode_GetDir(abstractEncoder_t *_this);
float bsp_abstractEncode_GetVol(abstractEncoder_t *_this);
int32_t bsp_abstractEncode_GetPos(abstractEncoder_t *_this);
int32_t bsp_abstractEncode_GetCount(abstractEncoder_t *_this);
#endif
