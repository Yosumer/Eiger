#ifndef __APP_USER_INSTRUCTION_H_
#define __APP_USER_INSTRUCTION_H_

#include <stdint.h>


typedef union{
	uint32_t uldata;
	struct {
		uint32_t phaco_curr_lfin_bit	: 1;		/*  */
		uint32_t phaco_curr_vfin_bit	: 1;
		uint32_t phaco_rev0				: 1;
		uint32_t dia_freq_bit			: 1;
		uint32_t dia_fin_bit			: 1;
		uint32_t dia_curr_fin_bit		: 1;
		uint32_t dia_volt_fin_bit		: 1;
		uint32_t dia_outmax_fin_bit		: 1;
		//uint32_t rev0 					: 0;
		
		uint32_t phaco_lfin_index_bit	: 11;/*  */			
		uint32_t phaco_vfin_index_bit 	: 11;
		uint32_t rev3 					: 2;			
	}bit;
}cal_opt_status_t;

typedef enum{
    OPT_PARAM_SET_NONE,
    OPT_PARAM_SET_FACTORY_DIA,
    OPT_PARAM_SET_FACTORY_PHACO,
    OPT_PARAM_SET_DIA_PWR_MAX,
}OPT_PARAM_SET;


typedef struct{
	uint8_t lPower;
	uint8_t vPower;
	uint16_t lTime;
	uint16_t vTime;
	uint16_t sTime;
}calib_phaco_raw_data_t;

typedef enum {
	RAW_PHACO_DISABLE,
	RAW_PHACO_MODE_L_DATA,	
	RAW_PHACO_MODE_T_DATA,	
	RAW_PHACO_MODE_ALL,
	RAW_PHACO_CMD_NUM_MAX,
}raw_phaco_mode_e;


enum CALIB_STEP_INDEX {
    CALIB_SET_DAC_400,
    CALIB_SET_DAC_600,
    CALIB_SET_DAC_800,
    CALIB_SET_DAC_1000,
    CALIB_SET_DAC_1200,
    CALIB_NUMS,
};

typedef struct {
    float dac_out[CALIB_NUMS];
} calib_dia_dac_set_t;

typedef struct{
	uint32_t  curr_adc;
	uint32_t  volt_adc;
	cal_opt_status_t		cal_status;
	calib_dia_dac_set_t		cal_dac;
}project_user_data_t;


extern int CALIB_diathermy_setCalDacTable	(uint16_t _usIndex);
extern void calib_diathermy_setDac			(uint16_t _usData);
extern void calib_diathermy_readSensor		(uint16_t _usCMD);
extern void calib_diathermy_caculate		(uint16_t _usCMD);
extern void calib_phaco_set_raw				(uint16_t _usCMD);
extern void calib_phaco_set_powIndex		(uint16_t _usCMD);
extern void calib_phaco_save_param			(uint16_t _usCMD);
extern void calib_opt_save_param			(uint16_t _usCMD);
extern void calib_diathermy_setMode			(uint16_t _usData);

#endif /* __APP_USER_INSTRUCTION_H_  */
