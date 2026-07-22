#ifndef __VALVE_TASK_H
#define __VALVE_TASK_H	
#include "bsp_valve.h"
#define VALVE_TASK_PRIO		28

//任务堆栈大小	
#define VALVE_STK_SIZE 		512


// enum VALVE_CHANNEL {
//     VALVE_IV_BREAK = 0, // IV杆刹车信号
//     VALVE_INFUSION,     // 灌注阀门
//     VALVE_REFLUX,        //回流阀门
//     VALVE_LEFT_BOX,     //积液盒左进
//     VALVE_RIGHT_BOX,    // 积液盒右进  
    
//     VALVE_AIR_PUMB,     //隔膜泵控制
//     VALVE_MAX
// };


typedef union _Valve_Task_t
{
    uint32_t buf[VALVE_MAX];
    struct valve_task
    {
        // 开关阀
        uint32_t    valve_iv_pwr;      //(0x54)   
        uint32_t    valve_infusion;     //
        uint32_t    valve_reflux;        //
        uint32_t    valve_left_box;     //
        uint32_t    valve_right_box;    //
        uint32_t    valve_air_pumb;     
    }valve;
}Valve_Task_t;

extern void Valve_Task(void *p_arg);
extern void Valve_SetWdata(Valve_Task_t *wdata);
#endif


