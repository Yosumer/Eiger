/*
 * @Author: sum 476328608@qq.com
 * @Date: 2024-04-16 17:04:02
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-03 17:15:35
 * @FilePath: \Fluid module 模拟IOP 开放FPUc:\RT-ThreadStudio\workspace\Template\User\base\Flash_Task.c
 * @Description:
 *
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved.
 */
#include "includes.h"
#include <flashdb.h>
#define LOG_TAG            "task.flash"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>
#define FDBKV_FAL_NAME    "fdb_kv"

extern Data_Block  ramdata;
static struct fdb_kvdb static_kvdb = {0};
OS_TCB Flash_TaskTCB;
CPU_STK Flash_TASK_STK[FLASH_STK_SIZE];

static Flash_cof _flash_cof = {
    ._devinfo = {
        .calibration = {
            .VersionMain=0x01,
            .VersionSub =0x00
        },
        .serial = {
            .data.num1 = 0x01,
            .data.num2 = 0x02,
            .data.num3 = 0x03,
            .data.num4 = 0x04,
            .data.num5 = 0x05,
            .data.num6 = 0x06,
            .data.num7 = 0x07,
            .data.num8 = 0x08,
        }
    },
    ._app = {
        .data.FWnum1 = 0x80,
        .data.FWnum2 = 0x08,
        .data.FWnum3 = 0x80,
        .data.FWnum4 = 0x08,
        .data.FWversion1 = 0x56,
        .data.FWversion2 = 0x01,
        .data.FWversion3 = 0x00,
        .data.FWversion4 = 0x00,
        .data.FWversion5 = 0x01,
        .data.FWyear = 2024,
        .data.FWmon = 5,
        .data.FWday = 16
    },
    ._factory = {
        .data.FWnum1 = 0x80,
        .data.FWnum2 = 0x08,
        .data.FWnum3 = 0x80,
        .data.FWnum4 = 0x08,
        .data.FWversion1 = 0x56,
        .data.FWversion2 = 0x01,
        .data.FWversion3 = 0x00,
        .data.FWversion4 = 0x00,
        .data.FWversion5 = 0x01,
        .data.FWyear = 2024,
        .data.FWmon = 5,
        .data.FWday = 16
    },
    ._motor._unuse = 0xA1,
    ._pid._unuse = 0xA2,
    ._sensor._unuse = 0xA3,
    ._user._unuse = 0xA4
};

Flash_DeviceInfo _Dev = {
    .calibration = {
        .VersionMain=0x01,
        .VersionSub =0x00
    },
    .serial = {
        .data.num1 = 0x01,
        .data.num2 = 0x02,
        .data.num3 = 0x03,
        .data.num4 = 0x04,
        .data.num5 = 0x05,
        .data.num6 = 0x06,
        .data.num7 = 0x07,
        .data.num8 = 0x08,
    }
};



Flash_Cof flash_cofTable[FLASH_CODE_MAX] = {
    {
        .code = FLASH_CODE_DEV,
        .name = "dev",
        .writeptr = (void *)RAM2_BASE,
        .readptr = &_flash_cof._devinfo,
        .size = sizeof(Flash_DeviceInfo),
    },
    {
        .code = FLASH_CODE_FW1,
        .name = "app",
        .writeptr = (void *)RAM2_BASE,
        .readptr = &_flash_cof._app,
        .size = sizeof(Flash_Fw),
    },
    {
        .code = FLASH_CODE_FW2,
        .name = "factory",
        .writeptr = (void *)RAM2_BASE,
        .readptr = &_flash_cof._factory,
        .size = sizeof(Flash_Fw),
    },
    {
        .code = FLASH_CODE_PID,
        .name = "pid",
        .writeptr = (void *)RAM2_BASE,
        .readptr = &_flash_cof._pid,
        .size = sizeof(struct Flash_PID),

    },
    {
        .code = FLASH_CODE_SENSOR,
        .name = "sensor",
        .writeptr = (void *)RAM2_BASE,
        .readptr = &_flash_cof._sensor,
        .size = sizeof(struct Flash_Sensor),
    },
    {
        .code = FLASH_CODE_MOTOR,
        .name = "motor",
        .writeptr = (void *)RAM2_BASE,
        .readptr = &_flash_cof._motor,
        .size = sizeof(struct Flash_Motor),
    },
    {
        .code = FLASH_CODE_USER,
        .name = "user",
        .writeptr = (void *)RAM2_BASE,
        .readptr = &_flash_cof._user,
        .size = sizeof(struct Flash_User),
    },
};

__weak void Flash_param_init(void)
{

}


int _Flash_task_Init()
{
    bool not_formatable = RT_FALSE;
    // 初始化FLASHDB数据库
    fdb_kvdb_control(&static_kvdb,FDB_KVDB_CTRL_SET_NOT_FORMAT,&not_formatable);
    fdb_err_t err = fdb_kvdb_init(&static_kvdb, FDBKV_FAL_NAME, FDBKV_FAL_NAME, NULL, NULL);
    if (err != FDB_NO_ERR)
    {
        LOG_E("KVDB '%s' not found. Probe failed!\n", FDBKV_FAL_NAME);
        return RT_ERROR;
    }
    else
    {
        LOG_E("Probed a KVDB | %s | part_name: %s | sec_size: %d | max_size: %d |.\n", static_kvdb.parent.name,
              FDBKV_FAL_NAME, static_kvdb.parent.sec_size, static_kvdb.parent.max_size);
    }

    //判断Flash中是否有配置表
    char *str = fdb_kv_get(&static_kvdb,"config");
    if(str == NULL) //第一次配置
    {
        //写入配置表
        fdb_kv_set(&static_kvdb,"config","default");

        //初始化默认配置参数
        // 初始化配置参数
        for(int i=0; i<FLASH_CODE_MAX; i++)
        {
            struct fdb_blob blob;
            fdb_kv_set_blob(&static_kvdb, flash_cofTable[i].name, fdb_blob_make(&blob, flash_cofTable[i].readptr, flash_cofTable[i].size));
        }
    }
    OS_ERR ferr;
    // 工程模式任务
    OSTaskCreate( (OS_TCB     *) &Flash_TaskTCB,
                  (CPU_CHAR   *) "Flash_Task",
                  (OS_TASK_PTR ) Flash_Task,
                  (void       *) 0,
                  (OS_PRIO     ) FLASH_TASK_PRIO,
                  (CPU_STK    *) &Flash_TASK_STK[0],
                  (CPU_STK_SIZE) FLASH_STK_SIZE / 10,
                  (CPU_STK_SIZE) FLASH_STK_SIZE,
                  (OS_MSG_QTY  ) 10,
                  (OS_TICK     ) 0,
                  (void       *) 0,
                  (OS_OPT      ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                  (OS_ERR     *) &ferr);
}
#ifdef USE_FAL_MODE
INIT_ENV_EXPORT(_Flash_task_Init);
#endif
void Flash_Task(void *p_arg)
{
    CanData_t *candata = NULL;
    OS_MSG_SIZE msg_size_t;
    OS_ERR err;
    CPU_TS ts_t;

    

    while(1)
    {
        candata = OSTaskQPend(0,OS_OPT_PEND_BLOCKING,&msg_size_t,&ts_t,&err);
        RT_ASSERT(candata); //判断数据是否有效
        LOG_D("ASSERT ADDR:%x",candata->addr);
        LOG_D("ASSERT DATA:%x",candata->data);
        if (candata->addr == (uint16_t)&ramcof.config) {
            //这里放第一次进入正常模式进入的操作
            App_SetDevMode(candata->data);  //设置模式
        }
        if (candata->addr == (uint16_t)&ramflash.control.ctl) {
            if(candata->data == FLASH_CTL_LOAD)//加载Flash
            {
                App_Com_WriteRam(&ramflash.control.status,FLASH_STATUS_BUSY);    //将Flash置为忙
                struct fdb_blob blob;
                size_t _len = fdb_kv_get_blob(&static_kvdb,
                                              flash_cofTable[ramflash.control.IDcode].name,
                                              fdb_blob_make(&blob, flash_cofTable[ramflash.control.IDcode].writeptr,flash_cofTable[ramflash.control.IDcode].size));
                if(_len == 0)
                    LOG_E("FDB LOAD %s Faild",flash_cofTable[ramflash.control.IDcode].name);
                App_Com_WriteRam(&ramflash.control.status,FLASH_STATUS_IDEL);    //将Flash置为空闲
            }
            else if(candata->data == FLASH_CTL_STR) //将内存数据存入Flash
            {
                App_Com_WriteRam(&ramflash.control.status,FLASH_STATUS_BUSY);    //将Flash置为忙
                struct fdb_blob blob;
                fdb_kv_set_blob(&static_kvdb,flash_cofTable[ramflash.control.IDcode].name,
                                fdb_blob_make(&blob, flash_cofTable[ramflash.control.IDcode].writeptr,flash_cofTable[ramflash.control.IDcode].size));
                App_Com_WriteRam(&ramflash.control.status,FLASH_STATUS_IDEL);    //将Flash置为空闲
            }
            else if(candata->data == Flash_CTL_READ)
            {
                static CanData_t _send_data = {
                    .dir = 0x02,
                    .id =CURRENT_MODULE_ID,
                };
                App_Com_WriteRam(&ramflash.control.status,FLASH_STATUS_BUSY);    //将Flash置为忙
                uint8_t _len = (flash_cofTable[ramflash.control.IDcode].size)/sizeof(uint32_t);
                uint32_t *_addr = flash_cofTable[ramflash.control.IDcode].writeptr;
                for(uint16_t i=0; i<_len; i++)
                {
                    _send_data.addr = BSWAP_16(i);
                    _send_data.data = BSWAP_32(*_addr);
                    Data_Distribution_SendMsg((uint8_t*)&_send_data,8);
                    _addr++;
                }
                App_Com_WriteRam(&ramflash.control.status,FLASH_STATUS_IDEL);    //将Flash置为空闲
            }
        }
    }
}


void Flash_Task_Write(const char *name,uint8_t *writebuf,uint16_t len)
{
    uint8_t _index = 0;
    if(!strcmp(name, "dev"))
    {
        _index = 0;
    }
    else if(!strcmp(name, "app"))
    {
        _index = 1;
    }
    else if(!strcmp(name, "factory"))
    {
        _index = 2;
    }
    else if(!strcmp(name, "pid"))
    {
        _index = 3;
    }
    else if(!strcmp(name, "sensor"))
    {
        _index = 4;
    }
    else if(!strcmp(name, "motor"))
    {
        _index = 5;
    }
    else if(!strcmp(name, "user"))
    {
        _index = 6;
    }
    else
    {
        LOG_E("Flash_Task_Write Error");
        LOG_E("Please use name app、dev、factory、pid、sensor、motor、user");
        return;
    }
    struct fdb_blob blob;
    fdb_kv_set_blob(&static_kvdb,
                    name,
                    fdb_blob_make(&blob, writebuf,len));

}

uint8_t* Flash_Task_Read(const char *name)
{
    uint8_t _index = 0;
    if(!strcmp(name, "dev"))
    {
        _index = 0;
    }
    else if(!strcmp(name, "app"))
    {
        _index = 1;
    }
    else if(!strcmp(name, "factory"))
    {
        _index = 2;
    }
    else if(!strcmp(name, "pid"))
    {
        _index = 3;
    }
    else if(!strcmp(name, "sensor"))
    {
        _index = 4;
    }
    else if(!strcmp(name, "motor"))
    {
        _index = 5;
    }
    else if(!strcmp(name, "user"))
    {
        _index = 6;
    }
    else
    {
        LOG_E("Flash_Task_Write Error");
        LOG_E("Please use name app、dev、factory、pid、sensor、motor、user");
        return NULL;
    }
    struct fdb_blob blob;

    fdb_kv_get_blob(&static_kvdb,
                    name,
                    fdb_blob_make(&blob, flash_cofTable[_index].writeptr,flash_cofTable[_index].size));


    // 1.申请Flash的内存
    void *_saveSensor  = rt_malloc(flash_cofTable[_index].size);
    if (_saveSensor != NULL)
    {
        rt_memcpy(_saveSensor,flash_cofTable[_index].writeptr,flash_cofTable[_index].size);
    }
    return (uint8_t*)_saveSensor;
}


void Flash_mode_test(uint8_t argc, char **argv)
{
    static uint8_t _index = 0;
    fdb_err_t err = FDB_NO_ERR;
    const char* help_info[] =
    {
        [0]     = "flash_mode load [key]                - load config key:app、dev、factory、pid、sensor、motor、user",
        [1]     = "flash_mode show                      - show ram",
        [2]     = "flash_mode get [key]                 - get value of a key.",
        [3]     = "flash_mode set [key] [value]         - set value of a key.",
        [4]     = "flash_mode store [key]               - write ram to flash",
        [5]     = "flash_mode write [addr] [data]       - write ram addr data",
    };
    if (argc < 2)
    {
        rt_kprintf("Usage:\n");
        for (int i = 0; i < sizeof(help_info) / sizeof(char*); i++)
        {
            rt_kprintf("%s\n", help_info[i]);
        }
        rt_kprintf("\n");
    }
    else
    {
        uint32_t addr, size;
        const char *operator = argv[1];

        if (!strcmp(operator, "load"))
        {
            struct fdb_blob blob;
            const char *_key =  argv[2];

            if(!strcmp(_key, "dev"))
            {
                _index = 0;
            }
            else if(!strcmp(_key, "app"))
            {
                _index = 1;
            }
            else if(!strcmp(_key, "factory"))
            {
                _index = 2;
            }
            else if(!strcmp(_key, "pid"))
            {
                _index = 3;
            }
            else if(!strcmp(_key, "sensor"))
            {
                _index = 4;
            }
            else if(!strcmp(_key, "motor"))
            {
                _index = 5;
            }
            else if(!strcmp(_key, "user"))
            {
                _index = 6;
            }
            else
            {
                rt_kprintf("Please use key app、dev、factory、pid、sensor、motor、user");
                return;
            }
            fdb_kv_get_blob(&static_kvdb,
                            argv[2],
                            fdb_blob_make(&blob, flash_cofTable[_index].writeptr,flash_cofTable[_index].size));
        }
        else if(!strcmp(operator, "show"))
        {

            ulog_hexdump(flash_cofTable[_index].name,4,flash_cofTable[_index].writeptr,flash_cofTable[_index].size);
        }
        else if(!strcmp(operator, "get"))
        {
            rt_kprintf("key:%s,valve:%s\r\n",argv[2],fdb_kv_get(&static_kvdb,argv[2]));
        }
        else if(!strcmp(operator, "set"))
        {
            if (argc > 3)
            {
                err = fdb_kv_set(&static_kvdb, argv[2], argv[3]);
                if (err != FDB_NO_ERR)
                {
                    rt_kprintf("set the KV '%s' failed!\n", argv[2]);
                }
                else
                {
                    rt_kprintf("set the KV '%s', value is: %s\n", argv[2], argv[3]);
                }
            }
            else
            {
                rt_kprintf("Usage: %s.\n", help_info[3]);
            }
        }
        else if(!strcmp(operator, "store"))
        {
            struct fdb_blob blob;
            fdb_kv_set_blob(&static_kvdb,
                            argv[2],
                            fdb_blob_make(&blob, flash_cofTable[_index].writeptr,flash_cofTable[_index].size));
        }
        else if(!strcmp(operator, "write"))
        {
            if (argc < 4)
            {
                rt_kprintf("Usage: %s.\n", help_info[5]);
                return;
            }
            else
            {
                addr = strtol(argv[2], NULL, 0);
                size = argc - 3;
                uint32_t *data = rt_malloc(size*4);
                if (data)
                {
                    for (int i = 0; i < size; i++)
                    {
                        data[i] = strtol(argv[3 + i], NULL, 0);
                    }
                    void *_waddr =(void*)((uint32_t)(flash_cofTable[_index].writeptr)+(addr*4));
                    rt_memcpy(_waddr,data,size);
                    rt_free(data);
                }
                else
                {
                    rt_kprintf("Low memory!\n");
                }
            }
        }
        else
        {
            rt_kprintf("Usage:\n");
            for (int i = 0; i < sizeof(help_info) / sizeof(char*); i++)
            {
                rt_kprintf("%s\n", help_info[i]);
            }
            rt_kprintf("\n");
            return;
        }
    }
}
MSH_CMD_EXPORT(Flash_mode_test, flash_mode operate);

void proj_flash_test(void)
{
//    //写入配置表
    //fdb_kv_set(&static_kvdb,"config","default");

    //初始化默认配置参数
    // 初始化配置参数
    for(int i=0; i<FLASH_CODE_MAX; i++)
    {
        struct fdb_blob blob;
        fdb_kv_set_blob(&static_kvdb, flash_cofTable[i].name, fdb_blob_make(&blob, flash_cofTable[i].readptr, flash_cofTable[i].size));
    }
}



