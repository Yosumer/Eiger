/*
 * @Author: 
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-08-29 10:56:50
 * @FilePath: \01LightModulec:\Work\Project\Patagonia\02Patagonia\02LiquidModule\Liquid\User\bsp\liquid\bsp_step_motor.h
 * @Description: 
 */
#ifndef BSP_STEP_MOTOR_H 
#define BSP_STEP_MOTOR_H 

#include <stdint.h>


typedef struct _bsp_step_motor_conf
{
    uint32_t    _amax;  //最大加速度    step/s^2
    uint32_t    _vmax;  //最大速度      step/s
    uint32_t    _dmax;  //最大减速度
    uint32_t    _spr;   //一圈多少步
    uint32_t    _espr;  //编码器一圈多少步
}bsp_step_motor_conf;

typedef struct _bsp_step_motor_data
{
    float       _rps;          //电机一圈速度
    int32_t    _targetPos;     //电机目标位置
    int32_t    _readPos;       //电机读取位置
    uint32_t    _amax;
    uint32_t    _dmax;  //最大减速度
    int32_t    _targetspeed;   //电机最大速度
    int32_t    _readspeed;     //电机读取速度
    uint32_t   _status;        //电机状态

    float      _encoderps;     //编码器一圈数值
    uint32_t   _lasttick;
    int32_t    _encodePoslast; //上次编码器数值
    int32_t    _encodePos;     //编码器数值
    int32_t    _encodespeed;   //编码器数据
}bsp_step_motor_data;



typedef struct _bsp_step_motor_ops
{
    uint8_t (*init)(void *_me);                                     //电机初始化
    void (*config)(void *_me,bsp_step_motor_conf *_cof);        //电机配置 (设置加速度、减速度等参数)
    void (*rotate)(void *_me,int32_t velocity);   //电机旋转 +：CW -:CWW
    void (*stop)(void *_me);                     //电机停止
    void (*moveTo)(void *_me,int32_t position);   //电机运动到具体位置
    void (*moveBy)(void *_me,int32_t step);       //电机在当前的位置上移动
    void (*set_position)(void *_me,int32_t _pos); //
    void (*task)(void *_this);
    uint32_t (*get_status)(void *_me);           //获取电机运行状态
    int32_t (*get_velocity)(void *_me);          //获取电机速度
    int32_t (*get_position)(void *_me);          //获取实时位置（理论）
    int32_t (*get_encode)(void *_me);            //获取编码器的值
}bsp_step_motor_ops;


typedef struct _bsp_step_motor
{
    uint32_t            status;
    char                name[8];            //
    void                *instance;
    bsp_step_motor_data data;
    bsp_step_motor_conf *cof;
    bsp_step_motor_ops *ops;
}bsp_step_motor_t;


uint8_t bsp_step_motor_Init(bsp_step_motor_t *_this,bsp_step_motor_ops *_ops);
void bsp_step_motor_Config(bsp_step_motor_t *_this,bsp_step_motor_conf *_cof);
void bsp_step_motor_Rotate(bsp_step_motor_t *_this,int32_t velocity);
void bsp_step_motor_Stop(bsp_step_motor_t *_this);
void bsp_step_motor_MoveTo(bsp_step_motor_t *_this,int32_t position);
void bsp_step_motor_MoveBy(bsp_step_motor_t *_this,int32_t step);
void bsp_step_motor_ResetPos(bsp_step_motor_t *_this);
void bsp_step_motor_Task(bsp_step_motor_t *_this);

int bsp_step_motor_GetEncodePosition(bsp_step_motor_t *_this);
uint8_t bsp_step_motor_IsEncodePositionInRange(bsp_step_motor_t *_this, int min, int max);
uint8_t bsp_step_motor_IsLoseSpeed(bsp_step_motor_t *_this, float duty);

#endif // BSP_STEP_MOTOR_H