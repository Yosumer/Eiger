/*
*********************************************************************************************************
*
*	模块名称 : 用户自定义工程
*	文件名称 : task_user_instruction.c
*	版    本 : V1.0
*	说    明 : .
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2024-11-14  Zhang  正式发布
*
*
*	Copyright (C), 2022-2032, 思埃然电子 Sierra Medical
*
*********************************************************************************************************
*/
/* 包含头文件 ----------------------------------------------------------------*/
#include "includes.h"

#include "app_instruction_com.h"
//#include "bsp_timer8_pwm.h"

/* 私有类型定义 --------------------------------------------------------------*/
typedef struct {
    uint16_t cmd;
    void (*InstructCB)(uint16_t _code,uint16_t _data);
} INSTRUCT_OPS_T;


typedef enum {
    PROJECT_CMD_NULL = 0,
    PROJECT_DIATHERMY_SET_VALUE,
    PROJECT_DIATHERMY_CALIB_READ,
    PROJECT_DIATHERMY_CALIB_CACULATE,
    PROJECT_PHACO_SET_POWER_L,
    PROJECT_PHACO_SET_POWER_T,
    PROJECT_PHACO_SET_CAL_INDEX,
    PROJECT_PHACO_SET_RAW,
    PROJECT_PHACO_SAVE_CALIB,
    PROJECT_OPT_SAVE_PARAM,
    PROJECT_DIATHERMY_SET_DAC,
    PROJECT_DIATHERMY_SET_MODE,
    PROJECT_DIATHERMY_SET_CAL_TABLE,
    PROJECT_DIATHERMY_ADC_UPDATE,
    PROJECT_DIATHERMY_SET_FREQ,
    PROJECT_DIATHERMY_FREQ_UPDATE,
    PROJECT_DIATHERMY_MODULE_TEST,
    PROJECT_DIATHERMY_MODULE_RESET,
    PROJECT_CMD_NUM_MAX,
} ENGINEER_COMMAND_INDEX_E;

typedef struct
{
    bsp_dia_t	*dia_handle;
    Phaco_Task_t *phaco_handle;
} Instruct_handle_t;



typedef enum _dia_cal_mode {
    DIA_CAL_MODE_DEFAULT,
    DIA_CAL_MODE_PLUS,			/* calibrate volt and curr */
    DIA_CAL_MODE_POWER_ONLY,
    DIA_CAL_MODE_MAX,
} dia_cal_mode_e;


typedef struct {
    float   dac[CALIB_NUMS];
    float   power[CALIB_NUMS];
} CALIB_OUTPUT_T;

typedef struct
{
    void  (*fal_phaco_write)(uint8_t *_buf,uint16_t _len);
    uint8_t  (*fal_phaco_read)(void);

    void  (*fal_chop_write)(uint8_t *_buf,uint16_t _len);
    uint8_t  (*fal_chop_read)(void);

    void  (*fal_dev_write)(uint8_t *_buf,uint16_t _len);
    uint8_t  (*fal_dev_read)(void);

} cal_ops;


typedef struct {

    bsp_dia_t				*dia_handle;
    Phaco_Task_t			*phaco_handle;
    project_user_data_t  	*project_user;
    uint32_t *ptr_data_tmp;

    struct {
        uint32_t 	calibrate_mode;
        uint8_t 	index;
        float		curr_adc[CALIB_NUMS];
        float		volt_adc[CALIB_NUMS];
        CALIB_OUTPUT_T  output;
        calib_dia_dac_set_t *set;
    } dia_info;

    struct {
        uint8_t curr_lcal_index;
        uint8_t curr_vcal_index;
    } phaco_info;


} OPT_CALIB_T;

typedef struct {
    uint8_t type;		//1-纵振 2-扭振
    uint8_t index;		//范围 0-10
} phaco_pow_index_t;



/* 私有宏定义 ----------------------------------------------------------------*/


extern Phaco_Task_t    _phaco ;

/* 私有变量 ------------------------------------------------------------------*/
Instruct_handle_t	m_tProject = {
    .dia_handle = &_dia_instance,
    .phaco_handle = &_phaco,
};

OPT_CALIB_T		m_tCalib  = {
    .dia_handle 	= &_dia_instance,
    .phaco_handle	= &_phaco,
    .project_user	= &ramdata.project_data,
    .dia_info = {0},
    .phaco_info = {0},
    .ptr_data_tmp = (uint32_t *)RAM_CALIB_DATA_ADDR,
};


const calib_dia_dac_set_t m_tCalibDacSetDefault = {
    .dac_out = {
        400,
        600,
        800,
        1000,
        1200
    }
};

/* 私有函数原形 --------------------------------------------------------------*/
static void _project_diathermy_set_power			(uint16_t _code, uint16_t _data);
static void _project_diathermy_calib_read_sensor	(uint16_t _code, uint16_t _data);
static void _project_diathermy_calib_caculate		(uint16_t _code, uint16_t _data);
static void _project_phaco_set_lPower				(uint16_t _code, uint16_t _data);
static void _project_phaco_set_tPower				(uint16_t _code, uint16_t _data);
static void _project_phaco_set_calIndex				(uint16_t _code, uint16_t _data);
static void _project_phaco_set_raw					(uint16_t _code, uint16_t _data);
static void _project_phaco_save_calib				(uint16_t _code, uint16_t _data);
static void _project_opt_save_param					(uint16_t _code, uint16_t _data);
static void _project_diathermy_setDac				(uint16_t _code, uint16_t _data);
static void _project_diathermy_setCalDacTable			(uint16_t _code, uint16_t _data);
static void _project_diathermy_calUsePowerOnly		(uint16_t _code, uint16_t _data);
static void _project_diathermy_adcUpdate			(uint16_t _code, uint16_t _data);
static void _project_diathermy_setFreq			(uint16_t _code, uint16_t _data);
static void _project_diathermy_freqUpdate			(uint16_t _code, uint16_t _data);
static void _project_diathermy_moduleTest			(uint16_t _code, uint16_t _data);
static void _project_diathermy_moduleReset			(uint16_t _code, uint16_t _data);


static void calib_diathermy_setDac		(uint16_t _usData);
static void calib_diathermy_setMode		(uint16_t _usData);
static void calib_diathermy_caculate	(uint16_t _usCMD);
static void calib_phaco_set_raw			(uint16_t _usCMD);
static void calib_updataPhacoStatus		(void);
static void calib_phaco_set_powIndex	(uint16_t _usCMD);
static void calib_phaco_save_param		(uint16_t _usCMD);
static void calib_opt_save_param(uint16_t _usCMD);


static void _project_diathermy_getParam				(struct Flash_Sensor *_sensor);
static int 	_project_diathermy_setParam				(struct Flash_Sensor const *_pSensor);

/* 函数体 --------------------------------------------------------------------*/


INSTRUCT_OPS_T Instruct_ops_table[] = {
    { PROJECT_DIATHERMY_SET_VALUE,		_project_diathermy_set_power},
    { PROJECT_DIATHERMY_CALIB_READ,		_project_diathermy_calib_read_sensor},
    { PROJECT_DIATHERMY_CALIB_CACULATE,	_project_diathermy_calib_caculate},
    { PROJECT_PHACO_SET_POWER_L,		_project_phaco_set_lPower},
    { PROJECT_PHACO_SET_POWER_T,		_project_phaco_set_tPower},
    { PROJECT_PHACO_SET_CAL_INDEX,		_project_phaco_set_calIndex},
    { PROJECT_PHACO_SET_RAW,			_project_phaco_set_raw},
    { PROJECT_PHACO_SAVE_CALIB,			_project_phaco_save_calib},
    { PROJECT_OPT_SAVE_PARAM,			_project_opt_save_param},
    { PROJECT_DIATHERMY_SET_DAC,		_project_diathermy_setDac},
    { PROJECT_DIATHERMY_SET_CAL_TABLE,	_project_diathermy_setCalDacTable},
    { PROJECT_DIATHERMY_ADC_UPDATE,		_project_diathermy_adcUpdate},
    { PROJECT_DIATHERMY_SET_FREQ,		_project_diathermy_setFreq},
    { PROJECT_DIATHERMY_FREQ_UPDATE,	_project_diathermy_freqUpdate},
    { PROJECT_DIATHERMY_MODULE_TEST,	_project_diathermy_moduleTest},
    { PROJECT_DIATHERMY_MODULE_RESET,	_project_diathermy_moduleReset},
};



/*
*********************************************************************************************************
*	函 数 名: Instruct_InitParam
*	功能说明: 参数初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int Instruct_InitOPT(void)
{
    for(uint8_t n = 0;  n < (sizeof(Instruct_ops_table)/sizeof(Instruct_ops_table[0])); n++ )
    {
        if(Instruct_ops_table[n].InstructCB != NULL)
            Instruction_Task_RegisterFun(Instruct_ops_table[n].cmd, Instruct_ops_table[n].InstructCB);
    }

    return 0;
}
INIT_APP_EXPORT(Instruct_InitOPT);


//extern void Selftest(void);


void  _project_diathermy_setFreq(uint16_t _code,uint16_t _data)
{

    (void)_code; //设置频率

    {
        set_dia_freq(_data);
        updata_time1_freq( _data);
    }

}



/*
*********************************************************************************************************
*	函 数 名: _project_diathermy_freqUpdate
*	功能说明: 频率更新
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void  _project_diathermy_freqUpdate(uint16_t _code,uint16_t _data)
{

    (void)_code; //保存频率

    {
        /* 保存数据 */
        struct Flash_Sensor   sensor_tmp = {0};

        _project_diathermy_getParam(&sensor_tmp);

        sensor_tmp._unuse = 0x55AA;
		
		if(_project_diathermy_setParam(&sensor_tmp) >= 0)
		{
			m_tCalib.project_user->cal_status.bit.dia_freq_bit = 1;
		}			
		
    }
}


void _project_diathermy_moduleTest(uint16_t _code,uint16_t _data)
{
    //Selftest();
}

void _project_diathermy_moduleReset(uint16_t _code, uint16_t _data)
{
    HAL_NVIC_SystemReset();
}


/*
*********************************************************************************************************
*	函 数 名: _project_diathermy_set_power
*	功能说明:
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void _project_diathermy_set_power(uint16_t _code, uint16_t _data)
{
    (void)_code;

    if(0 == _data)
    {
        m_tProject.dia_handle->_ops->disable(m_tProject.dia_handle);
    }
    else
    {
        float power_tmp = 0.0f;
        power_tmp = (float)_data / 10;

        if(power_tmp > 10.0f)
            power_tmp = 10.0f;

        /* for old emc comand */
//        if(1 == _data)
//        {
//            //output max
//            power_tmp = 10.0f;
//        }

        bsp_dia_setPower(m_tProject.dia_handle, power_tmp);

    }

}


/*
*********************************************************************************************************
*	函 数 名: _project_diathermy_calib_read_sensor
*	功能说明:
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void _project_diathermy_calib_read_sensor(uint16_t _code, uint16_t _data)
{
    (void)_code;

    calib_diathermy_readSensor(_data);
}


/*
*********************************************************************************************************
*	函 数 名: _project_diathermy_calib_read_sensor
*	功能说明:
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void _project_diathermy_calib_caculate(uint16_t _code, uint16_t _data)
{
    (void)_code;

    calib_diathermy_caculate(_data);
}


/*
*********************************************************************************************************
*	函 数 名: _project_phaco_set_lPower
*	功能说明:
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void _project_phaco_set_lPower(uint16_t _code, uint16_t _data)
{
    (void)_code;

    if(0 == _data)
    {
        bsp_ultprotocol_ctl_power1(&ult_handler, 0, 0, 0, 0, 0);
    }
    else
    {

        bsp_ultprotocol_ctl_power1(&ult_handler,
                                   phaco_solveX_v2(_data, _phaco.cal_param_t._lcali_tbl),
                                   0,
                                   5,
                                   0,
                                   0);
    }
}

/*
*********************************************************************************************************
*	函 数 名: _project_phaco_set_tPower
*	功能说明:
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void _project_phaco_set_tPower(uint16_t _code, uint16_t _data)
{
    (void)_code;

    if(0 == _data)
    {
        bsp_ultprotocol_ctl_power1(&ult_handler, 0, 0, 0, 0, 0);
    }
    else
    {
        bsp_ultprotocol_ctl_power1(&ult_handler,
                                   0,
                                   phaco_solveX_v2(_data, _phaco.cal_param_t._vcali_tbl),
                                   0,
                                   20,
                                   0);
    }

}


/*
*********************************************************************************************************
*	函 数 名: _project_phaco_set_raw
*	功能说明:
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void _project_phaco_set_calIndex				(uint16_t _code, uint16_t _data)
{
    (void)_code;

    calib_phaco_set_powIndex(_data);
}

/*
*********************************************************************************************************
*	函 数 名: _project_phaco_set_raw
*	功能说明:
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void _project_phaco_set_raw(uint16_t _code, uint16_t _data)
{
    (void)_code;

    calib_phaco_set_raw(_data);
}


/*
*********************************************************************************************************
*	函 数 名: _project_phaco_save_calib
*	功能说明:
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void _project_phaco_save_calib(uint16_t _code, uint16_t _data)
{
    (void)_code;

    calib_phaco_save_param(_data);
}

/*
*********************************************************************************************************
*	函 数 名: _project_opt_save_param
*	功能说明:
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void _project_opt_save_param(uint16_t _code, uint16_t _data)
{
    (void)_code;

    calib_opt_save_param(_data);
}


/*
*********************************************************************************************************
*	函 数 名: _project_diathermy_setDac
*	功能说明:
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void _project_diathermy_setDac(uint16_t _code, uint16_t _data)
{
    (void)_code;

    if(0 == _data)
    {
        m_tProject.dia_handle->_ops->disable(m_tProject.dia_handle);
    }
    else
    {
        calib_diathermy_setDac(_data);
    }

}


/*
*********************************************************************************************************
*	函 数 名: _project_diathermy_setCalDacTable
*	功能说明:
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void _project_diathermy_setCalDacTable(uint16_t _code, uint16_t _data)
{
    (void)_code;
    CALIB_diathermy_setCalDacTable(_data);

}


/*
*********************************************************************************************************
*	函 数 名: _project_diathermy_calUsePowerOnly
*	功能说明:
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void _project_diathermy_calUsePowerOnly		(uint16_t _code, uint16_t _data)
{
    (void)_code;

    calib_diathermy_setMode(_data);
}


/*
*********************************************************************************************************
*	函 数 名: _project_diathermy_setDac
*	功能说明:
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void _project_diathermy_setMode(uint16_t _code, uint16_t _data)
{
    (void)_code;

    if(0 == _data)
    {
        m_tProject.dia_handle->_ops->disable(m_tProject.dia_handle);
    }
    else
    {
        calib_diathermy_setDac(_data);
    }

}

/*
*********************************************************************************************************
*	函 数 名: _project_diathermy_adcUpdate
*	功能说明:
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void _project_diathermy_adcUpdate(uint16_t _code, uint16_t _data)
{
    (void)_code;

    if(0 == _data)
    {
        m_tProject.dia_handle->_adcUpdate = 0;
    }
    else
    {
        m_tProject.dia_handle->_adcUpdate = 1;
    }

}

/*
*********************************************************************************************************
*	函 数 名: _project_diathermy_getParam
*	功能说明:
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void _project_diathermy_getParam(struct Flash_Sensor *_sensor)
{
    _sensor->_outMax = m_tCalib.dia_handle->_cal->_outMax;
    _sensor->_diaFreq = m_tCalib.dia_handle->_cal->_diaFreq;

    memcpy(&_sensor->_vCal,  	&m_tCalib.dia_handle->_cal->_vcal, sizeof(CalibrationPara));
    memcpy(&_sensor->_iCal,  	&m_tCalib.dia_handle->_cal->_ical, sizeof(CalibrationPara));
    memcpy(&_sensor->_outCal,	&m_tCalib.dia_handle->_cal->_outcal, sizeof(CalibrationPara));
}

/*
*********************************************************************************************************
*	功能说明: 电凝单元的参数保存
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static int _project_diathermy_setParam(struct Flash_Sensor const *_pSensor)
{
	/* 向flash写入电凝校准数据 */
	Flash_Task_Write("sensor", (uint8_t *)_pSensor, sizeof(struct Flash_Sensor) );
	
    /* 从flash 读取电凝校准信息 并校验 */ 
	uint8_t *pdt;
    pdt = Flash_Task_Read("sensor");
    struct Flash_Sensor  *pSensor = (struct Flash_Sensor  *)pdt;

    if(0x55AA == pSensor->_unuse )
    {
        if(0 == memcmp(_pSensor, pdt, sizeof(struct Flash_Sensor)) )
			return 0;
    }
	
	return -1;
}

/*
*********************************************************************************************************
*	功能说明: 超乳单元的参数保存
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static int _project_phaco_setParam(struct Flash_User const *_pUser)
{
	/* 1.flash写入超乳校准信息 */
	Flash_Task_Write("user", (uint8_t *)_pUser, sizeof(struct Flash_User) );

    /* 2.从flash 读取超乳校准信息 并校验 */ 
	uint8_t *pdt;
    pdt = Flash_Task_Read("user");
    struct Flash_User  *pUser = (struct Flash_User  *)pdt;

    if(0x55AA == pUser->_unuse)
    {
        if(0 == memcmp(_pUser, pdt, sizeof(struct Flash_User)))
			return 0;
    }

	return -1;
}



/*
*********************************************************************************************************
*	函 数 名: CALIB_initDiathermy
*	功能说明:
*	形    参: _usData
*	返 回 值: 无
*********************************************************************************************************
*/
int CALIB_initDiathermy(void)
{
    m_tCalib.dia_info.set = &ramdata.project_data.cal_dac;
    memcpy(m_tCalib.dia_info.set, &m_tCalibDacSetDefault, sizeof(calib_dia_dac_set_t));
    return 0;
}
INIT_APP_EXPORT(CALIB_initDiathermy);


/*
*********************************************************************************************************
*	函 数 名: CALIB_diathermy_setCalDacTable
*	功能说明: 设定标定用的dac表 
*	形    参: _usData
*	返 回 值: 无
*********************************************************************************************************
*/
int CALIB_diathermy_setCalDacTable(uint16_t _usIndex)
{
    /* */
    if(_usIndex >= CALIB_NUMS)
        return -1;

    float _fData = 0;

    memcpy(&_fData,   (uint8_t *)(RAM_CALIB_DATA_ADDR + 4*_usIndex), sizeof(float));

    m_tCalib.dia_info.set->dac_out[_usIndex] = _fData;
    return 0;
}


/*
*********************************************************************************************************
*	函 数 名: calib_diathermy_setDac
*	功能说明:
*	形    参: _usData
*	返 回 值: 无
*********************************************************************************************************
*/
void calib_diathermy_setDac(uint16_t _usData)
{
    m_tCalib.dia_handle->_ops->enable(m_tCalib.dia_handle);

    m_tCalib.dia_handle->_ops->set_dac( _usData);

    m_tCalib.dia_handle->_info._dacval = _usData;
}


/*
*********************************************************************************************************
*	函 数 名: calib_diathermy_setDac
*	功能说明:
*	形    参: _usData
*	返 回 值: 无
*********************************************************************************************************
*/
void calib_diathermy_setMode(uint16_t _usData)
{
    m_tCalib.dia_info.calibrate_mode = _usData;
}

/*
*********************************************************************************************************
*	函 数 名: calib_diathermy_readSensor
*	功能说明:
*	形    参: _usCMD
*	返 回 值: 无
*********************************************************************************************************
*/
void calib_diathermy_readSensor(uint16_t _usCMD)
{
    if((_usCMD >= CALIB_NUMS) )
    {
        return ;
    }

    if(NULL == m_tCalib.dia_handle)
    {
        //m_tCalib.dia_handle = m_tProject.dia_handle;
        return ;
    }

    m_tCalib.dia_handle->_ops->enable(m_tCalib.dia_handle);

    m_tCalib.dia_info.index = _usCMD;

    m_tCalib.dia_handle->_ops->set_dac( m_tCalib.dia_info.set->dac_out[_usCMD] );
    m_tCalib.dia_info.output.dac[_usCMD] = m_tCalib.dia_info.set->dac_out[_usCMD];

    /* 读取ADC值 */
    rt_thread_delay(2);
    m_tCalib.dia_info.curr_adc[_usCMD] = m_tCalib.dia_handle->_ops->get_curr();
    m_tCalib.dia_info.volt_adc[_usCMD] = m_tCalib.dia_handle->_ops->get_vol();

    m_tCalib.project_user->curr_adc = m_tCalib.dia_info.curr_adc[_usCMD];
    m_tCalib.project_user->volt_adc = m_tCalib.dia_info.volt_adc[_usCMD];

}


typedef struct {
    float curr_out[CALIB_NUMS];
    float volt_out[CALIB_NUMS];
} dia_tmp_data;

#define DIA_CAL_PLUS  0U

/*
*********************************************************************************************************
*	函 数 名: calib_diathermy_caculate
*	功能说明: 电凝参数线性拟合
*	形    参: _usCMD
*	返 回 值: 无
*********************************************************************************************************
*/
void calib_diathermy_caculate(uint16_t _usCMD)
{
    if(1 == _usCMD)
    {
        CalibrationPara voltCal = {0};
        CalibrationPara iTemp = {0};
        CalibrationPara powCal = {0};
        float tmp_adc[CALIB_NUMS] = {0};
        float tmp_data[CALIB_NUMS] = {0};

        //dia_tmp_data * dia_out = (dia_tmp_data *)m_tCalib.ptr_data_tmp;;
        dia_tmp_data *dia_out = NULL;
        dia_out = (dia_tmp_data *)m_tCalib.ptr_data_tmp;

        if(NULL == dia_out)
        {
            return;
        }

		/* 用于实现电压曲线和电流曲线的拟合 */
        if(DIA_CAL_MODE_PLUS == m_tCalib.dia_info.calibrate_mode)
        {
            /*  I_ADC - 电流 线性拟合  */
            memcpy(tmp_adc,  	m_tCalib.dia_info.curr_adc, sizeof(tmp_adc));
            memcpy(tmp_data,    dia_out->curr_out, sizeof(tmp_data));
            if(Calib_linearFitting(CALIB_NUMS,  tmp_adc, dia_out->curr_out, &iTemp) > 0.99f)
            {
                bsp_dia_set_Curr_Calib(m_tCalib.dia_handle, iTemp);
                m_tCalib.project_user->cal_status.bit.dia_curr_fin_bit = 1;
            }

            /*  v_ADC - 电压 线性拟合  */
            memcpy(tmp_adc,  	m_tCalib.dia_info.volt_adc, sizeof(tmp_adc));
            //memcpy(tmp_data,   (uint8_t *)RAM_CALIB_DATA_ADDR + CALIB_NUMS * sizeof(float), sizeof(tmp_data));
            memcpy(tmp_data,   dia_out->volt_out, sizeof(tmp_data));

            if(Calib_linearFitting(CALIB_NUMS,  tmp_adc,  tmp_data, &voltCal) > 0.99f)
            {
                bsp_dia_set_Vol_Calib(m_tCalib.dia_handle, voltCal);
                m_tCalib.project_user->cal_status.bit.dia_volt_fin_bit = 1;
            }

        }

        uint32_t  temp_curr_input = 0;
        uint32_t  temp_volt_input = 0;
        float out_temp_array[3] = {0};
        float calib_error = 0.0f;

		/* 计算当前功率 */
        for (uint8_t i = 0; i < CALIB_NUMS ; i++)
        {
#if DIA_CAL_PLUS > 0u
            temp_curr_input = (uint32_t)m_tCalib.curr_adc[i];
            temp_volt_input = (uint32_t)m_tCalib.volt_adc[i];
            tmp_data[i] = (bsp_abstractLineFunc_GetOut(temp_curr_input,&iTemp)) * bsp_abstractLineFunc_GetOut(temp_volt_input,&voltCal)/1000;
#else
            if(DIA_CAL_MODE_POWER_ONLY == m_tCalib.dia_info.calibrate_mode)
            {
                /* 仅校准功率模式下 上位机下发的功率数据缓存在 电流缓存值内 */
                tmp_data[i] = dia_out->curr_out[i];
            }
            else
            {
                tmp_data[i] = dia_out->volt_out[i] * dia_out->curr_out[i] /1000;
            }
#endif
        }

		/* 参数拟合计算 */
        memcpy(tmp_adc,  	m_tCalib.dia_info.output.dac, sizeof(tmp_adc));

        polyfit(tmp_data, tmp_adc, CALIB_NUMS, 2, out_temp_array);

        powCal.A = out_temp_array[2];
        powCal.B = out_temp_array[1];
        powCal.C = out_temp_array[0];
		
		
		/*  拟合度低于0.99直接跳出 */
        if(calculate_Rsquard(tmp_data, tmp_adc, CALIB_NUMS, powCal.A, powCal.B, powCal.C) > 0.99f)
            bsp_dia_set_Out_Calib(m_tCalib.dia_handle,powCal);
        else
            return;
		
        /* 保存数据 */
        struct Flash_Sensor   sensor_tmp = {0};

        _project_diathermy_getParam(&sensor_tmp);
        sensor_tmp._unuse = 0x55AA;
		
		if(_project_diathermy_setParam(&sensor_tmp) >= 0)
		{
			/* 拟合成功标志位 置位*/
			m_tCalib.project_user->cal_status.bit.dia_fin_bit = 1;		
		}	
    }

}


/*
*********************************************************************************************************
*	函 数 名: calib_phaco_set_raw
*	功能说明: 超乳发送原始参数
*	形    参: _usCMD
*	返 回 值: 无
*********************************************************************************************************
*/
void calib_phaco_set_raw(uint16_t _usCMD)
{
    if(0 == _usCMD)
    {
        bsp_ultprotocol_ctl_power1(&ult_handler,0,0,0,0,0);
    }
    else
    {
        calib_phaco_raw_data_t this_data;

        memcpy(&this_data,   (uint8_t *)RAM_CALIB_DATA_ADDR, sizeof(this_data));

        bsp_ultprotocol_ctl_power1(&ult_handler,
                                   this_data.lPower,
                                   this_data.vPower,
                                   this_data.lTime,
                                   this_data.vTime,
                                   this_data.sTime);
    }
}


/*
*********************************************************************************************************
*	函 数 名: calib_updataPhacoStatus
*	功能说明:
*	形    参:
*	返 回 值: 无
*********************************************************************************************************
*/
void calib_updataPhacoStatus(void)
{
    if(m_tCalib.project_user->cal_status.bit.phaco_lfin_index_bit & 1 << m_tCalib.phaco_info.curr_lcal_index )
        m_tCalib.project_user->cal_status.bit.phaco_curr_lfin_bit = 1;
    else
        m_tCalib.project_user->cal_status.bit.phaco_curr_lfin_bit = 0;

    if(m_tCalib.project_user->cal_status.bit.phaco_vfin_index_bit & 1 << m_tCalib.phaco_info.curr_vcal_index )
        m_tCalib.project_user->cal_status.bit.phaco_curr_vfin_bit = 1;
    else
        m_tCalib.project_user->cal_status.bit.phaco_curr_vfin_bit = 0;
}


/*
*********************************************************************************************************
*	函 数 名: calib_phaco_set_powIndex
*	功能说明:
*	形    参: _usCMD
*	返 回 值: 无
*********************************************************************************************************
*/
void calib_phaco_set_powIndex(uint16_t _usCMD)
{
    uint8_t phaco_type = _usCMD>>8;
    uint8_t phaco_index = _usCMD;

    if(phaco_index >= PHACO_POWER_MAX)
        return;

    if(1 == phaco_type) {
        m_tCalib.phaco_info.curr_lcal_index = phaco_index;
    } else if(2 == phaco_type) {
        m_tCalib.phaco_info.curr_vcal_index = phaco_index;
    }

    calib_updataPhacoStatus();
}

/*
*********************************************************************************************************
*	函 数 名: calib_phaco_save_param
*	功能说明: 超乳
*	形    参: _usCMD
*	返 回 值: 无
*********************************************************************************************************
*/
void calib_phaco_save_param(uint16_t _usCMD)
{
    struct Flash_User	phaco_tmp = {0};

    uint32_t *p_cal = (uint32_t *)m_tCalib.ptr_data_tmp;
    uint16_t level_index		= m_tCalib.phaco_info.curr_lcal_index;
    uint16_t torinsinal_index	= m_tCalib.phaco_info.curr_vcal_index;

    if(NULL == p_cal)
    {
        return;
    }

    if((RAW_PHACO_DISABLE == _usCMD)&&(_usCMD >= RAW_PHACO_CMD_NUM_MAX))
        return ;


    if(RAW_PHACO_MODE_L_DATA == _usCMD)
    {
        m_tCalib.project_user->cal_status.bit.phaco_lfin_index_bit |= (1 << level_index);
        _phaco.cal_param_t._lcali_tbl->val[level_index].set_power = *p_cal;
    }
    if(RAW_PHACO_MODE_T_DATA == _usCMD)
    {
        m_tCalib.project_user->cal_status.bit.phaco_vfin_index_bit |= 1 << torinsinal_index;
        _phaco.cal_param_t._vcali_tbl->val[torinsinal_index].set_power = *p_cal;
    }

    memcpy(&phaco_tmp._phaco_iCal,   (uint8_t *)_phaco.cal_param_t._lcali_tbl, sizeof(phaco_cal_param_t));
    memcpy(&phaco_tmp._phaco_vCal,   (uint8_t *)_phaco.cal_param_t._vcali_tbl, sizeof(phaco_cal_param_t));
    phaco_tmp._unuse = 0x55AA;

    Flash_Task_Write("user",(uint8_t *)&phaco_tmp, sizeof(struct Flash_User) );

    calib_updataPhacoStatus();
}



/*
*********************************************************************************************************
*	函 数 名: calib_opt_save_param
*	功能说明:
*	形    参: _usCMD
*	返 回 值: 无
*********************************************************************************************************
*/
void calib_opt_save_param(uint16_t _usCMD)
{
    if(OPT_PARAM_SET_FACTORY_DIA == _usCMD)
    {
        struct Flash_Sensor   sensor_tmp = {0};
		
		bsp_dia_default();
		_project_diathermy_getParam(&sensor_tmp);

        sensor_tmp._unuse = 0x55AA;

        Flash_Task_Write("sensor",(uint8_t *)&sensor_tmp, sizeof(struct Flash_Sensor) );
    }
    else if(OPT_PARAM_SET_FACTORY_PHACO == _usCMD)
    {
        struct Flash_User	phaco_tmp = {0};

        memcpy(_phaco.cal_param_t._lcali_tbl,   (uint8_t *)&default_phaco_lcal_table, sizeof(phaco_cal_param_t));
        memcpy(_phaco.cal_param_t._vcali_tbl,   (uint8_t *)&default_phaco_vcal_table, sizeof(phaco_cal_param_t));

        memcpy(&phaco_tmp._phaco_iCal,   (uint8_t *)_phaco.cal_param_t._lcali_tbl, sizeof(phaco_cal_param_t));
        memcpy(&phaco_tmp._phaco_vCal,   (uint8_t *)_phaco.cal_param_t._vcali_tbl, sizeof(phaco_cal_param_t));

        phaco_tmp._unuse = 0x55AA;

        Flash_Task_Write("user",(uint8_t *)&phaco_tmp, sizeof(struct Flash_User) );

    }
    else if(OPT_PARAM_SET_DIA_PWR_MAX == _usCMD)
    {
        struct Flash_Sensor   sensor_tmp = {0};

        float *p_out_max = (float *)m_tCalib.ptr_data_tmp;
		m_tCalib.dia_handle->_cal->_outMax = *p_out_max;
		
		_project_diathermy_getParam(&sensor_tmp);
    
        sensor_tmp._unuse = 0x55AA;
		
		if(_project_diathermy_setParam(&sensor_tmp) >= 0)
		{
			m_tCalib.project_user->cal_status.bit.dia_outmax_fin_bit = 1;
		}
    
    }
}






