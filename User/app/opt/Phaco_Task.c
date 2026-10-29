/*------------------------------------------------------------------------------
 * @file    PHACO_TASK.C
 * @author  FIRMWARE TEAM
 * @date    2024/07/16 17:30:30
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
#include <rtthread.h>
#include "includes.h"
#include "Phaco_Task.h"
#include "bsp_ws2812b.h"
#include "bsp_m95512.h"
#define LOG_TAG            "task.Phaco"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>

#define ACTIVE_COLOR        0X007F00
#define UNACTIVE_COLOR      0x000000


#define PHACO_3D_CONFIG_DISABLE 0


/* Defines -------------------------------------------------------------------*/
#define PHACO_TASK_PRIO   20
//#define PHACO_TASK_STK_SIZE    512
#define PHACO_TASK_STK_SIZE    512

//#define E2PROM_READ_OFFSET  0x3200
#define E2PROM_READ_OFFSET  0x00
// #define E2PROM_WRITE_OFFSET 0x1000
#define E2PROM_WRITE_OFFSET 0x200

// #define E2PROM_WRITE_HANDPIECE
//  #define E2PROM_WRITE_INFINITOR

// #define E2P1_SSPI_BUS       "sspi2"
// #define E2P1_SSPI_NAME      "sp20"
// #define E2P1_SSPI_CS        "PD.3"


 #define E2P1_SSPI_BUS       "sspi3"
 #define E2P1_SSPI_NAME      "sp30"
 #define E2P1_SSPI_CS        "PD.3"



//#define E2P1_SSPI_BUS       "sspi3"
//#define E2P1_SSPI_NAME      "sp30"
//#define E2P1_SSPI_CS        "PD.4"

/* Variables -----------------------------------------------------------------*/
OS_TCB Phaco_TaskTCB;
CPU_STK Phaco_TASK_STK[PHACO_TASK_STK_SIZE];

/*----------------------------------------------------------------------------*/
// Handpiece_tag_t _default_tag = {
//     .info.ower = {'A', 'L', 'C', 'O'},
//     .info.needle = {"phaco"},
//     .info.type = {"Ozil"},
//     .info.sn = {"CGDD2404240011223344"}
// };

// Handpiece_tag_t _default_tag = {
//     .info.ower = {'A', 'L', 'C', 'O'},
//     .info.needle = {"phaco"},
//     .info.data.type = {0},
//     .info.sn = {"CGDD2404240011223344"}
// };

Handpiece_tag_t _default_tag = {
    .info.ower = {'A', 'L', 'C', 'O'},
    .info.data.type = {0},
    .info.sn = {"PH00T01-251206"}
};

const phaco_cal_param_t default_phaco_lcal_table = {
    /*PHACO_POWER_0,*/ 		0,	 	0,
    /*PHACO_POWER_10,*/ 	10,	 	7,
    /*PHACO_POWER_20,*/ 	20,	 	10,
    /*PHACO_POWER_30,*/ 	30,	 	14,
    /*PHACO_POWER_40,*/ 	40,	 	18,
    /*PHACO_POWER_50,*/ 	50,	 	22,
    /*PHACO_POWER_60,*/ 	60,	 	27,
    /*PHACO_POWER_70,*/ 	70,	 	31,
    /*PHACO_POWER_80,*/ 	80,	 	36,
    /*PHACO_POWER_90,*/ 	90,	 	40,
    /*PHACO_POWER_100,*/ 	100,	42,
};


const phaco_cal_param_t default_phaco_vcal_table = {
    /*PHACO_POWER_0, */		0,	 	0,
    /*PHACO_POWER_10,*/ 	10,	 	5,
    /*PHACO_POWER_20,*/ 	20,	 	13,
    /*PHACO_POWER_30,*/ 	30,	 	18,
    /*PHACO_POWER_40,*/ 	40,	 	23,
    /*PHACO_POWER_50,*/ 	50,	 	29,
    /*PHACO_POWER_60,*/ 	60,	 	35,
    /*PHACO_POWER_70,*/ 	70,	 	40,
    /*PHACO_POWER_80,*/ 	80,	 	44,
    /*PHACO_POWER_90,*/ 	90,	 	49,
    /*PHACO_POWER_100,*/ 	100,	56,
};
const uint8_t inifinte_data[1024]={
    0x41 ,0x4C ,0x43 ,0x4F ,0x4E ,0x00 ,0x32 ,0x30 ,0x30 ,0x31 ,0x37 ,0x37 ,0x39 ,0x36 ,0x31 ,0x30 ,
    0x58 ,0x00 ,0x00 ,0x31 ,0x30 ,0x30 ,0x35 ,0x32 ,0x30 ,0x00 ,0x01 ,0x03 ,0x07 ,0x05 ,0x46 ,0x3C ,
    0x04 ,0x0F ,0x1E ,0x0A ,0x34 ,0x9E ,0xA4 ,0xB5 ,0x0A ,0x00 ,0x78 ,0x00 ,0xB0 ,0x04 ,0xBC ,0x02 ,
    0xBC ,0x02 ,0x84 ,0x03 ,0x70 ,0x17 ,0x64 ,0x19 ,0x65 ,0x19 ,0x1B ,0x00 ,0x16 ,0x00 ,0x01 ,0x00 ,
    0x04 ,0x19 ,0x0A ,0x02 ,0x02 ,0x03 ,0x03 ,0x06 ,0x26 ,0x14 ,0x14 ,0x14 ,0x14 ,0x28 ,0x64 ,0x19 ,
    0xE8 ,0x03 ,0xFF ,0xFF ,0x00 ,0x00 ,0x03 ,0x00 ,0x05 ,0x00 ,0x08 ,0x00 ,0x0B ,0x00 ,0x0E ,0x00 ,
    0x11 ,0x00 ,0x14 ,0x00 ,0x17 ,0x00 ,0x1B ,0x00 ,0x1E ,0x00 ,0x21 ,0x00 ,0x24 ,0x00 ,0x27 ,0x00 ,
    0x2A ,0x00 ,0x2D ,0x00 ,0x31 ,0x00 ,0x34 ,0x00 ,0x37 ,0x00 ,0x3A ,0x00 ,0x3D ,0x00 ,0x41 ,0x00 ,
    0x44 ,0x00 ,0x47 ,0x00 ,0x4A ,0x00 ,0x4E ,0x00 ,0x51 ,0x00 ,0x54 ,0x00 ,0x57 ,0x00 ,0x5B ,0x00 ,
    0x5E ,0x00 ,0x61 ,0x00 ,0x65 ,0x00 ,0x68 ,0x00 ,0x6B ,0x00 ,0x6E ,0x00 ,0x72 ,0x00 ,0x75 ,0x00 ,
    0x78 ,0x00 ,0x7C ,0x00 ,0x7F ,0x00 ,0x82 ,0x00 ,0x86 ,0x00 ,0x89 ,0x00 ,0x8C ,0x00 ,0x90 ,0x00 ,
    0x93 ,0x00 ,0x96 ,0x00 ,0x9A ,0x00 ,0x9D ,0x00 ,0xA1 ,0x00 ,0xA4 ,0x00 ,0xA7 ,0x00 ,0xAB ,0x00 ,
    0xAE ,0x00 ,0xB1 ,0x00 ,0xB5 ,0x00 ,0xB8 ,0x00 ,0xBC ,0x00 ,0xBF ,0x00 ,0xC2 ,0x00 ,0xC6 ,0x00 ,
    0xC9 ,0x00 ,0xCD ,0x00 ,0xD0 ,0x00 ,0xD3 ,0x00 ,0xD7 ,0x00 ,0xDA ,0x00 ,0xDE ,0x00 ,0xE1 ,0x00 ,
    0xE5 ,0x00 ,0xE8 ,0x00 ,0xEB ,0x00 ,0xEF ,0x00 ,0xF2 ,0x00 ,0xF6 ,0x00 ,0xF9 ,0x00 ,0xFD ,0x00 ,
    0x00 ,0x01 ,0x03 ,0x01 ,0x07 ,0x01 ,0x0A ,0x01 ,0x0E ,0x01 ,0x11 ,0x01 ,0x15 ,0x01 ,0x18 ,0x01 ,
    0x1C ,0x01 ,0x1F ,0x01 ,0x23 ,0x01 ,0x26 ,0x01 ,0x29 ,0x01 ,0x2D ,0x01 ,0x30 ,0x01 ,0x34 ,0x01 ,
    0x37 ,0x01 ,0x3B ,0x01 ,0x3E ,0x01 ,0x42 ,0x01 ,0x45 ,0x01 ,0x49 ,0x01 ,0x4C ,0x01 ,0x75 ,0x40 ,
    0x02 ,0x16 ,0x01 ,0x0D ,0x0F ,0x1E ,0x08 ,0x00 ,0x30 ,0x75 ,0xD0 ,0x84 ,0x0A ,0x00 ,0x32 ,0x00 ,
    0xB0 ,0x04 ,0x0E ,0x00 ,0x0A ,0x00 ,0x00 ,0x00 ,0x04 ,0x19 ,0x06 ,0x28 ,0x4C ,0x01 ,0x64 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xE0 ,0x37 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x14 ,0x00 ,0xE8 ,0x60 ,0x47 ,0x00 ,0xCC ,0x5F ,0xBB ,0x00 ,0x9A ,0xD5 ,0x65 ,0x42 ,0x53 ,0x1B ,
    0xAF ,0x41 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x45 ,0x00 ,0x00 ,0x00 ,0x02 ,0x00 ,
    0x02 ,0xA0 ,0x6C ,0x00 ,0xCC ,0x5F ,0xBB ,0x00 ,0xD1 ,0x0C ,0x77 ,0x42 ,0x70 ,0x38 ,0x0F ,0x42 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    
    };

Phaco_Task_t    _phaco = {
	.cal_param_t._lcali_tbl = &ramdata.phaco_pow_tbl._lcali,
	.cal_param_t._vcali_tbl = &ramdata.phaco_pow_tbl._vcali,
};


e2prom_m95512_device_t  e2p1;
// e2prom_m95512_device_t  e2p2;
extern bsp_ws2812b ws1;
// extern bsp_ws2812b ws2;
/*----------------------------------------------------------------------------*/

struct {
    uint8_t lpow;
    uint8_t vpow;
    uint16_t ltime;
    uint16_t vtime;
    uint16_t stime;
} last_out;

static void _Phaco_Task_Handpiece_write_e2prom(Phaco_Task_t *_this);
void _Phaco_Update_Percent(uint32_t _percent);
void _Phaco_HandPice1_Write_E2prom(uint8_t *buf,uint16_t _offset, uint16_t _len);


#if 0
/* Old implementation kept for reference. Replaced because V-time elapsed calculation
 * used the quotient of the V segment instead of the elapsed time inside that segment,
 * causing ramdata._phacoruntime to update inaccurately when _ltim and _vtim are both used.
 */
static void _Phaco_Update_CDE(Phaco_Task_t *_this,uint8_t _en)
{
    /*
     * 功能：根据纵振(_ltim)和扭振(_vtim)的周期配置，累计有效超乳时间和CDE。
     * 旧实现中扭振时间使用除法商值判断，导致同时存在_ltim/_vtim时，V段时间不能连续增长。
     * 新实现按完整周期数 + 当前周期内已运行时间分别计算L/V有效时间。
     */
    static uint32_t _runtime = 0;
    static uint32_t _last_time = 0;
    static float _lastcde = 0.0f;
    static float _cde = 0.0f;
    static uint32_t _vaildtime = 0;
    if(_en)
    {
        _this->WData->_footcntim = (rt_tick_get_millisecond() -  _this->WData->_fristtim);//获取这次运行的时间
        _runtime+=  _this->WData->_footcntim;
        uint16_t _cycle = _this->Wcof->_prams._ltim + _this->Wcof->_prams._vtim + (_this->Wcof->_mode==PHACP_BRUST_MODE?_this->Wcof->_prams._cutetims:_this->Wcof->_prams._stim);

        float _vcde=0.0f;
        float _lcde=0.0f;
        uint32_t _vtime=0;
        uint32_t _ltime=0;
        //判断当前时间属于哪段 纵振->扭振->停止
        uint16_t _segall = _runtime%_cycle;
        uint16_t _cnt =  _runtime/_cycle;
        if(_this->Wcof->_prams._ltim>0)
        {
            //计算纵振时间
            // float _ltim = (float)((_this->WData->_footcntim/_cycle+((_this->WData->_footcntim%_cycle)/_this->Wcof->_prams._ltim))*_this->Wcof->_prams._ltim+((_this->WData->_footcntim%_cycle)%_this->Wcof->_prams._ltim));

             _ltime = ((_segall/_this->Wcof->_prams._ltim)>0)?_this->Wcof->_prams._ltim*(_cnt+1):(_segall%_this->Wcof->_prams._ltim+_this->Wcof->_prams._ltim*_cnt);
            //计算纵振CDE
            _lcde = (float)(_ltime * _this->WData->_lpwr*0.00001f);
        }
        if(_this->Wcof->_prams._vtim>0)
        {
            //计算扭振时间
            // float _vtim = (float)((_this->WData->_footcntim/_cycle+((_this->WData->_footcntim%_cycle)/_this->Wcof->_prams._vtim))*_this->Wcof->_prams._vtim+((_this->WData->_footcntim%_cycle)%_this->Wcof->_prams._vtim));
          
          
            int32_t  _segv = (_segall-_this->Wcof->_prams._ltim)/(_this->Wcof->_prams._vtim);
             _vtime = (_segv>0)?_this->Wcof->_prams._vtim*(_cnt+1):(_segv<0?_this->Wcof->_prams._vtim*_cnt:_segv%_this->Wcof->_prams._vtim+_this->Wcof->_prams._vtim*_cnt);
            //计算扭振CDE
            _vcde = (float)(_vtime * _this->WData->_vpwr*0.000004f);

        }
        //计算总的CDE
        _cde  = (_lcde + _vcde)>0?(_lcde + _vcde):_lastcde;
        _vaildtime =(_ltime + _vtime);
		float _increase = (float)(_cde-_lastcde);
//        _this->WData->_pow += _increase;
        if (_increase>0)
        {
            ramdata._phacoruntime+=(_vaildtime-_last_time);
					_this->WData->_pow += _increase;
        }
    }
    else
    {
        _runtime = 0;
        _last_time = 0;
        _vaildtime = 0;
        _lastcde = 0.0f;
		_cde = 0.0f;
    }
    _this->WData->_fristtim = rt_tick_get_millisecond();    //更新运算时间
    _lastcde = _cde;
    _last_time = _vaildtime;
}
#endif

static void _Phaco_Update_CDE(Phaco_Task_t *_this,uint8_t _en)
{
    /*
     * 功能：根据纵振(_ltim)和扭振(_vtim)的周期配置，累计有效超乳时间和CDE。
     * 旧实现中扭振时间使用除法商值判断，导致同时存在_ltim/_vtim时，V段时间不能连续增长。
     * 新实现按完整周期数 + 当前周期内已运行时间分别计算L/V有效时间。
     */
    static uint32_t _runtime = 0;
    static uint32_t _last_time = 0;
    static float _lastcde = 0.0f;
    static float _cde = 0.0f;
    static uint32_t _vaildtime = 0;

    if(_en)
    {
        uint32_t _now = rt_tick_get_millisecond();
        uint32_t _footcntim = _now - _this->WData->_fristtim;
        uint32_t _ltim = _this->Wcof->_prams._ltim;
        uint32_t _vtim = _this->Wcof->_prams._vtim;
        uint32_t _stim = (_this->Wcof->_mode == PHACP_BRUST_MODE) ? _this->Wcof->_prams._cutetims : _this->Wcof->_prams._stim;
        uint32_t _cycle = _ltim + _vtim + _stim;
        uint32_t _ltime = 0;
        uint32_t _vtime = 0;
        float _lcde = 0.0f;
        float _vcde = 0.0f;

        _this->WData->_footcntim = _footcntim;

        /* _cycle为一个输出周期：纵振时间 + 扭振时间 + 停止/间歇时间。 */

        if(_cycle > 0)
        {
            /* 累计脚踏输出总时长，用它反推当前处于周期内的哪一段。 */
            _runtime += _footcntim;

            uint32_t _cnt = _runtime / _cycle;
            uint32_t _segall = _runtime % _cycle;

            /* 完整周期内，L/V都已经各自运行了固定时长。 */

            _ltime = _cnt * _ltim;
            _vtime = _cnt * _vtim;

            /* 当前未完成周期内，先经过L段，再经过V段，最后是停止/间歇段。 */

            if(_segall < _ltim)
            {
                _ltime += _segall;
            }
            else
            {
                _ltime += _ltim;

                /* 已越过L段，_vin_cycle表示当前周期内进入V段后的已运行时间。 */

                uint32_t _vin_cycle = _segall - _ltim;
                if(_vin_cycle < _vtim)
                    _vtime += _vin_cycle;
                else
                    _vtime += _vtim;
            }

            _lcde = (float)(_ltime * _this->WData->_lpwr * 0.00001f);
            _vcde = (float)(_vtime * _this->WData->_vpwr * 0.000004f);
        }

        /* _vaildtime只统计真正有能量输出的L/V时间，不包含停止/间歇段。 */

        _cde = (_lcde + _vcde) > 0 ? (_lcde + _vcde) : _lastcde;
        _vaildtime = _ltime + _vtime;

        float _increase = (float)(_cde - _lastcde);
        if(_increase > 0)
        {
            /* 只把本次新增的有效L/V输出时间写入手柄运行时间。 */
            ramdata._phacoruntime += (_vaildtime - _last_time);
            _this->_handpiece_runtime += (_vaildtime - _last_time);
            _this->WData->_pow += _increase;
        }
    }
    else
    {
        _runtime = 0;
        _last_time = 0;
        _vaildtime = 0;
        _lastcde = 0.0f;
        _cde = 0.0f;
    }

    /* 更新时间基准和上一次累计值，供下一次增量计算使用。 */
    _this->WData->_fristtim = rt_tick_get_millisecond();
    _lastcde = _cde;
    _last_time = _vaildtime;
}
static void __Phaco_Set_Pwr(uint8_t _lpow, uint8_t _vpow, uint16_t _ltime, uint16_t _vtime, uint16_t _stime)
{
    static uint8_t _flag = 0;
    last_out.lpow = _lpow;
    last_out.vpow = _vpow;
    last_out.ltime = _ltime;
    last_out.vtime = _vtime;
    last_out.stime = _stime;

    if(bsp_ultprotocol_ctl_power1(_phaco.ult_handler, _lpow, _vpow, _ltime, _vtime, _stime) == 0) //出错
    {
    //    _phaco.WData->_error.bits.com = 1;  //报错通信错误
    }
    else
    {
        _phaco.WData->_error.bits.com = 0;  //无错误
    }

    if(_lpow == 0 && _vpow == 0 && _flag == 1)
    {
        _flag = 0;
        _phaco.WData->_outenable = 0;   //无能量输出
        _phaco.WData->_footcntim = (rt_tick_get_millisecond() -  _phaco.WData->_fristtim);//获取停止时间
        _Phaco_Update_CDE(&_phaco,1);    //计算CDE
        _Phaco_Update_CDE(&_phaco,0);    //计算CDE

        _Phaco_Task_Handpiece_write_e2prom(&_phaco);    //写入e2prom
        
    }
    else if((_lpow + _vpow) > 0)
    {
        if(_flag == 0)
        {
            _phaco.WData->_fristtim = rt_tick_get_millisecond();    //获取停止时间
            _flag = 1;
            _phaco.WData->_outenable = 1;   //无能量输出
        }
    }
    else
    {

    }
}


void phaco_set_default(void)
{
	memcpy(_phaco.cal_param_t._lcali_tbl, &default_phaco_lcal_table, sizeof(phaco_cal_param_t));
	memcpy(_phaco.cal_param_t._vcali_tbl, &default_phaco_vcal_table, sizeof(phaco_cal_param_t));
}

int32_t  CaculTwoPoint(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x)
{
    return y1 + ((int64_t)(y2 - y1) * (x - x1)) / (x2 - x1);
}

uint32_t    phaco_solveX_v2(float _input, phaco_cal_param_t *_cali)
{
    uint32_t _ret;

    /* 输入值的返回值为0,不参与拟合计算 防止关不断*/
    if(_input <= 0.0f)
        return 0;

    uint32_t x1, x2, y1, y2 = 0;


    if(_input <= _cali->val[0].tag_power)
    {
        _ret = _cali->val[0].set_power;
    }
    else if(_input >= _cali->val[PHACO_POWER_MAX - 1].tag_power)
    {
        _ret = _cali->val[PHACO_POWER_MAX - 1].set_power;
    }
    else
    {
        for(uint8_t i = 0; i < PHACO_POWER_MAX; i++)
        {
            if(_input <= _cali->val[i].tag_power)
            {
                x1 = _cali->val[i].tag_power;
                y1 = _cali->val[i].set_power;
                x2 = _cali->val[i - 1].tag_power;
                y2 = _cali->val[i - 1].set_power;
                break;
            }
        }

        _ret = CaculTwoPoint(x1, y1, x2, y2, (int32_t)_input);

    }

    return _ret;
}


static void __Phaco_Mode_Func(Phaco_Task_t *_this, uint8_t _mode)
{
    switch (_mode)
    {
        case PHACO_PLUSE_MODE:
        case PHACP_BRUST_MODE:
        case PHACO_CONTINUOUS_MODE:
        {
            if(rt_memcmp(&_this->_localCof, &_this->Wcof->_prams, sizeof(Phaco_Prams_Cof)) || (_this->_prmode)!=_mode)
            {
                rt_memcpy(&_this->_localCof, &_this->Wcof->_prams, sizeof(Phaco_Prams_Cof));
                rt_enter_critical();
                if(_this->_percentofPedal > 0)
                {

                    #if PHACO_3D_CONFIG_DISABLE
                        int32_t _lpow = _this->Wcof->_prams._elpow - _this->Wcof->_prams._slpow;
                        int32_t _vpow = _this->Wcof->_prams._evpow - _this->Wcof->_prams._svpow;
                        App_Com_WriteRam(&_this->Wcof->_prams._lpow, (int32_t)((int32_t)(_lpow)*_this->_percentofPedal) / 100 + _this->Wcof->_prams._slpow);
                        App_Com_WriteRam(&_this->Wcof->_prams._vpow, (int32_t)((int32_t)(_vpow)*_this->_percentofPedal) / 100 + _this->Wcof->_prams._svpow);
                        App_Com_WriteRam(&_this->Wcof->_prams._cutetims, (2500 - ((2500 - _this->Wcof->_prams._stim)*_this->_percentofPedal) / 100));
                    #else
                        float _configDuty = (100.0f-_this->_percent_3d_config)/100.0f;
                        int32_t _lpow = _this->_percentofPedal<=_this->_percent_3d_config?_this->Wcof->_prams._slpow:((_this->Wcof->_prams._elpow - _this->Wcof->_prams._slpow)/_configDuty)*((_this->_percentofPedal-_this->_percent_3d_config)/100.0f)+_this->Wcof->_prams._slpow;
                        int32_t _vpow = _this->_percentofPedal<=_this->_percent_3d_config?_this->Wcof->_prams._svpow:((_this->Wcof->_prams._evpow - _this->Wcof->_prams._svpow)/_configDuty)*((_this->_percentofPedal-_this->_percent_3d_config)/100.0f)+_this->Wcof->_prams._svpow;
                        App_Com_WriteRam(&_this->Wcof->_prams._lpow, _lpow);
                        App_Com_WriteRam(&_this->Wcof->_prams._vpow, _vpow);
                        uint32_t _cutetims = _this->_percentofPedal<=_this->_percent_3d_config?2500:(2500 - (((2500 - _this->Wcof->_prams._stim)/_configDuty)*((_this->_percentofPedal-_this->_percent_3d_config)/100.0f)));
                        App_Com_WriteRam(&_this->Wcof->_prams._cutetims, _cutetims);
                    #endif
                    _this->WData->_cutetims = _this->Wcof->_prams._cutetims;
                }
                _Phaco_Update_CDE(&_phaco,1);    //计算CDE
                _Phaco_Update_CDE(&_phaco,0);    //计算CDE
                // 更新读寄存器
                _this->WData->_lpwr = (_mode == PHACO_CONTINUOUS_MODE?_this->Wcof->_prams._lpow:_this->Wcof->_prams._ltim==0?0: _this->Wcof->_prams._lpow);
                _this->WData->_vpwr = (_mode == PHACO_CONTINUOUS_MODE?_this->Wcof->_prams._vpow:_this->Wcof->_prams._vtim==0?0:_this->Wcof->_prams._vpow);
                if(_this->Wcof->_prams._lpow != 0)
                    //_this->_downlpwr   = bsp_abstractLineFunc_SolveX(_this->Wcof->_prams._lpow, &_this->_lcali);
                    _this->_downlpwr   = phaco_solveX_v2(_this->Wcof->_prams._lpow, _this->cal_param_t._lcali_tbl);
                else
                    _this->_downlpwr = 0;

                if(_this->Wcof->_prams._vpow != 0)
                    //_this->_downvpwr = bsp_abstractLineFunc_SolveX(_this->Wcof->_prams._vpow, &_this->_vcali);
                    _this->_downvpwr   = phaco_solveX_v2(_this->Wcof->_prams._vpow, _this->cal_param_t._vcali_tbl);
                else
                    _this->_downvpwr = 0;

                uint16_t _l  = (_this->Wcof->_prams._lpow != 0) ? _this->Wcof->_prams._ltim : 0;
                uint16_t _v = (_this->Wcof->_prams._vpow != 0) ? _this->Wcof->_prams._vtim : 0;


//                if (_mode == PHACO_CONTINUOUS_MODE)
//                {
//                    _this->Wcof->_prams._ltim = (_this->Wcof->_prams._lpow != 0) ? 20 : 0;
//                    _this->Wcof->_prams._vtim = (_this->Wcof->_prams._vpow!= 0)? 80 : 0;
//                    _this->Wcof->_prams._stim = 0;
//                }
                rt_exit_critical();


                if(_mode == PHACO_CONTINUOUS_MODE)
                {
                    // _this->Wcof->_prams._ltim = (_this->Wcof->_prams._lpow != 0) ? 20 : 0;
                    // _this->Wcof->_prams._vtim = (_this->Wcof->_prams._vpow!= 0)? 80 : 0;
                    // _this->Wcof->_prams._stim = 0;
                    __Phaco_Set_Pwr(_this->_downlpwr, _this->_downvpwr, (_this->Wcof->_prams._lpow != 0) ? 20 : 0, (_this->Wcof->_prams._vpow != 0) ? 80 : 0, 0);
                }
                else if(_mode == PHACP_BRUST_MODE)
                    __Phaco_Set_Pwr(_this->_downlpwr, _this->_downvpwr, _l, _v, _this->Wcof->_prams._cutetims);
                else
                    __Phaco_Set_Pwr(_this->_downlpwr, _this->_downvpwr, _l, _v, _this->Wcof->_prams._stim);

//                rt_memcpy(&_this->_localCof, &_this->Wcof->_prams, sizeof(Phaco_Prams_Cof));
            }

            _this->_prmode = _mode;
        }
        break;

        case PHACO_IDEL_MODE:
        {
            if(_this->_prmode != _mode) //第一次进入
            {
                if(_this->_prmode == PHACO_TEST_MODE) //测试模式中
                {
                    rt_memset(&_this->Wcof->_prams, 0, sizeof(Phaco_Prams_Cof));
                }

                _this->_prmode = _mode;
                __Phaco_Set_Pwr(0, 0, 0, 0, 0);
                _Phaco_Update_Percent(0);
                rt_memset(&_this->_localCof, 0, sizeof(Phaco_Prams_Cof));
            }
        }
        break;

        case PHACO_TEST_MODE:
            if(rt_memcmp(&_this->_localCof, &_this->Wcof->_prams, sizeof(Phaco_Prams_Cof))) //配置有更改
            {
                rt_memcpy(&_this->_localCof, &_this->Wcof->_prams, sizeof(Phaco_Prams_Cof));
                __Phaco_Set_Pwr(_this->Wcof->_prams._lpow, _this->Wcof->_prams._vpow, _this->Wcof->_prams._ltim, _this->Wcof->_prams._vtim, _this->Wcof->_prams._stim);
            }

            _this->_prmode = _mode;
            break;
        case PHACO_SCANFREQ_MODE:
        {
            if(rt_memcmp(&_this->_localCof,&_this->Wcof->_prams,sizeof(Phaco_Prams_Cof)) && _this->_handle_scan_sta !=0)
            {
                rt_memcpy(&_this->_localCof,&_this->Wcof->_prams,sizeof(Phaco_Prams_Cof));
                _this->_handle_scan_sta = 0;    //清除扫频状态
                Phaco_Task_SetScanState(0);
                _this->WData->_error._error = 0;    //清除错误
                bsp_ultprotocol_clear_error(_this->ult_handler);    //清除错误
                rt_thread_delay(100);
                bsp_ultprotocol_scan_Freq(_this->ult_handler);
								rt_thread_delay(100);
                _this->_prmode = _mode;
            }
        }
            break;
        default:
            break;
    }
}

static void _Phaco_Task_Handpiece_write_e2prom(Phaco_Task_t *_this)
{
    //这里更新手柄eeprom中的信息
    if(_phaco.handpice->_read_e2prom)
    {
        
        //计算当前手柄使用时间
        // _this->WData->_tag_info._tag.info.data.power.time += _this->WData->_footcntim/1000.0f;
        _this->WData->_tag_info._tag.info.data.power.time = _this->_handpiece_runtime/1000.0f+_this->_handpiece_loadRunTime;
        //计算当前手柄使用次数
        _this->WData->_tag_info._tag.info.data.power.cnt = _this->_handpiece_usecnt;
        // int32_t _increase = _this->WData->_sumpow - _this->_lastpwr;
        // _this->WData->_tag_info._tag.info.data.power.cde  +=  (_increase<0?_this->WData->_sumpow:_increase)/12500.0f; //每次只写入增量
        // _this->_lastpwr = _this->WData->_sumpow;

        float _increase = _this->WData->_pow - _this->_lastpwr;
        _this->WData->_tag_info._tag.info.data.power.cde  +=  (_increase<0?_this->WData->_pow:_increase); //每次只写入增量
        _this->_lastpwr = _this->WData->_pow;

        //写入手柄中
        _Phaco_HandPice1_Write_E2prom( _this->WData->_tag_info._tag.info.data.type,E2PROM_WRITE_OFFSET+offsetof(Handpiece_tag_t,info.data),sizeof(Handpiece_data_t));
    }
}

static void _Phaco_Task_UpdateInfo(Phaco_Task_t *_this)
{

    if( _this->WData->_error.bits.com == 0 ) //无通信错误
    {
        if(_phaco.Wcof->_mode==PHACO_SCANFREQ_MODE){ //扫频中
            if(_this->_handle_scan_sta ==0)
            {
                _this->_handle_scan_sta = bsp_ultprotocol_query_handle_det_ready(_this->ult_handler);
                if(_this->_handle_scan_sta>=1) //扫频完成 1 扫频成功  2:扫频失败
                {
                    _this->Wcof->_mode = PHACO_IDEL_MODE;   //扫频完成后将模式切换为IDLE模式
                }
                    
            }        						 
        }
        _this->WData->_error._error = bsp_ultprotocol_query_Error(_this->ult_handler);
        if (_this->WData->_outenable)   //输出使能的时候才进行功率采集
        {
            // __Phaco_Set_Pwr(last_out.lpow,last_out.vpow,last_out.ltime,last_out.vtime,last_out.stime);

            _Phaco_Update_CDE(_this,1);    //计算CDE
            // _this->WData->_pow =        bsp_ultprotocol_query_Power(_this->ult_handler);
            float _temppwr = bsp_ultprotocol_query_SumPower(_this->ult_handler);
            _this->WData->_sumpow =     _temppwr==0?_this->WData->_sumpow:_temppwr;
        }
        
    }
}

void    Phaco_Task_ResetPwr()
{
    bsp_ultprotocol_reset_Sumpower(_phaco.ult_handler);
}

static void _Phaco_Task_ErrorHandler(bsp_ult_error *_error)
{
    if(_error->_error)  //有错误
    {
        if(_error->bits.com == 1)     //通信错误
        {
            bsp_error_list_add(PHACP_COM_ERROR);
						_error->bits.com  = 0;
        }
        else
        {
            if (_error->bits.impedance == 1)     //阻抗错误
            {
                bsp_error_list_add(PHACO_IMPEDANCE_ERROR);
            }

            if (_error->bits.needle == 1)        //未检出到针头
            {
                bsp_error_list_add(PHACO_NEEDLE_ERROR);
            }

            if (_error->bits.transducer == 1)    //换能器错误
            {
                bsp_error_list_add(PHACO_TRANSDUCER_ERROR);
            }

//            if(bsp_ultprotocol_reset_Error(_phaco.ult_handler) == 0)    
//            {   
//            //    _error->bits.com = 1;     //复位错误指令不回消息 这条需要屏蔽
//                _error->_error = 0;     //手动清空所有错误
//            }
						
						if(bsp_ultprotocol_clear_error(_phaco.ult_handler)==0)
						{
								_error->_error = 0;     //手动清空所有错误
						}
            else    //清除错误成功
            {
                _error->_error = 0;     //手动将所有错误都清除
            }
        
        }

        Phaco_Pedal_Disconnect();
    }
}

static void Phaco_Task_Connect_Detected(Phaco_Task_t * _this)
{
    if (_this->WData->_conStatus)   //当前手柄已经连接
    {
        if(!_this->handpice[_this->WData->_conStatus - 1]._get_state()) //断开连接
        {
            _this->handpice[_this->WData->_conStatus - 1]._disconnected();
            _this->WData->_conStatus = 0;
        }
    }
    else            //手柄没有连接
    {
        for(uint8_t i = 0; i < (sizeof(_this->handpice) / sizeof(_this->handpice[0])); i++)
        {
            if(_this->handpice[i]._get_state != RT_NULL)
            {
                if(_this->handpice[i]._get_state())       //手柄1第一次连接
                {
                    _this->WData->_conStatus = i + 1; //手柄连接
                    _this->handpice[i]._connected();
                    return;     //只检测一个端口
                }
            }
        }

    }
}

static void _Phaco_HandPice1_SetColor(uint32_t _rgb)
{

    LED_COLOR_32(color, LED_COLOR_RGB2GRB(_rgb));
    bsp_ws2812b_Ctl_AllColor(&ws1, color);
}

#define PHACO_CONNECT_PIN  GET_PIN(D,0)
static uint8_t _Phaco_HandPice1_GetConnect()
{
    uint8_t _ower[4];
    uint8_t _state = 0;
    static struct connect_state {
        uint8_t _state;
        uint8_t _connect_cnt;
    }phaco_connct = {
        ._state = 0,
       ._connect_cnt = 0
    };
#if 1

    if(_phaco.handpice->_read_e2prom)
    {
        _phaco.handpice->_read_e2prom(_ower,E2PROM_READ_OFFSET, 4);
        if(memcmp(_ower, _default_tag.info.ower, 4) == 0) //相等
        {
            _state = 1;
            phaco_connct._state = 1;
            phaco_connct._connect_cnt = 0;
        }
        else
        {
            if(phaco_connct._state == 1)	
            {
                phaco_connct._connect_cnt++;
                if(phaco_connct._connect_cnt >= 5) //连续检测10次
                {
                    phaco_connct._state = 0;
                    phaco_connct._connect_cnt = 0;
                    _state = 0;
                }
                else
                {
                    _state = 1;
                }
            }
            else
            {
                _state = 0;
            }
        }
    }

#else
    _state = !(rt_pin_read(PHACO_CONNECT_PIN));
#endif
    return _state;
}

static void _Phaco_HandPice1_Read_E2prom(uint8_t *buf, uint16_t _offset,uint16_t _len)
{
    bsp_m95512_Read(e2p1, E2PROM_READ_OFFSET+_offset, buf, _len);
}

static void _Phaco_HandPice1_Write_E2prom(uint8_t *buf,uint16_t _offset, uint16_t _len)
{
    bsp_m95512_Write(e2p1, E2PROM_READ_OFFSET+_offset, buf, _len);
}


static void _Phaco_HandPice1_Connected()
{
    // 1.从e2prom中读取完整的手柄信息存入内存中
    if(_phaco.handpice->_read_e2prom)
    {
        // _phaco.handpice->_read_e2prom(_phaco.WData->_tag_info._tag.buf, sizeof(Handpiece_tag_t));
        _phaco.handpice->_read_e2prom(_phaco.WData->_tag_info._tag.buf,E2PROM_WRITE_OFFSET, sizeof(Handpiece_tag_t));
    }

    // 2.设置灯的颜色
    // _Phaco_HandPice1_SetColor(ACTIVE_COLOR);
    // _this->Wcof->_rgb_cof[i]
    // _Phaco_HandPice1_SetColor((_phaco.Wcof->_rgb_cof[0]&0x00ffffff));
    
    // 3.设置当前的protk口
    bsp_ultprotocol_reset_Error(_phaco.ult_handler);    //复位错误
    rt_thread_delay(100);   //插入时复位等待100ms
    bsp_ultprotocol_change_Port(_phaco.ult_handler, 1);
    // 4.更新读寄存器
    _phaco.WData->_useport = 1;

    // 5.清空功率
    _phaco.WData->_sumpow = 0;
    _phaco.WData->_pow = 0;
    _phaco._lastpwr = 0;    //清空上次功率
    // 6.增加使用次数
    // _phaco.WData->_tag_info._tag.info.data.power.cnt++;
    _phaco._handpiece_usecnt = _phaco.WData->_tag_info._tag.info.data.power.cnt;
    _phaco._handpiece_loadRunTime = _phaco.WData->_tag_info._tag.info.data.power.time; //清空手柄加载时运行时间
    _phaco._handpiece_runtime = 0; //清空手柄运行时间
    _phaco._handpiece_usecnt++;
}
static void _Phaco_HandPice1_Disconnected()
{
    // 1.清除内存中的手柄信息存入内存中
    if(_phaco.handpice->_read_e2prom)
    {
        rt_memset(_phaco.WData->_tag_info._tag.buf, 0, sizeof(Handpiece_tag_t));
    }
    _phaco._handpiece_usecnt = 0;
    // 2.设置灯的颜色
    // _Phaco_HandPice1_SetColor(UNACTIVE_COLOR);

    // 3.断开输出
    _phaco.Wcof->_footcof = FOOTBOARD_ZERO; //不响应脚踏防止脚踏踩下有输出
    _Phaco_Update_Percent(0);
    _phaco.WData->_lpwr = 0;    //清除功率
    _phaco.WData->_vpwr = 0;    //清除功率
    Priming_Task_StopPhacoTest(); //断开超乳测试
}

// static void _Phaco_Task_LedCheck(Phaco_Task_t * _this)
// {
//     if(!_this->WData->_conStatus)   //没连接的时候才会进行检测
//     {
//         for(uint8_t i = 0; i < sizeof(_this->Wcof->_rgb_cof) / sizeof(_this->Wcof->_rgb_cof[0]); i++)
//         {
//             if(!(_this->Wcof->_rgb_cof[i] & 0xFD000000))
//             {
//                 if (_this->handpice[i]._set_color != RT_NULL)
//                 {
//                     _this->handpice[i]._set_color(_this->Wcof->_rgb_cof[i]);
//                 }

//                 _this->Wcof->_rgb_cof[i] |= 0xFD000000;
//             }
//         }
//     }
// }

static void _Phaco_Task_LedCheck(Phaco_Task_t * _this)
{
    // if(!_this->WData->_conStatus)   //没连接的时候才会进行检测
    // {
        static grb_t rgbbuf[10];
        if((ramcof._led_enable))   //有灯更改
        {
            // grb_t rgbbuf[10];
            for(uint8_t i = 0; i < 10; i++)
            {
                if(!(ramcof._led_rgb_cof[i] & 0xFD000000))
                {
                    LED_COLOR_32(color, LED_COLOR_RGB2GRB(ramcof._led_rgb_cof[i]));
                    // bsp_ws2812b_Ctl_SColor(&ws1, i, color);
                    rgbbuf[i] = color;
                    ramcof._led_rgb_cof[i] |= 0xFD000000;
                }
            }
            bsp_ws2812b_Ctl_RangeColor(&ws1, 0, 9, rgbbuf);
            ramcof._led_enable = 1;
        }
    // }
}


int Phaco_Task_Init(void)
{
    OS_ERR err;
    _phaco.ult_handler = &ult_handler;
    Phaco_Task_SetScanState(-1);
    if(bsp_ultprotocol_reset_Error(_phaco.ult_handler) == 0)
    {
//        _phaco.WData->_error.bits.com = 1;  //报错通信错误
    }

    _phaco.handpice[0]._connected = _Phaco_HandPice1_Connected;
    _phaco.handpice[0]._disconnected = _Phaco_HandPice1_Disconnected;
    _phaco.handpice[0]._get_state = _Phaco_HandPice1_GetConnect;
    _phaco.handpice[0]._set_color = _Phaco_HandPice1_SetColor;
    _phaco.handpice[0]._read_e2prom = _Phaco_HandPice1_Read_E2prom;

    // 超乳模块默认使能踏板
    ramcof.footcof = FOOTBOARD_LINE_ONE;  //响应第三段这个是响应总的踏板

    Phaco_Task_SetWcof(&ramcof.phacocof);
    Phaco_Task_SetData(&ramdata.phaco_data);


    _phaco.Wcof->_detectionConn = 0;    //使能超乳检测
    _phaco._handpiece_usecnt = 0; //初始化手柄使用次数
    _phaco.Wcof->_rgb_cof[0] = ACTIVE_COLOR|0xFD000000;



    _phaco._percent_3d_config = 15; //默认3D脚踏配置为10%
    _phaco._handpiece_runtime = 0; //清空手柄运行时间
    // 初始化插入检测
    rt_pin_mode(PHACO_CONNECT_PIN, PIN_MODE_INPUT_PULLUP);

//    // 设置响应参数  纵振线性参数
    _phaco._lcali.A = 2.7778;
    _phaco._lcali.B = 0;
    _phaco._lcali.C = 0;

    // 扭振线性参数
    _phaco._vcali.A = 2.0;
    _phaco._vcali.B = 0;
    _phaco._vcali.C = 0;

    // 初始化e2prom
    e2p1 =  bsp_m95512_create_3_soft_spi(E2P1_SSPI_BUS, E2P1_SSPI_NAME, 256 * 1024, 64, E2P1_SSPI_CS);

    if(e2p1 == RT_NULL)
        LOG_E("Create e2prom1 Faild");
    else
    {
#ifdef E2PROM_WRITE_HANDPIECE
        // bsp_m95512_Write(e2p1, E2PROM_READ_OFFSET, _default_tag.buf, sizeof(Handpiece_tag_t));
        bsp_m95512_Write(e2p1, E2PROM_WRITE_OFFSET, _default_tag.buf, sizeof(Handpiece_tag_t));
#endif
#ifdef E2PROM_WRITE_INFINITOR
        bsp_m95512_ContiueWrite(e2p1, 0, (uint8_t*)inifinte_data, 1024);
#endif // DEBUG
    }
		
		Flash_PhacoParam_init();
    Phaco_Task_ResetPwr();
    Pedal_Task_RegisterFun(Phaco_Pedal_Deal);
    OSTaskCreate( (OS_TCB     *) &Phaco_TaskTCB,
                  (CPU_CHAR   *) "phaco_task",
                  (OS_TASK_PTR ) Phaco_Task,
                  (void       *) 0,
                  (OS_PRIO     ) PHACO_TASK_PRIO,
                  (CPU_STK    *) &Phaco_TASK_STK[0],
                  (CPU_STK_SIZE) PHACO_TASK_STK_SIZE / 10,
                  (CPU_STK_SIZE) PHACO_TASK_STK_SIZE,
                  (OS_MSG_QTY  ) 10,
                  (OS_TICK     ) 0,
                  (void       *) 0,
                  (OS_OPT      ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                  (OS_ERR     *) &err);
}
INIT_APP_EXPORT(Phaco_Task_Init);

void Phaco_Task(void *arg)
{
	
		rt_thread_delay(500);
			//这里需要判断版本号读取问题
		uint32_t _DspVersion = bsp_ultprotocol_query_dspVersion(_phaco.ult_handler);
		if(0 !=_DspVersion )
		{
			ramdata.fw1.data.FWnum1 = _DspVersion;
		}
		else	//这里再读一遍
			ramdata.fw1.data.FWnum1 = bsp_ultprotocol_query_dspVersion(_phaco.ult_handler);		//获取DSP版本号
		
		
		uint32_t  _FPGAVersion = bsp_ultprotocol_query_fpgaVersion(_phaco.ult_handler);
		if(0 != _FPGAVersion)
		{
			ramdata.fw1.data.FWnum2 = _FPGAVersion;
		}
		else
			ramdata.fw1.data.FWnum2 = bsp_ultprotocol_query_fpgaVersion(_phaco.ult_handler);  //获取FPGA版本号
    while (1)
    {
        if(_phaco.Wcof->_detectionConn) //检测手柄
        {
            Phaco_Task_Connect_Detected(&_phaco);

            if((_phaco.WData->_conStatus))
            {
                __Phaco_Mode_Func(&_phaco, _phaco.Wcof->_mode); //可以根据配置直接操作
            }
        }
        else                            //不进行手柄检测
        {
            __Phaco_Mode_Func(&_phaco, _phaco.Wcof->_mode); //可以根据配置直接操作
        }

        if(_phaco.Wcof->_mode != PHACO_IDEL_MODE)
        {
            _Phaco_Task_UpdateInfo(&_phaco);    //功率读取这里需要注意这个版本有问题
        }

        _Phaco_Task_ErrorHandler(&_phaco.WData->_error);
        _Phaco_Task_LedCheck(&_phaco);
        rt_thread_delay(5);
    }
}

void Phaco_Task_SetWcof(Phaco_Task_Cof *_cof)
{
    RT_ASSERT(_cof);
    _phaco.Wcof = _cof;
}

void Phaco_Task_SetData(Phaco_Task_Data *_data)
{
    RT_ASSERT(_data);
    _phaco.WData = _data;
}

static void _Phaco_Update_Percent(uint32_t _percent)
{


    _phaco._percentofPedal = (_percent > 100) ? 100 : _percent;

    
    if(_phaco._percentofPedal)
    {
    #if PHACO_3D_CONFIG_DISABLE
        int32_t _lpow = _phaco.Wcof->_prams._elpow - _phaco.Wcof->_prams._slpow;
        int32_t _vpow = _phaco.Wcof->_prams._evpow - _phaco.Wcof->_prams._svpow;
        App_Com_WriteRam(&_phaco.Wcof->_prams._lpow, (int32_t)((int32_t)(_lpow)*_phaco._percentofPedal) / 100 + _phaco.Wcof->_prams._slpow);
        App_Com_WriteRam(&_phaco.Wcof->_prams._vpow, (int32_t)((int32_t)(_vpow)*_phaco._percentofPedal) / 100 + _phaco.Wcof->_prams._svpow);
        App_Com_WriteRam(&_phaco.Wcof->_prams._cutetims, (2500 - ((2500 - _phaco.Wcof->_prams._stim)*_phaco._percentofPedal) / 100));
    #else

        float _configDuty = (100.0f-_phaco._percent_3d_config)/100.0f;
        int32_t _lpow = _phaco._percentofPedal<=_phaco._percent_3d_config?_phaco.Wcof->_prams._slpow:((_phaco.Wcof->_prams._elpow - _phaco.Wcof->_prams._slpow)/_configDuty)*((_phaco._percentofPedal-_phaco._percent_3d_config)/100.0f)+_phaco.Wcof->_prams._slpow;
        int32_t _vpow = _phaco._percentofPedal<=_phaco._percent_3d_config?_phaco.Wcof->_prams._svpow:((_phaco.Wcof->_prams._evpow - _phaco.Wcof->_prams._svpow)/_configDuty)*((_phaco._percentofPedal-_phaco._percent_3d_config)/100.0f)+_phaco.Wcof->_prams._svpow;
        App_Com_WriteRam(&_phaco.Wcof->_prams._lpow, _lpow);
        App_Com_WriteRam(&_phaco.Wcof->_prams._vpow, _vpow);
        uint32_t _cutetims = _phaco._percentofPedal<=_phaco._percent_3d_config?2500:(2500 - (((2500 - _phaco.Wcof->_prams._stim)/_configDuty)*((_phaco._percentofPedal-_phaco._percent_3d_config)/100.0f)));
        App_Com_WriteRam(&_phaco.Wcof->_prams._cutetims, _cutetims);
    #endif  //这里屏蔽的是老的版本脚踏控制，新的版本脚踏控制是根据3D脚踏配置来控制的
    }
    else
    {
        App_Com_WriteRam(&_phaco.Wcof->_prams._lpow, 0);
        App_Com_WriteRam(&_phaco.Wcof->_prams._vpow, 0);
        App_Com_WriteRam(&_phaco.Wcof->_prams._cutetims, 2500);
        App_Com_WriteRam(&_phaco.WData->_cutetims, 0);  //脚踏回零的时候关断时间清零

    }

    // 更新读寄存器(因为增加CDE把这里注释掉试一下
    // _phaco.WData->_lpwr = _phaco.Wcof->_prams._lpow;
    // _phaco.WData->_vpwr = _phaco.Wcof->_prams._vpow;
}

void Phaco_Pedal_Deal(uint16_t _code, uint32_t _data)
{
    uint16_t _cmd = _phaco.Wcof->_footcof;      //获取当前超乳踏板的响应配置
    uint32_t _duty = 0;

    _code = BSWAP_16(_code);

    if(_code > FOOTBOARD_SWITCH) //清除开关分量
    {
        _code -= FOOTBOARD_SWITCH;
        _data &= 0xFFFF;
    }

    if(_cmd)   //踏板可控制
    {
        switch (_cmd)
        {
            case FOOTBOARD_LINE_ONE:   //响应踏板第一阶段
            case FOOTBOARD_LINE_SECOND: //响应踏板第二阶段
            case FOOTBOARD_LINE_THIRD:
            {
                // if(_code == _cmd)   //参数匹配
                // {
                //     _duty = _data;
                // }
                if(_code > _cmd && _code < FOOTBOARD_SWITCH) //超过所设置的段就最大输出
                    _duty = 100;
                else
                {
                    _duty = (_code == _cmd) ? (_data>100?100:_data) : 0;
                }
            }
            break;

            case FOOTBOARD_POS1:   //响应踏板第一阶段
            case FOOTBOARD_POS2:   //响应踏板位置2
                if(_code == FOOTBOARD_POS1)   //参数匹配
                {
                    _duty = 20;
                }
                else
                {
                    _duty = 50;
                }

                break;

            default:
                break;
        }

        if(_code == FOOTBOARD_ZERO) //踏板回到零点
            _duty = 0;

        if(_code > _cmd && _code < FOOTBOARD_SWITCH) //超过所设置的段就最大输出
            _duty = 100;

        _Phaco_Update_Percent(_duty);
    }

}

void Phaco_Pedal_Disconnect()
{
    _Phaco_Update_Percent(0);
}

void Phaco_Task_ShutDown()
{
    Phaco_Pedal_Disconnect();
    _Phaco_HandPice1_SetColor(UNACTIVE_COLOR);
}

uint8_t Phaco_Task_get_Connection()
{
    return _phaco.WData->_conStatus;
}

uint8_t Phaco_Task_GetScanState()
{
    return _phaco._handle_scan_sta;
}
uint8_t Phaco_Task_SetScanState(int32_t _state)
{
    _phaco._handle_scan_sta = _state;
}

void Phaco_Task_SetMode(enum PHACO_MODE _phacoMode,Phaco_Prams_Cof _prams)
{
    if (_phaco.Wcof != NULL)
    {
        rt_enter_critical();
        _phaco.Wcof->_mode = _phacoMode;
        rt_memcpy(&_phaco.Wcof->_prams,&_prams,sizeof(Phaco_Prams_Cof));
        rt_exit_critical();
    }
}
