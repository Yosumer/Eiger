#include "includes.h"
#include "Data_Task.h"
#include "Pedal_User_Task.h"
#include "bsp_com_protocol.h"
#include "bsp_step_tmc5160.h"
#include "finsh.h"
#include "Suction_Task.h"
#define SENSOR_UART_NAME "uart8"

rt_uint8_t  sensor_tx_task_stack[512];
static struct rt_thread sensor_tx_tcb;  // 任务句柄
rt_device_t _uart8 = RT_NULL;

protocol_com_t protocol = {0};
protocol_com_ops _pro_ops = {0};



struct {
    uint8_t    len;         //发送数据包的第一个数据
    float  motor_speed;
    uint32_t  valve_infusion;
    uint32_t  valve_reflux;
    uint32_t  pedal_stage;
    float     vacuum_pressure;
}Sensor_Transfer_Data; 


static void _Sensor_Decode_Fun(uint8_t *rxbuf,uint8_t rxlen) {
    
}

static uint8_t _Sensor_uart_read_byte(void) {
	uint8_t _ch;
	rt_device_read(_uart8,0,&_ch,1);
	return _ch;
}

static void _Sensor_uart_write_byte(uint8_t *data,uint16_t len) {
    rt_device_write(_uart8, 0, data, len);
}

static rt_err_t _Sensor_uart_rx_ind(rt_device_t dev, rt_size_t size)
{
    bsp_protocol_com_IRQHandler(&protocol);
}



void Sensor_Transfer_Task(void *p_arg)
{
    protocol_com_t *protocol = (protocol_com_t *)p_arg;
    while (1) {
        bsp_protocol_com_Task(protocol);
        Sensor_Transfer_Data.motor_speed = tmc5160_motor.data._rps;
        Sensor_Transfer_Data.valve_infusion = ramdata._valve.valve.valve_infusion;
        Sensor_Transfer_Data.valve_reflux = ramdata._valve.valve.valve_reflux;
        Sensor_Transfer_Data.vacuum_pressure = _Suction_Task_GetPressure();
        bsp_protocol_com_SendConf(protocol,(uint8_t*)&Sensor_Transfer_Data);
        rt_thread_mdelay(20);
    }
}
void Sensor_Task_Pedal_Deal(uint16_t _code,uint32_t _data)
{
    uint32_t    _cmd = 1;      //获取踏板的配置
    _code = BSWAP_16(_code);
    if(_code>FOOTBOARD_SWITCH)  //开关占空比混合
    {
        _code -= FOOTBOARD_SWITCH;
    }
    if(_cmd>0&&_cmd<FOOTBOARD_SWITCH)   //踏板形成控制
    {
        switch (_code)
        {
            case FOOTBOARD_LINE_ONE:   //响应踏板第一阶段
                Sensor_Transfer_Data.pedal_stage = 1;
            break;
            case FOOTBOARD_LINE_SECOND: //响应踏板第二阶段
                Sensor_Transfer_Data.pedal_stage = 2;
            break;
            case FOOTBOARD_LINE_THIRD:
            {
                Sensor_Transfer_Data.pedal_stage = 3;
            }
            break;
            default:
                Sensor_Transfer_Data.pedal_stage = 3;
                break;
        }
        if(_code == FOOTBOARD_ZERO) //踏板回到零点
            Sensor_Transfer_Data.pedal_stage = 0;
    }
    else if(_cmd > FOOTBOARD_END)   //响应按键
    {

    }
    else    //不检测
    {

    }

}
int Sensor_Transfer_Task_Init() {

	// 串口初始化
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
	/* step1：修改串口配置参数 */
	config.baud_rate = BAUD_RATE_115200;	//修改波特率为 115200
	
	/* step2：查找串口设备 */
    _uart8  = rt_device_find(SENSOR_UART_NAME);
	
    if (!_uart8)
    {
        rt_kprintf("find %s failed!\n",SENSOR_UART_NAME);
    }
	/* step3：控制串口设备。通过控制接口传入命令控制字，与控制参数 */
	rt_device_control(_uart8, RT_DEVICE_CTRL_CONFIG, &config);	
    
	/* step3：打开串口设备。以中断接收及轮询发送模式打开串口设备 */
	rt_device_open(_uart8, RT_DEVICE_FLAG_INT_RX);	
	/*注册回调函数*/
	rt_device_set_rx_indicate(_uart8,_Sensor_uart_rx_ind);

    _pro_ops.read_byte = _Sensor_uart_read_byte;
    _pro_ops.send_data = _Sensor_uart_write_byte;
    _pro_ops.decode_completion = _Sensor_Decode_Fun;
    protocol.ops = &_pro_ops;
    Sensor_Transfer_Data.len = sizeof(Sensor_Transfer_Data);
    bsp_protocol_com_Init(&protocol,0);
    Pedal_Task_RegisterFun(Sensor_Task_Pedal_Deal);
    rt_err_t res;
    res =  rt_thread_init(&sensor_tx_tcb,"sensor_tx",Sensor_Transfer_Task,&protocol,sensor_tx_task_stack,512,28,10);
    RT_ASSERT(res == RT_EOK);
    rt_thread_startup(&sensor_tx_tcb);  
}
//INIT_APP_EXPORT(Sensor_Transfer_Task_Init);