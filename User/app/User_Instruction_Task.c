/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-05-14 09:32:09
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-10-23 13:54:52
 * @FilePath: \EigeV1.0.01\User\app\User_Instruction_Task.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "includes.h"
#include "Phaco_Task.h"
#include "Diathermy_Task.h"
#include "Cassette_Task.h"
#include "Suction_Task.h"
#include "Pneumatic_Task.h"
#include "instruction_Task.h"


#define SYSTEM_CMD_CODE             0XFE    //系统控制指令 


static void __system_Instruction_Ctl(uint16_t _code,uint16_t _data)
{
    switch (_code)
    {
        case SYSTEM_CMD_CODE:
            if(_data == 0x01)   //关机
            {
                //脚踏失能
                Pedal_Task_Disable();	
                //1.停止超乳
                Phaco_Task_ShutDown();
                //2.停止电凝
                Diathermy_Pedal_Disconnect();
                //3.停止波切
                Pneumatic_Pedal_Disconnect();
                //4.停止抽吸
                Suction_Task_Pedal_Disconnect();
                //5.退盒
                // ErigeEvt *pe = Q_NEW(ErigeEvt,CASSETTERELEASE_SIG);
				// QACTIVE_PUBLISH(&pe->super,NULL);
            }
            break;
        
        default:
            break;
    }
}




int User_Instruction_ShutDown_Register()
{
    Instruction_Task_RegisterFun(SYSTEM_CMD_CODE,__system_Instruction_Ctl);
    return 0;
}
INIT_APP_EXPORT(User_Instruction_ShutDown_Register);