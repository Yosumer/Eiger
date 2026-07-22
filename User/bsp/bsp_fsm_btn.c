#include "bsp_fsm_btn.h"
#include "includes.h"
void btn_fsm_Init(btn_fsm_t *btn_fsm,btn_get_level_t get_level, uint32_t time_ms,const char * _name)
{
    if (btn_fsm == NULL)
    {
        return ;
    }
        // TODO: 初始化参数
    // strcpy(btn_fsm->name,_name);
    rt_strcpy(btn_fsm->name,_name);
    // strncpy(btn_fsm->name,_name,6);
    btn_fsm->get_level = get_level;
    btn_fsm->status = BTN_UP;
    btn_fsm->event = BTN_EVENT_NONE;
    btn_fsm->event_handle = NULL;
    btn_fsm->time_ms = time_ms;
    btn_fsm->long_press_count = 0;
}



void btn_fsm_handle(btn_fsm_t *self)
{
    switch (self->status)
    {
    case BTN_UP:
        if (self->get_level() == 1)     //按键触发
        {
            self->status = BTN_SURE;
            self->event = BTN_EVENT_NONE;
        }
        else
        {
            self->status = BTN_UP;  
        }
        break;
    case BTN_SURE:                      //该状态进行消抖
        if (self->get_level() == 1)
        {
            self->status = BTN_DOWN;
            self->event = BTN_EVENT_PRESS;
            if (self->event_handle != NULL)
            {
                self->event_handle(self->name,BTN_EVENT_PRESS);
            }
        }
        else
        {
            self->status = BTN_UP;
            self->event = BTN_EVENT_NONE;
        }
        break;
    case BTN_DOWN:
        self->long_press_count++;
        if (self->get_level() == 1)
        {
			uint32_t _longtime = (self->long_press_count * self->time_ms);
			if (_longtime >= BTN_LONG_PRESS_MAX_TIME_MS)
			{
                self->long_press_count = 0;
                self->status = BTN_LONG_PRESS;
                self->event = BTN_EVENT_LONG;
                if (self->event_handle != NULL)
                {
                    self->event_handle(self->name,BTN_EVENT_LONG);
                }
			}
            else
            {
                self->status = BTN_DOWN;
                self->event = BTN_EVENT_NONE;
            }
        }
        else
        {
            self->long_press_count = 0;
            self->status = BTN_UP;
            self->event = BTN_EVENT_RELEASE;
            if (self->event_handle != NULL)
            {
                self->event_handle(self->name,BTN_EVENT_RELEASE);
            }
        }

        break;

    case BTN_LONG_PRESS:
        if (self->get_level() == 1)
        {
            self->status = BTN_LONG_PRESS;
            self->event = BTN_EVENT_NONE;
        }
        else
        {
            self->status = BTN_UP;
            self->event = BTN_EVENT_RELEASE;
            if (self->event_handle != NULL)
            {
                self->event_handle(self->name,BTN_EVENT_RELEASE);
            }
        }

        break;

    default:
        break;
    }
}

btn_event_t btn_fsm_get_event(btn_fsm_t *self)
{
    if (self == NULL)
    {
        return BTN_EVENT_NONE;
    }

    return self->event;
}

void btn_fsm_set_event_cb(btn_fsm_t *self, btn_event_handle_t event_handle)
{
    if (self == NULL)
    {
        return;
    }

    self->event_handle = event_handle;
}