
/*------------------------------------------------------------------------------
 * @file    BSP_WS2812B.H
 * @author  SIERRA TEAM 
 * @date    2024/07/04 14:56:34
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
#ifndef __BSP_WS2812B_H
#define __BSP_WS2812B_H

/* Files includes  -----------------------------------------------------------*/
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/

// #define CODE0_PULSE         3  //0码占空比
#define CODE0_PULSE         5  //0码占空比
#define CODE1_PULSE         15  //1码占空比
#define RESET_NUM           80  //复位时间

#define LED_COLOR_BLACK     0x000000
#define LED_COLOR_RED       0x00FF00
#define LED_COLOR_GREEN     0XFF0000
#define LED_COLOR_BLUE      0X0000FF

#define LED_COLOR_DEFAULT   (50<<16|50<<8|50)


#define LED_COLOR_BLUE1      0xE9EBFE 




#define APP_RGB_RED                    0XFF0000
#define APP_RGB_GREED                  0X007F00
#define APP_RGB_BLUE                   0X00005F
#define APP_RGB_ORANGE                 0X7F6100
#define APP_RGB_BLACK                  0X000000
#define APP_RGB_PURPLE                 0X3F00FF            //紫色



    


#define LED_COLOR_32(name,x) \
grb_t name = {.grb = x}
#define LED_COLOR_32_C(name,x)  \
{name.grb = x;}
#define LED_COLOR_RGB(name,r,g,b)    \
grb_t name = { .color = { .blue = b, .red = r, .green = g, ._rev = 0x00 } }
#define LED_COLOR_RGB_C(name,r,g,b)    \
{ name.color.blue = b; name.color.red = r;name.color.green = g;}

#define LED_COLOR_RGB2GRB(X) ((X & 0xFF0000) >> 8) | (X & 0x00FF00) << 8 | ((X & 0x0000FF))

typedef union _grb_t
{
    uint32_t grb;
    struct 
    {
        uint8_t blue;
        uint8_t red;
        uint8_t green;
        uint8_t _rev;
    }color;
}grb_t;


typedef struct _bsp_ws2812b
{
    uint16_t num;       //灯珠总数量
    grb_t*  color_buf;  
    uint32_t *colorbit_buf;
    void (*Init)();     //初始化函数 需要提供1.05Mhz的pwm波且plus为10
    void (*Start_PWM_DMA) (uint32_t*_buf,uint16_t _len);
}bsp_ws2812b;


/* Variables -----------------------------------------------------------------*/

extern bsp_ws2812b ws1;
extern bsp_ws2812b ws2;
extern bsp_ws2812b ws3;
/* Functions ----------------------------------------------------------------*/
int bsp_ws2812b_Init(bsp_ws2812b *_this,uint16_t _num,void (*_Init)(),void (*_Start_PWM_DMA) (uint32_t*_buf,uint16_t _len));
void bsp_ws2812b_Ctl_SColor(bsp_ws2812b *_this,uint16_t _index,grb_t _color);
void bsp_ws2812b_Ctl_RangeColor(bsp_ws2812b *_this,uint16_t _start,uint16_t _end,grb_t* _color);


// void bsp_ws2812b_Ctl_RangeColor(bsp_ws2812b *_this,uint16_t _start,uint16_t _end,grb_t _color);

void bsp_ws2812b_Ctl_AllColor(bsp_ws2812b *_this,grb_t _color);

void bsp_ws2812b_Ctl_SOFF(bsp_ws2812b *_this,uint16_t _index);
void bsp_ws2812b_Ctl_AllOFF(bsp_ws2812b *_this);
#endif
