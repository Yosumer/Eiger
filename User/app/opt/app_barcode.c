/*
*********************************************************************************************************
*
*	模块名称 : 二维码数据储存应用程序
*	文件名称 : app_barcode.c
*	版    本 : V1.0
*	说    明 : 本模块主要用于二维码数据的储存
*	修改记录 :
*		版本号   日期        作者     说明
*		V1.0    2024-09-30  Zhang   正式发布
*
*	Copyright (C), 2022-2032, 思埃然医疗 Medical
*
*********************************************************************************************************
*/


/* 包含头文件 ----------------------------------------------------------------*/
#include "includes.h"
#include "app_barcode.h"

/* 私有类型定义 --------------------------------------------------------------*/

/* 私有宏定义 ----------------------------------------------------------------*/
#define EXAMPLE_BARCODE_CMD 	250

/* 私有变量 ------------------------------------------------------------------*/
#define RAM_BARCODE_ADDR 	RAM2_BASE + 0x0100*4


/* 私有函数原形 --------------------------------------------------------------*/
static void example_BarcodeSaveCMD(uint16_t _code,uint16_t _data);
/* 函数体 --------------------------------------------------------------------*/



/*
*********************************************************************************************************
*	函 数 名:
*	功能说明:
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void barcode_handle(uint8_t _ucCmd)
{
    if(1 == _ucCmd) //
    {
        barcode_t		user_temp = {0};
        memcpy(&user_temp,  (uint8_t *)RAM_BARCODE_ADDR, sizeof(barcode_t));

        user_temp.info.index[0] = 'L';
        user_temp.info.index[1] = '0';
        user_temp.info.index[2] = '9';
        user_temp.info.index[3] = ' ';
        user_temp.info.rev1[0]  = ' ';
        user_temp.info.rev1[1]  = ' ';
        user_temp.info.rev1[2]  = ' ';
        user_temp.info.rev1[3]  = ' ';
        Flash_Task_Write("dev",(uint8_t *)&user_temp,sizeof(SerialNum));

        uint8_t *pdt;
        pdt = Flash_Task_Read("dev");

        memcpy(&ramdata.serial, pdt, sizeof(barcode_t) );

    }
    else if( 2 == _ucCmd)
    {
        uint8_t *pdt;
        pdt = Flash_Task_Read("dev");
        memcpy(&ramdata.serial, pdt, sizeof(barcode_t) );

    }

}





/*
*********************************************************************************************************
*	函 数 名: Project_BarcodeSave
*	功能说明: 保存二维码
*	形    参: _cmd 	指令
*	返 回 值: 无
*********************************************************************************************************
*/
int  example_Barcode_register(void)
{
    Instruction_Task_RegisterFun(EXAMPLE_BARCODE_CMD,(void *)example_BarcodeSaveCMD);

    return 0;
}
INIT_APP_EXPORT(example_Barcode_register);

/*
*********************************************************************************************************
*	函 数 名: Project_BarcodeSave
*	功能说明: 保存二维码
*	形    参: _cmd 	指令
*	返 回 值: 无
*********************************************************************************************************
*/
static void example_BarcodeSaveCMD(uint16_t _code,uint16_t _data)
{
    switch(_code)
    {
    case EXAMPLE_BARCODE_CMD:
        barcode_handle(_data);
        break;

    default:
        break;
    }

}


/***************************** 思埃然医疗 Medical (END OF FILE) *********************************/
