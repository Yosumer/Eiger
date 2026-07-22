#ifndef LIQUIDSUBCOF_H 
#define LIQUIDSUBCOF_H 


#define     CASSETTE_UNLOAD                 0           //盒子未加载
#define     CASSETTE_LOAD                   1           //盒子上盒
#define     CASSETTE_RELEASING              2           //盒子退盒中
#define     CASSETTE_LOADERROR              3           //盒子加载错误
// #define     CASSETTE_LOADING                4           //盒子上盒中


// 进盒的方案
#define    USE_SLIDE_RHEOSTAT_MODE	1		//滑动变阻器方案
#define    USE_LIGHT_MODE	        2				//光学方案


#define  DECETE_MODE    USE_LIGHT_MODE      //当前使用光学方案




// QM任务优先级         //数值越大优先级越高
#define QPC_CASSETTE_PRO  10          //积液盒上机任务优先级
#define QPC_INFUSION_PRO  16
#define QPC_SUCTION_PRO   15
#define QPC_REFLUX_PRO    14
#define QPC_PRIMING_PRO   13
#define QPC_SYSTEMCHECK_PRO  12

#define IV_RUN_MAX_SPEED                500000 


enum CASSETTE_ERROR {
    // CASSETTE_CLAMP_ERROR=0X4100,            //积液盒装载错误
    // CASSETTE_RELEASE_ERROR=0X4102,          //退盒失败

    CASSETTE_CLAMP_ERROR=0X5104,            //积液盒装载错误
    CASSETTE_RELEASE_ERROR=0X5105,          //退盒失败
};



enum INFUSION_ERROR {
    INFINITE_IV_ERROR=0X2100,               //灌注IV杆错误
};

enum PRIMING_ERROR {
    // PRIMING_SUCTION_VACCUM_HIGH=0X4301,                  // 抽吸管路负压过高（可能原因抽吸管路阻塞）
    // PRIMING_BOTTLE_PRESSURE_LOW=0X4302,                  // 瓶压过低
    // PRIMING_SUCTION_VACCUM_LOW=0X4303,                 // 抽吸负压低（可能原因抽吸管路漏气，接头漏气）
    PRIMING_SUCTION_VACCUM_HIGH=0X5102,                  // 抽吸管路负压过高（可能原因抽吸管路阻塞）
    PRIMING_BOTTLE_PRESSURE_LOW=0X5103,                  // 瓶压过低
    PRIMING_SUCTION_VACCUM_LOW=0X5101,                 // 抽吸负压低（可能原因抽吸管路漏气，接头漏气）
    // PRIMING_PHACO_TEST_FAIL1=0X4304,                   //超乳纵振测试失败
    // PRIMING_PHACO_TEST_FAIL2 = 0X4305,                 //超乳扭振测试失败

    PRIMING_PHACO_TEST_FAIL1=0X3101,                   //超乳纵振测试失败
    PRIMING_PHACO_TEST_FAIL2 = 0X3102,                 //超乳扭振测试失败
};

//外设错误
enum PERIPHERALS_ERROR {
    INFUSION_VALVE_ERROR=0X5106,               //灌注阀门错误
    REFLUX_VALVE_ERROR=0X5107,                 //回流阀门错误
    MOTOR_LOSESPEED_ERROR=0x5501                  //蠕动泵时失速
};

//气路相关错误
enum AIR_ERROR {
    AIR_LOW_PRESSURE=0X4501,               //隔膜泵管路工作气压低于保护阈值
    AIR_PUMP_CURRENT_ERROR = 0x4701,        //隔膜泵工作电流异常
};

enum DIATHERMY_ERROR {
    DIATHERMY_CURRENT_MAX_ERROR = 0X8501,         //电凝功率异常
    DIATHERMY_CURRENT_MIN_ERROR = 0X8502,          //电凝功率丢失
    DIATHERMY_VOL_MAX_ERROR = 0X8503,   //电凝温度异常
    DIATHERMY_VOL_MIN_ERROR = 0X8504,    //电凝温度丢失 
};

typedef struct _Infusion_Task_Cof
{
    // uint32_t    _pressure;      //灌注压力
    uint32_t    _pedalcof;          //灌注脚踏配置     
    uint32_t    _polltime;          //灌注轮训时间
    uint32_t    _enable;        // 0:关闭灌注   1:打开灌注  2:打开气液交换   
}Infusion_Task_Cof;



typedef struct _Infusion_Task_Data
{

    uint32_t    _errCode;       //错误代码
    uint32_t    _prstate;       //上一个状态
    uint32_t    _state;         //状态      
    uint32_t    _ivenable;      //IV杆是否使能
    uint32_t    _ivmotorstep;   //IV杆脉冲步数
    float       _ivcmhight;     //IV杆上升高度

}Infusion_Task_Data;

typedef struct {
    uint32_t _enCaseMode;   //CASE模式使能
    uint32_t _upVaccum;             //负压上限
    uint32_t _downVaccum;           //负压下限
    uint32_t _targetVaccum;         //目标负压
    uint32_t _vaccumDurationTime;   // 负压持续时间（20ms~2000ms）
}CaseCof;

typedef struct _Suction_Vaccum_Cof
{
    uint32_t _minvalue;     //最小负压限制值
    uint32_t _maxvalue;     //最大负压限制值
    uint32_t _blockvalue;   //阻塞吧负压值（阻塞模式下才有用）
    CaseCof    _case; //
}Suction_Vaccum_Cof;


typedef struct _Suction_Flow_Cof {
    uint32_t    _minflow;       //最小配置流量
    uint32_t    _maxflow;       //最大配置流量
    uint32_t    _blockflow;     //阻塞时流量配置
    struct      {
        uint32_t _unoccluded;  // 未阻塞时泵压设置
        uint32_t _occluded;    // 阻塞时泵压设置
    }pumpRamp;  //泵压设置点
}Suction_Flow_Cof;  //抽吸流量相关配置值


typedef struct _Suction_Task_Cof
{
    uint32_t                _en;            //功能是否使能
    uint32_t                _cleanCassette;  //清洗积液盒
    uint32_t                _pedalcof;      //液路模块响应踏板

    uint32_t                _enBlockmode;   //阻塞模式是否开启

    Suction_Vaccum_Cof      _vaccumCof; //负压相关配置参数
    Suction_Flow_Cof        _flowCof;   //流量相关配置

    // uint32_t                _vaccumLimit;   //负压限制值
    // uint32_t                _start;         //起始值
    // uint32_t                _end;           //终止值        //0x26
}Suction_Task_Cof;


typedef struct _Suction_Task_Data
{

    uint32_t    _errCode;               //错误代码
    uint32_t    _prstate;               //上一个状态
    uint32_t    _state;                 //状态     
    uint32_t    _motorctlstep;          //蠕动泵控制脉冲转速
    float       _ctlflowcc;             //（实际流量）
    uint32_t    _vaccumRawdata;         //负压ADC值
    float       _vaccumpressure;       // 实际负压
    uint32_t    _percentofPedal;       //踏板百分比    //0X95
    uint32_t    _ctlValue;             //液流控制值    //0X96
    uint32_t    _inputCtlValue;        //液流输入控制值
    uint32_t    _vaccumLimit;         //负压限制值
}Suction_Task_Data;




typedef struct _Reflux_Sub_Cof {
    uint32_t _en;               //回吐功能使能（结束后自动清零）
    uint32_t _refluxMode;       //回吐模式  0:瓶回吐    1:蠕动泵回吐
    uint32_t _maxPressure;         //回吐功能停止正压
    uint32_t _targetPressure;      //回吐功能结束后的判断压力  说明 若回吐结束后压力小于此值打开回吐阀门等待压力大于此值同时打开灌注阀门等待正压达到此值（超时判断）
    uint32_t _timeout;             //回吐释放后的超时时间（ms）
}Reflux_Sub_Cof;

typedef struct _Reflux_SubDrain_Cof
{
    uint32_t _en;   // 排液功能测试使用
    uint32_t _drainMode;        //排液模式  0:瓶排液    1：蠕动泵排液
    uint32_t _releasePressure;  //释放目标压力(排液释放压力)
    uint32_t _timeout;          //压力检测超时时间
}Reflux_SubDrain_Cof;   //排液子模式


typedef struct _Reflux_Task_Cof {
    Reflux_Sub_Cof          _refluxparms;   //回吐参数配置  
    Reflux_SubDrain_Cof     _drainparms;    //排液参数
}Reflux_Task_Cof;


typedef struct _Reflux_Task_Data {
    uint32_t _errCode;                  //错误代码
    uint32_t _prstate;                  //上一个状态
    uint32_t _state;                    //状态
    uint32_t _releaseTime;              //压力释放时间
    float   _releaseCurrentPressure;   //释放结束时压力
}Refluex_Task_Data;



typedef struct _Priming_Task_Cof
{
    uint32_t _enBypass;             // 使能分流
    uint32_t _prmingcmd;            // 0:停止预充       1：开始全部预充     2：预充第一段   3：预充第二段   4：预充第三段
    uint32_t _phacoTest;            // 0:停止超乳测试   1：开始超乳测试(纵振) 2:开始超乳测试(扭振) 3：开始超乳测试(纵振+扭振)
    uint32_t _vitTest;              // 0:停止波切测试   1：开始波切测试   
}Priming_Task_Cof;


typedef struct _Priming_Task_Data
{
    uint32_t    _errCode;           //错误代码
    uint32_t    _prstate;           //上一个预充状态
    uint32_t    _state;             //预充状态      
    int32_t    _primingRuntime;    // 预充运行时间
}Priming_Task_Data;

typedef struct _SystemCheck_Task_Data
{
    uint32_t    _errCode;           //错误代码
    uint32_t    _prstate;           //上一个预充状态
    uint32_t    _state;             //预充状态   
    struct 
    {
        uint32_t    _leftmotor:1;          //流体电机2自检状态 0:正常 1:异常
        uint32_t    _rightmotor:1;        //流体电机1自检状态 0:正常 1:异常
        uint32_t    _vaccumModule:1;      //真空度自检状态    0:正常 1:异常
        uint32_t    _infusionValve:1;     //流体灌注阀自检状态  0:正常 1:异常
        uint32_t    _refluexValve:1;      //流体抽吸阀自检状态   0:正常 1:异常
        uint32_t    _pump:1;             //蠕动泵自检状态       0:正常 1:异常
        uint32_t    _diathermy:1;        //电凝自检状态          0:正常 1:异常
        uint32_t   :25;
    }checkStatus;
}SystemCheck_Task_Data;

typedef struct _SystemCheck_Task_Cof
{
    uint32_t    _enableCheck;   //使能自检
}SystemCheck_Task_Cof;

#endif // LIQUIDSUBCOF_H





