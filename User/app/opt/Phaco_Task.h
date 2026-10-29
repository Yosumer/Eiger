/*------------------------------------------------------------------------------
 * @file    PHACO_TASK.H
 * @author  SIERRA TEAM 
 * @date    2024/07/16 13:52:31
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
#ifndef __PHACO_TASK_H
#define __PHACO_TASK_H

/* Files includes  -----------------------------------------------------------*/
#include "bsp_ultrasonic_protocol.h"
#include "bsp_abstractLineFunc.h"
/* Defines -------------------------------------------------------------------*/


/* Variables -----------------------------------------------------------------*/

enum PHACO_MODE{
    PHACO_IDEL_MODE = 0,
    PHACO_PLUSE_MODE,
    PHACP_BRUST_MODE,
    PHACO_CONTINUOUS_MODE,
    PHACO_TEST_MODE,
    PHACO_SCANFREQ_MODE    //加一个扫频模式
};

enum PHACO_ERROR_CODE {
    // PHACO_IMPEDANCE_ERROR = 0X6300,             //阻抗不匹配
    // PHACO_NEEDLE_ERROR  = 0X6301,               //换能器异常
    // PHACO_TRANSDUCER_ERROR = 0X6302,            //未检出到针头
    // PHACP_COM_ERROR =   0X6501                  //超乳手柄通信错误 

    PHACO_IMPEDANCE_ERROR = 0X3501,             //阻抗不匹配
    PHACO_NEEDLE_ERROR  = 0X3502,               //换能器异常
    PHACO_TRANSDUCER_ERROR = 0X3503,            //未检出到针头
    PHACP_COM_ERROR =   0X3504                  //超乳手柄通信错误 

};


// enum PHACO_ERROR_CODE {
//     PHACO_IMPEDANCE_ERROR = 0X6101,
//     PHACO_NEEDLE_ERROR  = 0X6102,
//     PHACO_TRANSDUCER_ERROR = 0X6103,
//     PHACP_COM_ERROR =   0X6701
// };


// typedef struct _Handpiece_data_t
// {
//     float       cde;    //总共使用的cde
//     float       time;   //总共使用的时间(s)
//     uint32_t    cnt;    //总共使用的次数
// }Handpiece_data_t;     //超乳任务数据


// typedef union _Handpiece_tag_t
// {
//     uint8_t buf[64];
//     struct 
//     {
//         uint8_t     ower[4];        //所属类型
//         uint8_t     needle[16];     //针头型号
//         union {
//         	uint8_t             type[24];  
//             Handpiece_data_t   power;    //功率统计相关
//         }data;  
//         uint8_t     sn[20];         
//     }info;
// }Handpiece_tag_t;



typedef struct _Handpiece_data_t
{
    uint32_t    cnt;    //总共使用的次数
    float       cde;    //总共使用的cde
    float       time;   //总共使用的时间(s)
}Handpiece_data_t;     //超乳任务数据


//固件相关信息
typedef union {
    uint32_t word[7];   
    struct
    {
		uint8_t    type[4];                 //手柄类型   目前SP01
        struct {
            uint8_t year[4];                //年
            uint8_t mon[2];                 //月
            uint8_t day[2];                 //日      
        }date;
       	uint16_t     fs_longitudinal;         //纵振中心频率
        uint16_t     fs_torsion;              //扭振中心频率
        float     C0;                      //阻抗分析仪纵振C0
        float     longitudinal_R1;         //阻抗分析仪纵振R1
        float     torsion_R1;              //阻抗分析仪扭振R1
        
    }data;
}hwHard; //固件相关信息

typedef union _Handpiece_tag_t{
    uint8_t buf[64];
    struct
    {
        uint8_t     ower[8];        //公司Sierra
        uint8_t     sn[16];         //公司SN码 PH00T01-251206
        hwHard      hw;             //手柄硬件版本及生产日期 以及工作频率
        union {
        	uint8_t             type[12];  
            Handpiece_data_t   power;    //功率统计相关
        }data; 
    }info;
}Handpiece_tag_t;









typedef struct _Phaco_Prams_Cof         //超乳参数配置
{

    uint32_t    _lpow;                   //纵振控制 （该位只有在踏板不可控的情况下才能配置）
    uint32_t    _vpow;                   //扭振控制  (该位只有在踏板不可控的情况下才能配置）
    uint32_t    _cutetims;              //关断时间(只读寄存器)（爆破模式下使用）

    uint32_t    _slpow;                  //起始纵振幅   (0~100)
    uint32_t    _elpow;                  //结束纵振幅   (0~100)
    uint32_t    _svpow;                  //起始扭振幅   (0~100)
    uint32_t    _evpow;                  //结束扭振幅   （0~100）
    uint32_t    _ltim;                  //纵振时间      （0~500）ms
    uint32_t    _vtim;                  //扭振时间      (0~500)ms
    uint32_t    _stim;                  //停机时间
}Phaco_Prams_Cof;

typedef struct _Phaco_Task_Cof
{
    uint32_t    _mode;                    //超乳模式，0x00不使能、0x01脉冲模式、0x02 爆破模式、0x03 连续模式
    uint32_t    _detectionConn;           //超乳手柄检测连接        默认检测    1:检测手柄 0:不检测手柄
    uint32_t    _footcof;                 //踏板控制模式  0:踏板不可控制  1:踏板一段控制  2：1号位置输出恒定抽吸模式 
                                                        //3:二段控制   4:2号位置输出恒定抽吸    5:三段控制

    uint32_t    _rgb_cof[1];            //2个手柄的RGB灯                
    Phaco_Prams_Cof _prams;             //超乳下发控制参数
}Phaco_Task_Cof;
// typedef union _bsp_ult_error      //底层超乳通信协议的错误
// {
//     uint32_t    _error;
//     struct 
//     {
//         uint16_t    revs:   12;
//         uint16_t    impedance   :1;      //阻抗不匹配
//         uint16_t    transducer  :1;     //换能器异常
//         uint16_t    needle      :1;         //未检出到针头
//         uint16_t    com:1;          //超乳手柄通信错误 （自定义的）
//         uint16_t    __rev;
//     }bits;
// }bsp_ult_error;

typedef union _bsp_ult_error      //底层超乳通信协议的错误
{
    uint32_t    _error;
    struct 
    {
//        uint32_t    revs:   12;
    uint32_t    ERR_CRC:1;
    uint32_t    ERR_MEM:1;
    uint32_t    ERR_PWR:1;
    uint32_t    ERR_VOLT:1;
    uint32_t    ERR_CURR:1;
    uint32_t    ERR_TIME:1;
    uint32_t    ERR_FREQ:1;
    uint32_t    ERR_TURE:1;
    uint32_t    ERR_PHASE:1;
    uint32_t    ERR_UNDECTED:1;
    uint32_t    ERR_EEPROM:1;
    uint32_t    ERR_SENSOR:1;
    uint32_t    impedance   :1;      //阻抗不匹配
    uint32_t    transducer  :1;     //换能器异常
    uint32_t    needle      :1;     //未检出到针头
    uint32_t    com:1;              //超乳手柄通信错误 （自定义的）
    uint32_t    P33V_ERROR:1;  ////3.3v电压异常
    uint32_t    P5V_ERROR:1;   ////5v电压异常
    uint32_t    P12V_ERROR:1; ////+12v电压异常
    uint32_t    N12V_ERROR:1; ////-12v电压异常
    uint32_t    CURRENT_TRANSFOR_ERROR:1;  ////电流互感器异常
    uint32_t    VOLT_TRANSFOR_ERROR:1;      ////电压互感器异常
    uint32_t    TIP_CUT_ERROR:1; ////断针
    uint32_t    TIP_LOOSE_ERROR:1;   ////针松动
    uint32_t    LONGITUDINA_FRQ_ERROR:1; ////纵振调谐异常
    uint32_t    TORSION_FRQ_ERROR:1; ////扭振调谐异常
    uint32_t    CURRENT_OVERLOAD_ERROR:1;////手柄电流过高
    uint32_t    VOLT_OVERLOAD_ERROR:1;  ////手柄电压过高
    uint32_t    Q_HIGH_ERROR:1;           ////手柄Q值过高
    uint32_t    Q_LOW_ERROR:1;              
    uint32_t    __rev:2;
    }bits;
}bsp_ult_error;

typedef struct _Phaco_Task_Data
{
    uint32_t         _outenable;    //输出使能  0:无能量输出 1：有能量输出
    uint32_t         _useport;      //当前使用的port    
    uint32_t         _conStatus;    //连接状态              0：无连接   1:超乳手柄1连接 2:超乳手柄2连接 
    uint32_t         _fristtim;     //第一次启动时间
    uint32_t         _footcntim;    //一次踏板工作的时间     ms
    uint32_t         _lpwr;         //纵振计算数值
    uint32_t         _vpwr;         //扭振计算数值
    uint32_t         _cutetims;     //关断时间（爆破模式下使用）
    bsp_ult_error    _error;        //当前手柄底层错误
    float            _pow;          //当前功率
    float            _sumpow;       //累计功率
    union           {
        uint32_t    _words[sizeof(Handpiece_tag_t)/4];
        Handpiece_tag_t _tag;
    }_tag_info;
}Phaco_Task_Data;

typedef struct _Phaco_Task_Handpiece_ops
{
    uint8_t (*_get_state)();    //获取连接状态
    void (*_connected)();       //连接操作函数
    void (*_disconnected)();    //断开连接操作函数
    void (*_set_color)(uint32_t _rgb);  //设置rgb灯
    void (*_read_e2prom)(uint8_t *buf,uint16_t _offset,uint16_t _len);
}Phaco_Task_Handpiece_ops;

typedef enum e_power_table{
	PHACO_POWER_0,
	PHACO_POWER_10,
	PHACO_POWER_20,
	PHACO_POWER_30,
	PHACO_POWER_40,
	PHACO_POWER_50,
	PHACO_POWER_60,
	PHACO_POWER_70,
	PHACO_POWER_80,
	PHACO_POWER_90,
	PHACO_POWER_100,
	PHACO_POWER_MAX,
}phaco_power_enum;

typedef struct{
	//uint8_t index;
	uint32_t tag_power;
	uint32_t set_power;
}power_table_t; 

typedef struct{
	power_table_t  val[PHACO_POWER_MAX];
}phaco_cal_param_t;

typedef struct Phaco_Task
{
    uint32_t    _prmode;                    //上一个状态
    uint32_t    _downlpwr;                  //下发纵振功率
    uint32_t    _downvpwr;                  //下发扭振功率
    uint32_t    _percentofPedal;            //踏板百分比
    CalibrationPara _lcali;                 //纵振校准参数
    CalibrationPara _vcali;                 //纵振校准参数
    ult_protocol_t  *ult_handler;           //超乳协议操作句柄
    Phaco_Prams_Cof  _localCof;             //本地配置
    Phaco_Task_Cof  *Wcof;                  //配置块指针
    Phaco_Task_Data *WData;                 //数据块指针        
    Phaco_Task_Handpiece_ops  handpice[2];    //手柄连接操作函数
	uint32_t    _lastpwr;                  //上一次功率
    uint32_t    _handpiece_usecnt;        //手柄使用次数
    int32_t     _handle_scan_sta;        //手柄扫频状态
	struct{
		uint32_t 			_flash_flag;		
		phaco_cal_param_t	*_lcali_tbl;
		phaco_cal_param_t	*_vcali_tbl;
	}cal_param_t;
	uint32_t    _percent_3d_config;         //3D脚踏配置
    float       _handpiece_loadRunTime;     //手柄加载时运行时间
    uint32_t    _handpiece_runtime;        //手柄运行时间
}Phaco_Task_t;

/* Functions ----------------------------------------------------------------*/
int     Phaco_Task_Init(void);
void    Phaco_Task(void *arg);
void    Phaco_Task_SetWcof(Phaco_Task_Cof *_cof);
void    Phaco_Task_SetData(Phaco_Task_Data *_data);
void    Phaco_Pedal_Deal(uint16_t _code,uint32_t _data);

void    Phaco_Task_ResetPwr();

void 	Phaco_Pedal_Disconnect();
void 	Phaco_Task_ShutDown();
uint8_t Phaco_Task_get_Connection();
void Phaco_Task_SetMode(enum PHACO_MODE _phacoMode,Phaco_Prams_Cof _prams);

uint8_t Phaco_Task_GetScanState();
uint8_t Phaco_Task_SetScanState(int32_t _state);
extern void 	phaco_set_default	(void);
extern uint32_t	phaco_solveX_v2		(float _input,phaco_cal_param_t *_cali);

extern const phaco_cal_param_t default_phaco_lcal_table;
extern const phaco_cal_param_t default_phaco_vcal_table;
extern Phaco_Task_t    _phaco;


#endif
