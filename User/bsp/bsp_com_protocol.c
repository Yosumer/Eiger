/*------------------------------------------------------------------------------
 * @file    BSP_COM_PROTOCOL.C
 * @author  FIRMWARE TEAM 
 * @date    2024/01/02 14:12:22
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
#include "bsp_com_protocol.h"
static uint8_t  bsp_protocol_crc8(uint8_t *crc_buff,uint8_t len_in)
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
			}
			else
			{
				crc_reg <<= 1;
			}
		}
	}
    return crc_reg;  
}

/**
 * @description: 初始化通信协议
 * @param {protocol_com_t} *_this
 * @param {uint8_t} flag    1:使能转译发送 0:不能使能转译发送
 * @return NULL
 */
void bsp_protocol_com_Init(protocol_com_t *_this,uint8_t flag)
{
    if(_this == NULL || _this->ops == NULL)
        return;
    _this->flag = flag;
}
/**
 * @description: 接收中断回调函数，使用时放在数据接收中断中
 * @param {protocol_com_t} *_this
 * @return {*}
 */
void bsp_protocol_com_IRQHandler(protocol_com_t *_this)
{
    uint8_t receivedByte = _this->ops->read_byte();
    // 根据协议解析状态进行处理
    switch (_this->readpack.protocolState) {
        case PROTOCOL_STATE_IDLE:
            if (receivedByte == PROTOCOL_HEAD) {
                _this->readpack.rxIndex = 0;
                _this->readpack.packflag = 0;
                _this->readpack.pack.allbyte[_this->readpack.rxIndex] = receivedByte;
                _this->readpack.protocolState = PROTOCOL_STATE_FIX1;
            }
            break;
        case PROTOCOL_STATE_FIX1:
            if (receivedByte == PROTOCOL_FIXED1) {
                _this->readpack.pack.allbyte[(++_this->readpack.rxIndex)] = receivedByte;
                _this->readpack.protocolState = PROTOCOL_STATE_FIX2;
            } else {
                _this->readpack.protocolState = PROTOCOL_STATE_IDLE;
            }
            break;
        case PROTOCOL_STATE_FIX2:
            if (receivedByte == PROTOCOL_FIXED2) {
                _this->readpack.pack.allbyte[(++_this->readpack.rxIndex)] = receivedByte;
                _this->readpack.protocolState = PROTOCOL_STATE_START;
            } else {
                _this->readpack.protocolState = PROTOCOL_STATE_IDLE;
            }
            break;
        case PROTOCOL_STATE_START:
            if (receivedByte == PROTOCOL_FIXED3) {
                _this->readpack.pack.allbyte[(++_this->readpack.rxIndex)] = receivedByte;
                _this->readpack.protocolState = PROTOCOL_STATE_REAL_DATA;
            } else {
                _this->readpack.protocolState = PROTOCOL_STATE_IDLE;
            }
            break;
        case PROTOCOL_STATE_REAL_DATA:
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

                _this->readpack.pack.allbyte[(++_this->readpack.rxIndex)] = receivedByte;
                if (_this->readpack.rxIndex >= MAX_RX_LENG) {
                    _this->readpack.protocolState = PROTOCOL_STATE_IDLE;
                }
                if (receivedByte == PROTOCOL_TAIL) {
                    _this->readpack.packflag = 1;   //解包完成
                    _this->readpack.protocolState = PROTOCOL_STATE_IDLE;
                }
            }
            break;
        default:
            _this->readpack.protocolState = PROTOCOL_STATE_IDLE;
            break;
    }
}
/**
 * @description: 接收数据包解析函数
 * @param {ult_protocol_t} *_this
 * @return {*}
 */
void static bsp_protocol_com_Prase(protocol_com_t *_this)
{
    _this->readpack.packflag = 0; //清除完整报标志位
    protocol_com_recive pack;
    memcpy(&pack,&_this->readpack,sizeof(protocol_com_recive));  //copy数据防止接收区被破坏
    uint8_t _len = pack.pack.part.len;   //接收数据长度
    // 进行CRC8校验判断数据是否正确                             
    uint8_t crc = bsp_protocol_crc8(&pack.pack.part.len,_len);        //CRC 校验的范围=数据长度+命令字+数据段（不包含包头、包尾以及自己本身）
    if(crc == pack.pack.allbyte[4+_len]) //校验CRC是否相等
    {
        // 调用接收完成回调函数
        _this->ops->decode_completion(&pack.pack.part.len,_len);
    }
    else    //crc包校验错误
    {
        printf("crc faild!\r\n");
    }
    
}
void bsp_protocol_com_Task(protocol_com_t *_this)
{
    if(1 == _this->readpack.packflag)   //解包完成
    {
        bsp_protocol_com_Prase(_this);
    }
}
void bsp_protocol_com_SendConf(protocol_com_t *_this,uint8_t *usedata)
{
    if(_this == NULL || _this->ops == NULL)
        return;
    memcpy(&_this->sendpack.pack.part.len,usedata,*usedata);

    _this->sendpack.pack.part.head = PROTOCOL_HEAD;
    _this->sendpack.pack.part.fix1 = PROTOCOL_FIXED1;
    _this->sendpack.pack.part.fix2 = PROTOCOL_FIXED2;
    _this->sendpack.pack.part.fix3 = PROTOCOL_FIXED3;
    // 1.进行CRC校验
    uint8_t _crc = bsp_protocol_crc8(&_this->sendpack.pack.part.len,*usedata);
    // 2.计算crc长度
    uint8_t _len = *usedata;
    _this->sendpack.pack.sendbyte[4+_len] = _crc;
    _this->sendpack.pack.sendbyte[4+_len+1] = PROTOCOL_TAIL;
    //这里判断是否需要转义
    if(_this->flag == 1)    //需要转义
    {
        memcpy(_this->rawsendbyte,&_this->sendpack.pack.part.head,4);   //copy 固定字节数据
        uint8_t sendindex = 4;
        // 检查发送的数据是否需要进行转义
        for(uint8_t i=0;i<_len;i++)
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
        _this->rawsendbyte[sendindex+1] = PROTOCOL_TAIL;
        _this->ops->send_data(_this->rawsendbyte,sendindex+2);
    } 
    else                    //不需要进行转义
    {   
        _this->ops->send_data((&_this->sendpack.pack.part.head),4+_len+2);
    }
}
/*----------------------------------------------------------------------------*/

