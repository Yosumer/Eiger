/*------------------------------------------------------------------------------
 * @file    BASECONFIG.H
 * @author  SIERRA TEAM
 * @date    2024/04/09 13:15:08
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
#ifndef __EIGERCONFIG_H
#define __EIGERCONFIG_H

/* Files includes  -----------------------------------------------------------*/
#include <stdint.h>
#include <rtthread.h>
#include "valve_task.h"
#include "LiquidSubCof.h"
#include "AirSubCof.h"
#include "OptSubCof.h"

#include "app_barcode.h"
#include "app_instruction_com.h"

/* Defines -------------------------------------------------------------------*/
#ifndef __IO
#define __IO volatile /*!< Defines 'read / write' permissions */
#endif
#ifndef __O
#define __IO volatile /*!< Defines 'read / write' permissions */
#endif
#ifndef __I
#define __I volatile const /*!< Defines 'read only' permissions */
#endif
#define Delay_ms(x) rt_thread_delay(x)

#define BSWAP_16(x) ((((x) & 0xFF00) >> 8) | (((x) & 0x00FF) << 8))

#define BSWAP_32(x)                                                            \
  ((((x) & 0xFF000000) >> 24) | (((x) & 0x00FF0000) >> 8) |                    \
   (((x) & 0x0000FF00) << 8) | (((x) & 0x000000FF) << 24))

/*内存配置块 ----------------------------------------------------------------*/
#define RAM2_BASE 0X2001C000UL

#define CONTROL_BASE 0XE00
#define ERROR_BASE 0XFF0
#define FLASH_RAM_BASE 0XFF4
#define OFFSET_BASE 0X30
#define CONFIG_BASE 0X1000
#define DATA_BASE 0X2000
#define MAX_BASE 0X3000

#define CAN_TEMP RT_SECTION(".TEMP") // 临时段 在Flash模式中会使用
#define CAN_CONTROL RT_SECTION(".CONTROL") // 控制段
#define CAN_ERROR RT_SECTION(".ERROR")     // 错误段
#define CAN_CONFIG RT_SECTION(".CONFIG")   // 配置段
#define CAN_DATA RT_SECTION(".DATABLOCK")  // 数据段
#define NO_INIT RT_SECTION("NO_INIT") // 不初始化字段16个字节 用来存放升级标志问
#define MEMPOOL RT_SECTION(".MEM") // 内存管理区域 1K

#define CONTROL ((Control_Block *)(RAM2_BASE + CONTROL_BASE))
#define ERROR ((Error_Block *)(RAM2_BASE + ERROR_BASE))
#define CONFIG ((Config_Block *)(RAM2_BASE + CONFIG_BASE + OFFSET_BASE))
#define DATA ((Data_Block *)(RAM2_BASE + DATA_BASE + OFFSET_BASE))
#define FLASH_ADDR ((RAM2_BASE + ERROR_BASE + 4))
/*模块定义----------------------------------------------------------------*/
#define CURRENT_MODULE_ID EIGER_MODULE_ID
#define MONITOR_MODULE_ID 0X02
#define LIQUID_MODULE_ID 0X07
#define PEDAL_MODULE_ID 0XA5
#define AIR_MODE_ID 0x0C
#define LASER_MODULE_ID 0X03
#define PHOTOELECTRIC_MODE_ID 0X08
#define ELECT_MODE_ID 0X0E
#define PHACO_MODE_ID 0X0F

#define PEDAL_MODULE_CtlID 0X0D
#define EIGER_MODULE_ID 0XE5
#define EIGER_IV_MODULE_ID 0XE6

// 其他模块请自行定义

/*模式定义----------------------------------------------------------------*/
#define FLASH_MODE 0x0906     // Flash 模式
#define PROJECT_MODE 0X060D   // 工程模式
#define NORMAL_MODE 0X020C    // 正常模式
#define SELFCHECK_MODE 0X0009 // 自检模式
#define NOCONFIG_MODE 0X070F  // 配置文件缺失
// 其他模块请自行定义
/*指令模式----------------------------------------------------------------*/
#define GLOBAL_INSTRUCTION 0XE00     // 全局指令
#define SIGNAL_INSTRUCTION 0XE01     // 单指令
#define BOOTLOADER_INSTRUCTION 0XE0F // bootloader 指令
#define REBOOT_INSTRUCTION 0XE10     // 固件切换指令

/*升级标志位*/
#define UPDATE_FLAG_ENABLE 0XA5F5 // 升级标志位
/*单指令表大小*/
#define INSTRUCTION_CMD_SIZE 100

/*全局使能*/
#define GLOBALMsg_Q_NUM 8 // 全局接收消息队列的数量

/*错误块内存大小*/
#define ERROR_CODE_MAX_NUM 64

/*心跳包时间间隔*/
#define HEART_PER_TICK 500 // 心跳包发送间隔

/*Flash相关操作*/
enum {
  FLASH_CODE_DEV = 0X0000, // 设备信息
  FLASH_CODE_FW1,          // APP固件信息
  FLASH_CODE_FW2,          // 出厂固件信息
  FLASH_CODE_PID,          // PID校准信息
  FLASH_CODE_SENSOR,       // 传感器校准信息
  FLASH_CODE_MOTOR,        // 电机相关信息
  FLASH_CODE_USER,         // 用户校准信息
  FLASH_CODE_MAX
};

#define FLASH_CTL_LOAD 0X0001 // 从Flash中加载内存
#define FLASH_CTL_STR 0X0002  // 将内存中数据写入Flash
#define Flash_CTL_READ 0X0003 // 连续读取内存

#define FLASH_STATUS_IDEL 0 // Flash空闲
#define FLASH_STATUS_BUSY 1 // Flash忙

#define INSTRUCT_CAL_USE 1U

/* Variables -----------------------------------------------------------------*/

typedef struct _CanData_t {
  uint8_t dir;   // 1:代表写 2:代表读
  uint8_t id;    // 模块ID
  uint16_t addr; // 地址段模式
  uint32_t data; // 数据
} CanData_t;     // CAN数据结构

typedef struct _OTA_Task_Cof {
  uint32_t _module; // 升级模块
  uint32_t _cmd;    // 升级命令
} OTA_Task_Cof;

// 不初始化段
typedef struct {
  uint32_t _updateflag; // 0:当前未升级   1:当前有新版本 需要在Boot中进行判断
} NoInit_Block;


//控制块定义
typedef struct {
    __IO uint32_t    enable;                    //全局失能开关
    __IO uint32_t    singleInstruction;         //单指令开关
    __IO uint32_t    parms[12];                 //单指令参数12个
    __IO uint32_t    bootmode;                  //bootload模式      0：退出 1:进入boot模式
    __IO uint32_t    dowmcmd;                   //固件下载指令
    __IO uint32_t    reboot;                    //复位指令          0x01 系统软复位   
    struct {                                    //批量读只允许读数据寄存器
        __IO uint32_t bat_enable;               //批量读使能  
        __IO uint32_t bat_S_addr;               //批量读起始地址
        __IO uint32_t bat_len;                  //批量读长度     
    }bat;
    uint32_t    heartEn;                        //心跳包使能
    OTA_Task_Cof _ota_cof;                       //升级配置
    uint32_t    _unuse;                          //没有使用的字段
}Control_Block;

//错误块定义
typedef union
{
    __IO uint32_t    errorbuf[4];       //读buff
    struct
    {
        __IO uint32_t    select;         // 写时有效翻页操作          -0x0000 上一页  -0x0001 下一页
    }control;
}Error_Block;
//Flash操作块
typedef union 
{
    __IO uint32_t    buf[12];       
    struct
    {
        __IO uint32_t IDcode;        //Flash模式下需要操作的code
        __IO uint32_t ctl;           //Flash操作模式  -0x0001 加载指定段到内存 -0x0002 写入内存到指定段 -0x0003 批量读取全部内容
        __IO uint32_t status;        //Flash状态寄存器   -0x0000 Flash空闲状态 -0x0001 Flash忙状态
    }control;
}Flash_Block;


//控制模块
typedef struct
{
    __IO uint32_t    config;                     //-正常模式：0x020C、-工程模式：0x060D 、-Flash模式：0x0906
    //各模块根据自己的配置表进行填写
    __IO uint32_t   footcof;                     //本模块踏板响应
    Fan_Task_Conf       fancof; 
    Phaco_Task_Cof      phacocof;                   //超乳配置  //(0x1011)
    Diathermy_Task_Cof  diacof;                     //电凝配置  //(0x1017)
    Pneumatic_Task_Cof  aircof;                     //气路配置
    Infusion_Task_Cof   _inf_cof;                   //灌注配置
    Suction_Task_Cof    _suctioncof;                //抽吸模式
    Reflux_Task_Cof    _refluxcof;                  //回流配置
    Priming_Task_Cof    _primingcof;
    SystemCheck_Task_Cof _systemCheckCof;
    uint32_t    _led_rgb_cof[10];                    //2个手柄的RGB灯  
    uint32_t    _led_enable;                         //LED使能
    uint32_t    _pedaldetectCof;                     //踏板检测
		uint32_t    _set_zero;													 //积液盒负压清零
    uint32_t    _is_cassette_detected;               //积液盒是否检测  
    uint32_t    _is_dia_error_detected;             //电凝错误检测
    uint32_t    _unuse;                        //没有使用的字段
}Config_Block;
//序列号
typedef union {
  uint32_t word[8];
  struct {
    uint32_t num1;
    uint32_t num2;
    uint32_t num3;
    uint32_t num4;
    uint32_t num5;
    uint32_t num6;
    uint32_t num7;
    uint32_t num8;
  } data;
} SerialNum; // 序列号
// 固件相关信息
typedef union {
  uint32_t word[12];
  struct {
    uint32_t FWnum1;     // 固件号1
    uint32_t FWnum2;     // 固件号2
    uint32_t FWnum3;     // 固件号3
    uint32_t FWnum4;     // 固件号4
    uint32_t FWversion1; // 固件1级版本v
    uint32_t FWversion2; // 固件2级版本1
    uint32_t FWversion3; // 固件3级版本0
    uint32_t FWversion4; // 固件4级版本0
    uint32_t FWversion5; // 固件5级版本
    uint32_t FWyear;     // 固件年
    uint32_t FWmon;      // 固件月
    uint32_t FWday;      // 固件日
  } data;
} FwHard; // 固件相关信息
typedef struct {
  uint32_t VersionMain; // 校准文件主版本
  uint32_t VersionSub;  // 校准文件子版本
} Calibration;
typedef struct {
  FwHard fw; // 固件1信息
} Flash_Fw;

// 数据块
typedef struct {
  __IO uint32_t
      mode; // 0x0000 空闲未配置,-0x0001 自检中,-0x0002 正常模式,-0x0003
            // 工程模式,-0x0004 FLASH模式,-0x0005 ERROR
  SerialNum serial; // 序列号
  FwHard fw1;       // 固件1相关信息
  FwHard fw2;       // 固件2相关信息
  uint32_t runapp; // 当前设备运行的固件    0：代表使用固件1    1：代表使用固件2
  Calibration calibration; // 校准文件版本信息

  Valve_Task_t _valve;
  Fan_Task_Data fan_data;
  Phaco_Task_Data phaco_data;   // 超乳模块数据
  Diathermy_Task_Data dia_data; // 电凝模块数据
  Pneumatic_Task_Data air_data; // 气路模块数据

  struct {
    uint32_t _state : 2; // 1:CASSETTE_UNLOAD       2:CASSETTE_LOAD 3:CASSETTE_Test
    uint32_t _release_sig : 1; // 退盒按键状态    1:触发  0:未触发
    uint32_t _release_left : 1; // 退盒左光电开关到位   1:触发  0:未触发
    uint32_t _release_right : 1; // 退盒右光电开关到位   1:触发  0:未触发
    uint32_t _iv_is_connect : 1;  // IV模块连接状态  1:连接  0:未连接
    uint32_t _pen_is_connect : 1; // 踏板是否连接  1:连接  0:未连接
    uint32_t _pen_is_press_1:1;  //脚踏按压一档未释放
    uint32_t : 24;
  } _cassette_state; // 0x87

  Infusion_Task_Data _infusion; // 灌注
  Suction_Task_Data _suctionData;
  Refluex_Task_Data _refluxData;
  Priming_Task_Data _primingData;
  
  uint32_t _pwrstaus; // 外设供电状态
  SystemCheck_Task_Data   _systemCheckStatus; //自检状态

  project_user_data_t project_data; // 工程模式数据
  struct {
    phaco_cal_param_t _lcali;
    phaco_cal_param_t _vcali;
  } phaco_pow_tbl; // 当前使用的超乳功率表格
  
  dia_cal_param	_diaCalData;	//电凝校准信息
  uint32_t    _phacoruntime;

  float    _N_torque; //单位N/m 扭矩
  uint32_t _unuse;    // 没有使用的字段
} Data_Block;

typedef struct {
  SerialNum serial;        // 序列号
  Calibration calibration; // 校准文件版本
} Flash_DeviceInfo;

#ifndef USE_FLASH_PID // Flash中没使用PID字段
struct Flash_PID {
  uint32_t _unuse;
};
#endif
#ifndef USE_FLASH_SENSOR // Flash中没使用PID字段
struct Flash_Sensor {
  CalibrationPara _vCal;   //
  CalibrationPara _iCal;   //
  CalibrationPara _outCal; //
  float _outMax;
  uint32_t _diaFreq;
  uint32_t _unuse;
};
#endif
#ifndef USE_FLASH_MOTOR // Flash中没使用PID字段
struct Flash_Motor {
  CalibrationPara _pumpMotor;       //蠕动泵校准参数
  uint32_t _unuse;
};
#endif
#ifndef USE_FLASH_USER // Flash中没使用PID字段
struct Flash_User {
  phaco_cal_param_t _phaco_iCal; //
  phaco_cal_param_t _phaco_vCal; //
  uint32_t _unuse;
};
#endif

typedef struct {
  Flash_DeviceInfo _devinfo;
  FwHard _app;
  FwHard _factory;
  struct Flash_PID _pid;
  struct Flash_Sensor _sensor;
  struct Flash_Motor _motor;
  struct Flash_User _user;
} Flash_cof;

/* extern----------------------------------------------------------------*/
extern Control_Block ramctl; // 全局控制块
extern Error_Block ramerr;   // 全局错误块
extern Config_Block ramcof;  // 全局配置块
extern Data_Block ramdata;   // 全局数据块
extern Flash_Block ramflash;
// extern Flash_Fw flash_fw1;              //flash固件1信息
// extern Flash_Fw flash_fw2;              //flash固件2信息
// extern Flash_DeviceInfo flash_devinfo;  //设备信息

#endif
