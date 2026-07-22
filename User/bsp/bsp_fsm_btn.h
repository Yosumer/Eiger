/*** 
 * @Author: sum 476328608@qq.com
 * @Date: 2023-11-09 11:44:01
 * @LastEditors: sum 476328608@qq.com
 * @LastEditTime: 2023-11-30 12:07:22
 * @FilePath: \elabc:\sierra\LiquidModule\Liquid_Cj\code\phaco\Source\bsp\bsp_fsm_btn.h
 * @Description: 
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __BSP_FSM_BTN_H
#define __BSP_FSM_BTN_H
//#include "includes.h"
#include <stdio.h>
#include <stdint.h>
#define BTN_LONG_PRESS_MAX_TIME_MS 		(500)
typedef enum _btn_status_t
{
    BTN_UP = 0,       // 传入的状态，松开的时候
    BTN_SURE,         // 派生的状态，松开到按下的时候，用于消抖
    BTN_DOWN,         // 传入的状态，按下的时候
    BTN_LONG_PRESS,   // 派生的状态，在按下超过时间的时候进入该状态
} btn_status_t;

typedef enum _btn_event_t
{
    BTN_EVENT_NONE = 0,
    BTN_EVENT_PRESS,   // 按下事件
    BTN_EVENT_RELEASE, // 松开事件
    BTN_EVENT_LONG,    // 长按事件
} btn_event_t;

typedef uint8_t (*btn_get_level_t)(void);
typedef void (*btn_event_handle_t)(const char*name,btn_event_t event);

#pragma(4)
typedef struct _btn_fsm_t
{
	char name[6];
    btn_status_t status;
    btn_event_t event;
    btn_get_level_t get_level;
    btn_event_handle_t event_handle;
    uint32_t time_ms;
    uint32_t long_press_count;
} btn_fsm_t;

void btn_fsm_Init(btn_fsm_t *btn_fsm,btn_get_level_t get_level, uint32_t time_ms,const char * _name);
void btn_fsm_handle(btn_fsm_t *self);
btn_event_t btn_fsm_get_event(btn_fsm_t *self);
void btn_fsm_set_event_cb(btn_fsm_t *self, btn_event_handle_t event_handle);
#endif
