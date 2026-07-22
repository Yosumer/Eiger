/*
 * @Author: sum 476328608@qq.com
 * @Date: 2023-10-26 10:13:41
 * @LastEditors: YouSumer 476328608@qq.com
 * @LastEditTime: 2026-07-20 11:57:52
 * @FilePath: \patagonia\light\User\bsp\opt\bsp_ultrasonic_protocol.c
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#include "bsp_ultrasonic_protocol.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#define LOG_TAG            "drv.ult"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>
#define __BSWAP_16(x) ((((x) & 0xFF00) >> 8) | (((x) & 0x00FF) << 8))
#define __BSWAP_32(x) ((((x) & 0xFF000000) >> 24) | \
                    (((x) & 0x00FF0000) >> 8)  | \
                    (((x) & 0x0000FF00) << 8)  | \
                    (((x) & 0x000000FF) << 24))


// #define ULT_UART_NAME              "uart3"  // V1.1
#define ULT_UART_NAME              "uart7"  // V2.0

// #define ULT_UART_DEBUG  



static struct rt_completion _ult_rx_down;
static struct rt_completion _ult_prase_down;    //解析成功
static float FourBytesToFloat (uint32_t data)
{
    int bits=__BSWAP_32(data);
     int s = ( bits >> 31 ) == 0 ? 1 : -1;
     int e = ( bits >> 23 ) & 0xff;
     int m = ( e == 0 ) ?
         ( bits & 0x7fffff ) << 1 :
     ( bits & 0x7fffff ) | 0x800000;
     return s * m * ( float ) pow( 2, e - 150 );    
}
static uint8_t bsp_ultprotocol_crc8(uint8_t *crc_buff,uint8_t len_in)
{
  uint8_t crc_reg = 0;    // Initial value
 
	for(uint8_t i=0;i<len_in;i++)
	{
		crc_reg ^= crc_buff[i];
		for(uint8_t j=0;j<8;j++)
		{
			if( crc_reg & 0x80)
			{
				crc_reg=(crc_reg<<1)^0x07;
//                crc_reg=(crc_reg<<1)^0x08;
			}
			else
			{
				crc_reg <<= 1;
			}
		}
	}
    return crc_reg;  	
}

static void Ultrasoni_decode_complete(void* _this, uint8_t *data,uint8_t len)
{
    ult_protocol_t *_me = (ult_protocol_t*)_this;
	uint32_t _temp = 0;
	switch(data[0])
	{
		case ULT_RECV_ERROR:
            //之前超乳读错误操作
            // _me->_info._error = __BSWAP_16(*((uint16_t*)&data[1]));
            //现在操作读错误操作
            _temp = __BSWAP_32(*((uint32_t*)&data[1]));
            _me->_info._error = _temp;
			break;
		case ULT_RECV_R1:
		case ULT_RECV_C0:	
		case ULT_RECV_FREQ:
		case ULT_RECV_CURR:
		case ULT_RECV_VOLT:
		case ULT_RECV_PWR:
        case ULT_RECV_SUMPWR:
        case ULT_RECV_VERSION:
        case ULT_RECV_HANDLE_READY_STA:
            case ULT_RECV_FPGA_VERSIONS:
        case ULT_RECV_Long_Qm:
        case ULT_RECV_Tor_Qm:
		{
			float *_index  =  &_me->_info._R1;
//			_temp = __BSWAP_32(*((uint32_t*)&data[4]));
			_temp = (*((uint32_t*)&data[4]));
            _index += (data[0] - ULT_RECV_R1);
			
			(*_index) = *((float*)&_temp);
		}
        case ULT_RECV_EOK:
            rt_completion_done(&_ult_prase_down);   //命令解析成功
            break;
		case ULT_RECV_RESET:
			rt_completion_done(&_ult_prase_down);   //命令解析成功
			break;	
        case ULT_RECV_ECOM:                     //通信或者指令错误

            break;
        
		default: break;
	}
}
void bsp_ultprotocol_Init(ult_protocol_t *_this,ult_ops *_ops,uint8_t flag)
{
    if(_this == NULL || _ops == NULL)
        return;
    _this->ops = _ops;
    _this->flag = flag;
    rt_completion_init(&_ult_rx_down);
    rt_completion_init(&_ult_prase_down);
}
void bsp_ultprotocol_IRQHandler(ult_protocol_t *_this)
{
    uint8_t receivedByte = _this->ops->read_byte();
    // 根据协议解析状态进行处理
    switch (_this->readpack.protocolState) {
        case ULT_IDLE:
            if (receivedByte == ULTRASONIC_HEAD) {
                _this->readpack.rxIndex = 0;
                _this->readpack.packflag = 0;
                _this->readpack.pack.allbyte[_this->readpack.rxIndex] = receivedByte;
                _this->readpack.protocolState = ULT_FIX1;
            }
            break;
        case ULT_FIX1:
            if (receivedByte == ULTRASONIC_FIXED1) {
                _this->readpack.pack.allbyte[(++_this->readpack.rxIndex)] = receivedByte;
                _this->readpack.protocolState = ULT_FIX2;
            } else {
                _this->readpack.protocolState = ULT_IDLE;
            }
            break;
        case ULT_FIX2:
            if (receivedByte == ULTRASONIC_FIXED2) {
                _this->readpack.pack.allbyte[(++_this->readpack.rxIndex)] = receivedByte;
                _this->readpack.protocolState = ULT_START;
            } else {
                _this->readpack.protocolState = ULT_IDLE;
            }
            break;
        case ULT_START:
            if (receivedByte == ULTRASONIC_FIXED3) {
                _this->readpack.pack.allbyte[(++_this->readpack.rxIndex)] = receivedByte;
                _this->readpack.protocolState = ULT_DATA;
            } else {
                _this->readpack.protocolState = ULT_IDLE;
            }
            break;


        case ULT_DATA:
            {
                // 数据状态下需要判断是否需要转义解析
                if(_this->flag == 1)    //需要转义
                {
                    if(receivedByte == 0x77) //转义码
                    {
                       //不需要这个数据
                        break;  //直接跳出
                    }
                }
                if (_this->readpack.rxIndex >= (MAX_ULTRASONICPROTOCOL_LENG-1)) {
                    _this->readpack.protocolState = ULT_IDLE;
                    break;
                }
                _this->readpack.pack.allbyte[(++_this->readpack.rxIndex)] = receivedByte;
                if (receivedByte == ULTRASONIC_TAIL && _this->readpack.rxIndex>=13) {
                    _this->readpack.packflag = 1;   //解包完成
                    _this->readpack.protocolState = ULT_IDLE;
                    rt_completion_done(&_ult_rx_down);
                }
            }
            break;
        default:
            _this->readpack.protocolState = ULT_IDLE;
            break;
    }
}
void static bsp_ultprotocol_Prase(ult_protocol_t *_this)
{
    _this->readpack.packflag = 0; //清除完整报标志位
    ult_recive pack;
    memcpy(&pack,&_this->readpack,sizeof(ult_recive));  //copy数据防止接收区被破坏
    // 进行CRC8校验判断数据是否正确                             
    uint8_t crc = bsp_ultprotocol_crc8((uint8_t*)&pack.pack.part.cmd,8);        //CRC 校验的范围=数据长度+命令字+数据段（不包含包头、包尾以及自己本身）
    if(crc == pack.pack.allbyte[12]) //校验CRC是否相等
    {
        if(_this->ops->decode_completion != NULL)
            // 调用接收完成回调函数
            _this->ops->decode_completion(_this,&pack.pack.part.cmd,8);
    }
    else    //crc包校验错误
    {
        LOG_E("crc faild!\r\n");
    }
}
void bsp_ultprotocol_Task(ult_protocol_t *_this)
{
	RT_UNUSED(_this);
    while (1)
    {
        rt_completion_wait(&_ult_rx_down,RT_WAITING_FOREVER);
        #ifdef ULT_UART_DEBUG
        rt_kprintf( "[Receive]:");
            for (uint8_t i = 0; i < 14; i++)
            {
                rt_kprintf("%x ",ult_handler.readpack.pack.allbyte[i]);
            }
            rt_kprintf("\r\n");
        #endif
        if(1 == ult_handler.readpack.packflag)   //解包完成
        {
            bsp_ultprotocol_Prase(&ult_handler);
        }
    }
}
void bsp_ultprotocol_sendConf(ult_protocol_t *_this,ult_cmd _cmd)
{
    if(_this == NULL || _this->ops == NULL)
        return;
    for(int i =0;i<3;i++)
    {
        _cmd.data.data[i] = __BSWAP_16(_cmd.data.data[i]);
    }
    memcpy(&_this->sendpack.pack.part.cmd,&_cmd,sizeof(_cmd));

    _this->sendpack.pack.part.head = ULTRASONIC_HEAD;
    _this->sendpack.pack.part.fix1 = ULTRASONIC_FIXED1;
    _this->sendpack.pack.part.fix2 = ULTRASONIC_FIXED2;
    _this->sendpack.pack.part.fix3 = ULTRASONIC_FIXED3;
    // 1.进行CRC校验
    uint8_t _crc = bsp_ultprotocol_crc8((uint8_t*)&_this->sendpack.pack.part.cmd,sizeof(_cmd));
    // 2.计算crc长度
    uint8_t _len = sizeof(_this->sendpack.pack.part);
    _this->sendpack.pack.sendbyte[_len] = _crc;
    _this->sendpack.pack.sendbyte[_len+1] = ULTRASONIC_TAIL;
    //这里判断是否需要转义
    if(_this->flag == 1)    //需要转义
    {
        memcpy(_this->rawsendbyte,&_this->sendpack.pack.part.head,4);   //copy 固定字节数据
        uint8_t sendindex = 4;
        // 检查发送的数据是否需要进行转义
        for(uint8_t i=0;i<8;i++)
        {
            uint8_t byte = _this->sendpack.pack.sendbyte[4+i];
            if (byte == 0X7A)   //转义成 0x77 0x7A
            {
                _this->rawsendbyte[sendindex] = 0x77;
                sendindex++;
                _this->rawsendbyte[sendindex] = 0x7A;
            }
            else if (byte == 0X75) //转义成 0x77 0x75
            {
                _this->rawsendbyte[sendindex] = 0x77;
                sendindex++;
                _this->rawsendbyte[sendindex] = 0X75;
            }
            else if (byte == 0X77)  //转义成 0x77 0x77
            {
                _this->rawsendbyte[sendindex] = 0x77;
                sendindex++;
                _this->rawsendbyte[sendindex] = 0x77;
            }
            else    //不需要转义
            {
                _this->rawsendbyte[sendindex] = byte;
                sendindex++;
            }
        }
        // 转义完成后添加CRC
        _this->rawsendbyte[sendindex] = _crc;
        // 添加数据尾
        _this->rawsendbyte[sendindex+1] = ULTRASONIC_TAIL;
        _this->ops->send_data(_this->rawsendbyte,sendindex+2);
    } 
    else                    //不需要进行转义
    {   
        _this->ops->send_data((&_this->sendpack.pack.part.head),_len+2);
    }
}


uint8_t bsp_ultprotocol_ctl_power1(ult_protocol_t *_this,uint8_t _lpow,uint8_t _vpow,uint16_t _ltime,uint16_t _vtime,uint16_t _stime)
{
    if(_this == NULL)
        return 0; //发送失败
    if(_lpow>=63)
        _lpow = 63;
    if(_vpow>=63)
        _vpow = 63;
    ult_cmd _cmd;
    _cmd.cmd.ctl.cmd =ULT_CTRL_1;
    _cmd.cmd.ctl.power = _lpow;
    _cmd.cmd.ctl.range = _vpow;
    _cmd.cmd.instruct = __BSWAP_16(_cmd.cmd.instruct);
    _cmd.data.tims.lengthwaysTime = _ltime;
    _cmd.data.tims.verticalTime = _vtime;
    _cmd.data.tims.stopTime = _stime;
    bsp_ultprotocol_sendConf(_this,_cmd);
    rt_err_t _err =  rt_completion_wait(&_ult_prase_down,5);
    LOG_D("ult send :%d %d",ULT_CTRL_1,(_err==RT_EOK?1:0));
    if(_err == RT_EOK)
        return 1;
    else
        return 0;
}
uint8_t bsp_ultprotocol_ctl_stopPwr1(ult_protocol_t *_this)
{
    return bsp_ultprotocol_ctl_power1(_this,0,0,0,0,0);
}
uint8_t bsp_ultprotocol_ctl_cmd(ult_protocol_t *_this,uint16_t cmd)
{
    ult_cmd _cmd;
    _cmd.cmd.instruct = cmd;
    rt_memset(_cmd.data.data,0,3);
    bsp_ultprotocol_sendConf(_this,_cmd);
    rt_err_t _err =  rt_completion_wait(&_ult_prase_down,5);
    LOG_D("ult send :%d %d",cmd,(_err==RT_EOK?1:0));
    if(_err == RT_EOK)
        return 1;
    else
        return 0;
}
uint8_t bsp_ultprotocol_sendcmd(ult_cmd _cmd)
{
    bsp_ultprotocol_sendConf(&ult_handler,_cmd);
    rt_err_t _err =  rt_completion_wait(&_ult_prase_down,5);
    LOG_D("ult send :%d %d",_cmd,(_err==RT_EOK?1:0));
    if(_err == RT_EOK)
        return 1;
    else
        return 0;
}

uint8_t bsp_ultprotocol_change_Port(ult_protocol_t *_this,uint8_t _port)
{
    return bsp_ultprotocol_ctl_cmd(_this,(_port == 1?ULT_CTRL_PORT1:ULT_CTRL_PORT2));
}
uint8_t bsp_ultprotocol_reset_Error(ult_protocol_t *_this)
{
    // 手动清除之前的错误
    bsp_ultprotocol_ctl_stopPwr1(_this);    //先停止再清错
     _this->_info._error = 0;
    return bsp_ultprotocol_ctl_cmd(_this,ULT_CTRL_CLE_ERROR);
}
uint8_t bsp_ultprotocol_scan_Freq(ult_protocol_t *_this)
{
    _this->_info._CDE = 0;
    _this->_info._handle_scan_sta = 0;
    return bsp_ultprotocol_ctl_cmd(_this,ULT_DEBUG_SCANFREQ);
}

uint32_t bsp_ultprotocol_query_handle_det_ready(ult_protocol_t *_this)
{
    if(bsp_ultprotocol_ctl_cmd(_this,ULT_QUERY_HANDLE_DET_STA))
        return _this->_info._handle_scan_sta;
}

uint8_t bsp_ultprotocol_reset_Sumpower(ult_protocol_t *_this)
{
    _this->_info._sumPower = 0;
    return bsp_ultprotocol_ctl_cmd(_this,ULT_CTRL_CLE_SUMPWR);
}
uint16_t bsp_ultprotocol_query_Error(ult_protocol_t *_this)
{
    return _this->_info._error;
    // return bsp_ultprotocol_ctl_cmd(_this,ULT_QUERY_ERROR);
}

float bsp_ultprotocol_query_R1(ult_protocol_t *_this)
{
    if(bsp_ultprotocol_ctl_cmd(_this,ULT_QUERY_R1))
        return _this->_info._R1;
}
float bsp_ultprotocol_query_C0(ult_protocol_t *_this)
{
    if(bsp_ultprotocol_ctl_cmd(_this,ULT_QUERY_C0))
        return _this->_info._C0;
}
float bsp_ultprotocol_query_Freq(ult_protocol_t *_this)
{
    if(bsp_ultprotocol_ctl_cmd(_this,ULT_QUERY_FREQ))
        return _this->_info._Freq;
}
uint8_t bsp_ultprotocol_clear_error(ult_protocol_t *_this)
{
    _this->_info._error=0;
    return bsp_ultprotocol_ctl_cmd(_this,ULT_DEBUG_CLR_ERROR);
}
float bsp_ultprotocol_query_Current(ult_protocol_t *_this)
{
    if(bsp_ultprotocol_ctl_cmd(_this,ULT_QUERY_CURR))
        return _this->_info._I;
}
float bsp_ultprotocol_query_Volt(ult_protocol_t *_this)
{
    if(bsp_ultprotocol_ctl_cmd(_this,ULT_QUERY_VOLT))
        return _this->_info._V;
}
float bsp_ultprotocol_query_Power(ult_protocol_t *_this)
{
    if(bsp_ultprotocol_ctl_cmd(_this,ULT_QUERY_PWR))
        return _this->_info._power;
}
float bsp_ultprotocol_query_SumPower(ult_protocol_t *_this)
{
    if(bsp_ultprotocol_ctl_cmd(_this,ULT_QUERY_SUMPWR))
        return _this->_info._sumPower;
}

ult_protocol_t ult_handler = {0};
static ult_ops _ops = {0}; 
rt_device_t ult_uart = RT_NULL;
static uint8_t _ult_uart_read_byte()
{
	uint8_t _ch;
	rt_device_read(ult_uart,0,&_ch,1);
	return _ch;
}
static void _ult_uart_send_data(uint8_t *data,uint16_t len)
{

	rt_device_write(ult_uart, 0, data, len);
    #ifdef ULT_UART_DEBUG
        rt_kprintf( "[Send]:");
        for (uint8_t i = 0; i < len; i++)
        {
            rt_kprintf("%x ",data[i]);
        }
         rt_kprintf("\r\n");
    #endif // DEBUG
}

/* 接收数据回调函数 */
static rt_err_t _ult_uart_rx_ind(rt_device_t dev, rt_size_t size)
{
		SCB_CleanInvalidateDCache();
    for (size_t i = 0; i < size; i++)
    {
        bsp_ultprotocol_IRQHandler(&ult_handler);
    }
}

#define  UART_ULT_TASK_SIZE     1024
rt_uint8_t  _ult_uart_task_stack[UART_ULT_TASK_SIZE];
static struct rt_thread _ult_uart_tcb;  // can接收任务句柄
int __bsp_ultprotocol_test(void)
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
	/* step1：修改串口配置参数 */
	config.baud_rate = BAUD_RATE_115200;	//修改波特率为 115200
	
	/* step2：查找串口设备 */
    ult_uart  = rt_device_find(ULT_UART_NAME);
	
    if (!ult_uart)
    {
        LOG_E("find %s failed!\n",ULT_UART_NAME);
    }
	/* step3：控制串口设备。通过控制接口传入命令控制字，与控制参数 */
	rt_device_control(ult_uart, RT_DEVICE_CTRL_CONFIG, &config);	
    
	/* step3：打开串口设备。以中断接收及轮询发送模式打开串口设备 */
		// rt_device_open(ult_uart, RT_DEVICE_FLAG_INT_RX|RT_DEVICE_FLAG_INT_TX);	
   rt_device_open(ult_uart, RT_DEVICE_FLAG_DMA_RX|RT_DEVICE_FLAG_INT_TX);
	/*注册回调函数*/
	rt_device_set_rx_indicate(ult_uart,_ult_uart_rx_ind);

    _ops.read_byte = _ult_uart_read_byte;
    _ops.send_data = _ult_uart_send_data;
    _ops.decode_completion = Ultrasoni_decode_complete;
    bsp_ultprotocol_Init(&ult_handler,&_ops,0);

    rt_err_t res;
    res =  rt_thread_init(&_ult_uart_tcb,"_ult_uart_task",bsp_ultprotocol_Task,RT_NULL,_ult_uart_task_stack,UART_ULT_TASK_SIZE,10,10);
    RT_ASSERT(res == RT_EOK);
    rt_thread_startup(&_ult_uart_tcb);  

    // rt_thread_t _ult =rt_thread_create("_ult_uart_task",bsp_ultprotocol_Task,RT_NULL,1024,20,10);
    // if(RT_NULL ==_ult)
    //     LOG_E("Create ult rx task faild");
    // rt_thread_startup(_ult);



    // static uint16_t _cmd_16 = ULT_QUERY_R1;
    // static uint8_t _flag = 0;
    // static struct 
    // {
    //     uint8_t power;
    //     uint8_t ver;
    //     uint16_t    ltim;
    //     uint16_t    vtim;
    //     uint16_t    stop;
    // }_ctl_power;
    
    // while (1)
    // {

    //     if(_flag)
    //         bsp_ultprotocol_ctl_cmd(&ult_handler,_cmd_16);
    //     else
    //         bsp_ultprotocol_ctl_power1(&ult_handler,_ctl_power.power,_ctl_power.ver,_ctl_power.ltim,_ctl_power.vtim,_ctl_power.stop);
    //     rt_thread_delay(100);
    // }
    
}
INIT_ENV_EXPORT(__bsp_ultprotocol_test);


void ult(int argc, char *argv[])
{
    if(argc>1)
    {
        if (!strcmp(argv[1], "power"))
        {
            bsp_ultprotocol_ctl_power1(&ult_handler,5,0,5,0,0);
        }
        else if(!strcmp(argv[1], "stop"))
        {
            bsp_ultprotocol_ctl_stopPwr1(&ult_handler);
        }
        else if(!strcmp(argv[1], "rerr"))
        {
            rt_kprintf("ult error:%d",bsp_ultprotocol_query_Error(&ult_handler));
        }
        else if (!strcmp(argv[1], "cerr"))
        {
            bsp_ultprotocol_reset_Error(&ult_handler);
        }
        else if (!strcmp(argv[1], "rpwr"))
        {
            rt_kprintf("ult pwr:%0.2f",bsp_ultprotocol_query_Power(&ult_handler));
        }
        else if(!strcmp(argv[1], "rsumpwr"))
        {
            rt_kprintf("ult pwr:%0.2f",bsp_ultprotocol_query_SumPower(&ult_handler));
        }
        else if(!strcmp(argv[1], "csumpwr"))

        {
            bsp_ultprotocol_reset_Sumpower(&ult_handler);
        }
        else if (!strcmp(argv[1], "port"))
        {
            bsp_ultprotocol_change_Port(&ult_handler,atoi(argv[2]));
            rt_kprintf("ult set port:%d",atoi(argv[2]));
        }      
    }
    else
    {
        rt_kprintf("Usage:\n");
        rt_kprintf("ult power               - ult power 10 5ms \n");
        rt_kprintf("ult stop                - ult stop\n");
        rt_kprintf("ult rerr              - ult read error\n");
        rt_kprintf("ult cerr              - ult clear error\n");
        rt_kprintf("ult rpwr              - ult read power\n");
        rt_kprintf("ult port              - ult change port 1/2\n");
    }

}
MSH_CMD_EXPORT(ult,phaco_test);