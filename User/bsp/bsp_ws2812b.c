/*------------------------------------------------------------------------------
 * @file    BSP_WS2812B.C
 * @author  FIRMWARE TEAM 
 * @date    2024/07/04 16:11:47
 * @brief   THIS FILE PROVIDES ALL THE FIRMWARE FUNCTIONS.
 * -----------------------------------------------------------------------------
 * @attention 
 
 * THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE 
 * CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE 
 * TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR 
 * CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH 
 * HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN 
 * CONNECTION WITH PRODUCTS MADE BY CUSTOMERS. 
 * 
 * <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2> 
------------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion  ------------------------------------*/


/* Files includes  -----------------------------------------------------------*/
#include "bsp_ws2812b.h"
#include <rtthread.h>
#define LOG_TAG            "drv.ws2812"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>
#define _ASSERT(x)         RT_ASSERT(x)


static void _bsp_ws2812b_color2bit(bsp_ws2812b *_this)
{
    // for(int i=0;i<_this->num;i++)
    // {
    //     for(int j=0;j<24;j++)
    //     {
    //         if(_this->color_buf->grb & (0x800000>>j))
    //             _this->colorbit_buf[j+i*24] = CODE1_PULSE;
    //         else
    //             _this->colorbit_buf[j+i*24] = CODE0_PULSE;
    //     }
        
    // }
    // for(int j=0;j<RESET_NUM;j++)
    //     _this->colorbit_buf[_this->num*24+j] = 0;


    for(int j=0;j<RESET_NUM;j++)
        _this->colorbit_buf[j] = 0;
    for(int i=0;i<_this->num;i++)
    {
        for(int j=0;j<24;j++)
        {
            if(_this->color_buf[i].grb & (0x800000>>j))
                _this->colorbit_buf[RESET_NUM+j+i*24] = CODE1_PULSE;
            else
                _this->colorbit_buf[RESET_NUM+j+i*24] = CODE0_PULSE;
        }
        
    }
    _this->colorbit_buf[_this->num*24+RESET_NUM] = 0;
}

static void _bsp_ws2812b_refresh(bsp_ws2812b *_this)
{
    _bsp_ws2812b_color2bit(_this);
    _this->Start_PWM_DMA((uint32_t*)_this->colorbit_buf,_this->num*24+RESET_NUM+1);
}
/*----------------------------------------------------------------------------*/
int bsp_ws2812b_Init(bsp_ws2812b *_this,uint16_t _num,void (*_Init)(),void (*_Start_PWM_DMA) (uint32_t*_buf,uint16_t _len))
{
    	_ASSERT(_this);
        _ASSERT(_Start_PWM_DMA);
        _this->Init = _Init;
        _this->num = _num;
        _this->Start_PWM_DMA = _Start_PWM_DMA;

        // 申请内存
        _this->color_buf = (grb_t*)rt_calloc(_num,sizeof(grb_t));
        _this->colorbit_buf = rt_calloc(_num,4*_num*24+RESET_NUM+1);
        if(RT_NULL == _this->color_buf ||  RT_NULL == _this->colorbit_buf)
        {
            LOG_E("ws2812b calloc memory faild!!");
            return -1;
        }
        if(RT_NULL != _Init)
            _this->Init();  //初始化硬件pwm 
}
void bsp_ws2812b_Ctl_SColor(bsp_ws2812b *_this,uint16_t _index,grb_t _color)
{
       	_ASSERT(_this);
        _ASSERT(_index<_this->num);
        _this->color_buf[_index] = _color;
        _bsp_ws2812b_refresh(_this);
}

void bsp_ws2812b_Ctl_RangeColor(bsp_ws2812b *_this,uint16_t _start,uint16_t _end,grb_t* _color)
{
        _ASSERT(_this);
        _ASSERT(_start<_this->num);
        _ASSERT(_end<_this->num);
        for(int i=_start;i<=_end;i++)
        {
            _this->color_buf[i] = _color[i];
        }
        _bsp_ws2812b_refresh(_this);

}

void bsp_ws2812b_Ctl_AllColor(bsp_ws2812b *_this,grb_t _color)
{
    _ASSERT(_this);
    for(int i=0;i<_this->num;i++)
    {
        _this->color_buf[i] = _color;
    }
    _bsp_ws2812b_refresh(_this);
}

void bsp_ws2812b_Ctl_SOFF(bsp_ws2812b *_this,uint16_t _index)
{
    LED_COLOR_32(_color,LED_COLOR_BLACK);
    bsp_ws2812b_Ctl_SColor(_this,_index,_color);
}

void bsp_ws2812b_Ctl_AllOFF(bsp_ws2812b *_this)
{
    LED_COLOR_32(_color,LED_COLOR_BLACK);
    bsp_ws2812b_Ctl_AllColor(_this,_color);
}

#include "bsp_timer2_pwm.h"
bsp_ws2812b ws1;
bsp_ws2812b ws2;
int bsp_ws2812b_test(void)
{
   bsp_ws2812b_Init(&ws1,10,bsp_timer2_pwm_Init,bsp_timer2_Start_PWM_CH1_DMA);

   LED_COLOR_32(color,LED_COLOR_BLUE);
   bsp_ws2812b_Ctl_AllOFF(&ws1);

//    bsp_ws2812b_Ctl_RangeColor(&ws1,0,4,color);

	// LED_COLOR_32(color1,LED_COLOR_GREEN);
//    LED_COLOR_32(color1,LED_COLOR_GREEN);

//   bsp_ws2812b_Ctl_RangeColor(&ws1,5,9,color1);


    LED_COLOR_32(color1,LED_COLOR_DEFAULT);
    bsp_ws2812b_Ctl_AllColor(&ws1,color1);
}
INIT_ENV_EXPORT(bsp_ws2812b_test);


