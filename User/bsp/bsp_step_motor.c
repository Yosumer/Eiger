
#include <rtthread.h>
#include "bsp_step_motor.h"

#include "list.h"
#include "math.h"
#define LOG_TAG            "drv.spMotor"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>

LIST(motor); //单指令链表
static uint8_t	motor_mempool[12*5];
static struct rt_mempool motor_mp;
typedef struct _motor_fun_l{
	struct _motor_fun_l *next;			//指向下一个链表
    bsp_step_motor_t    *motor;
}motor_fun_l;



void _bsp_step_motor_App_Task(void *p)
{
    while (1)
    {
        motor_fun_l *_item = list_head(motor);
        while (_item != NULL)
        {
            bsp_step_motor_t    *motor = _item->motor;
            bsp_step_motor_Task(motor);
            _item = list_item_next(_item);
        }
        rt_thread_mdelay(100);  // 100ms更新一下 
    }
}

static int bsp_step_motorTable_Init()
{
    rt_err_t err;
    err = rt_mp_init(&motor_mp, "motor_mp", &motor_mempool[0], sizeof(motor_mempool), 8);
    list_init(motor);

    rt_thread_t _tmc =rt_thread_create("_motor_task",_bsp_step_motor_App_Task,RT_NULL,1024,25,10);
    if(RT_NULL ==_tmc)
        LOG_E("Create _motor_task faild");
    rt_thread_startup(_tmc);
} 
INIT_COMPONENT_EXPORT(bsp_step_motorTable_Init);

uint8_t bsp_step_motor_Init(bsp_step_motor_t *_this,bsp_step_motor_ops *_ops)
{
    RT_ASSERT(_this);
    RT_ASSERT(_ops);
    _this->ops = _ops;
    uint8_t _ret = _this->ops->init(_this);
    if(_ret == 0)
    {
        LOG_E("bsp_step_motor_Init Faild");
		return 0;
    }
    motor_fun_l * _l = rt_mp_alloc(&motor_mp,RT_WAITING_FOREVER);
	if(_l == RT_NULL)
	{
		LOG_E("rt_mp_alloc motor_fun_l error");
		return 0;
	}
    _l->motor = _this;
	list_add(motor,_l);
    return _ret;
}
void bsp_step_motor_Config(bsp_step_motor_t *_this,bsp_step_motor_conf *_cof)
{
    RT_ASSERT(_this);
    RT_ASSERT(_cof);
    _this->data._amax =  _cof->_amax;
    _this->data._dmax = _cof->_dmax;
    _this->data._targetspeed = _cof->_vmax;
    _this->cof = _cof;
    _this->ops->config(_this,_cof);
}
void bsp_step_motor_Rotate(bsp_step_motor_t *_this,int32_t velocity)
{
    RT_ASSERT(_this);
    _this->data._targetspeed = velocity;
    _this->ops->rotate(_this,velocity);
}
void bsp_step_motor_Stop(bsp_step_motor_t *_this)
{
    RT_ASSERT(_this);
    _this->data._targetspeed = 0;
    _this->ops->stop(_this);
}
void bsp_step_motor_MoveTo(bsp_step_motor_t *_this,int32_t position)
{
    RT_ASSERT(_this);
    _this->data._targetPos = position;
    _this->ops->moveTo(_this,position);
}
void bsp_step_motor_MoveBy(bsp_step_motor_t *_this,int32_t step)
{
    RT_ASSERT(_this);
    _this->data._targetPos = _this->ops->get_position(_this) + step;
    _this->ops->moveBy(_this,step);
}
void bsp_step_motor_ResetPos(bsp_step_motor_t *_this)
{
    RT_ASSERT(_this);
    _this->data._readPos = 0;
    _this->data._targetPos = 0;
    _this->ops->set_position(_this,0);
}
int32_t bsp_step_motor_GetPosition(bsp_step_motor_t *_this)
{
    RT_ASSERT(_this);
    return _this->data._readPos;
}
int bsp_step_motor_GetEncodePosition(bsp_step_motor_t *_this)
{
    RT_ASSERT(_this);
    return _this->data._encodePos;
}
uint8_t bsp_step_motor_IsEncodePositionInRange(bsp_step_motor_t *_this, int min, int max) {
    int position = bsp_step_motor_GetEncodePosition(_this);
    return position >= min && position <= max;
}
// 检查速度是否在范围内
uint8_t bsp_step_motor_IsLoseSpeed(bsp_step_motor_t *_this, float duty) {

    float min = ((_this->data._encoderps * (1.0f-duty)));  // 计算最小速度
    float max = ((_this->data._encoderps * (1.0f+duty)));  // 计算最大速度
		
    min = fabsf(min);
    max = fabsf(max);
    if(fabsf(_this->data._rps)<0.4)    // 小于0.4rps 不进行失速检测 直接任务正常
        return 0;
    return !(fabsf(_this->data._rps) >= min && fabsf(_this->data._rps) <= max);
} 

void bsp_step_motor_Task(bsp_step_motor_t *_this)
{
    RT_ASSERT(_this);
    _this->ops->task(_this);
    _this->data._readPos = _this->ops->get_position(_this);
    _this->data._readspeed = _this->ops->get_velocity(_this);
    _this->data._status = _this->ops->get_status(_this);
    if(_this->cof)
        _this->data._rps = (float)(_this->data._readspeed) /(float) _this->cof->_spr;
    if(_this->ops->get_encode)
    {
        _this->data._encodePos = _this->ops->get_encode(_this);
        uint32_t _tick = rt_tick_get_millisecond();
        _this->data._encodespeed = (int32_t)((_this->data._encodePos - _this->data._encodePoslast)/(float)(_tick-_this->data._lasttick) * 1000);
        _this->data._encoderps =  (float)(_this->data._encodespeed) / (float)_this->cof->_espr;
        _this->data._lasttick   = _tick;
        _this->data._encodePoslast = _this->data._encodePos;
    }
}


bsp_step_motor_t * bsp_step_motor_findbyname(const char *name)
{
    uint8_t _len = list_length(motor);
    motor_fun_l *_item = list_head(motor);
    for (uint8_t i = 0; i < _len; i++)
    {
        if (!strcmp(_item->motor->name,name))
        {
            return _item->motor;
        }
        _item = list_item_next(_item);
    }
    return RT_NULL;
}


static void bsp_step_motor_listAll()
{
    uint8_t _len = list_length(motor);
    motor_fun_l *_item = list_head(motor);
    for (uint8_t i = 0; i < _len; i++)
    {
         rt_kprintf("Motor Name:%s",_item->motor->name);
        _item = list_item_next(_item);
    }
}


#ifdef FINSH_USING_MSH
static void step_motror(int argc, char *argv[]) 
{
    if(argc >2)
    {

        if (!strcmp(argv[1], "list"))
        {
            bsp_step_motor_listAll();
        }
        

        if (!strcmp(argv[2], "rotate"))
        {
            bsp_step_motor_t *handler = bsp_step_motor_findbyname(argv[1]);
            if (handler == RT_NULL)
            {
                rt_kprintf("CANNOT find motor: %s\n", argv[1]);
                return;
            }
            
            bsp_step_motor_Rotate(handler,atoi(argv[3]));
        }
        else if (!strcmp(argv[2], "stop"))
        {
            bsp_step_motor_t *handler = bsp_step_motor_findbyname(argv[1]);
            if (handler == RT_NULL)
            {
                rt_kprintf("CANNOT find motor: %s\n", argv[1]);
                return;
            }
            bsp_step_motor_Stop(handler);
        }
        else if (!strcmp(argv[2], "moveTo"))
        {
            bsp_step_motor_t *handler = bsp_step_motor_findbyname(argv[1]);
            if (handler == RT_NULL)
            {
                rt_kprintf("CANNOT find motor: %s\n", argv[1]);
                return;
            }
            bsp_step_motor_MoveTo(handler,atoi(argv[3]));
        }
        else if (!strcmp(argv[2], "moveBy"))
        {
            bsp_step_motor_t *handler = bsp_step_motor_findbyname(argv[1]);
            if (handler == RT_NULL)
            {
                rt_kprintf("CANNOT find motor: %s\n", argv[1]);
                return;
            }
            bsp_step_motor_MoveBy(handler,atoi(argv[3]));
        }
        else if (!strcmp(argv[2], "resetPos"))
        {
            bsp_step_motor_t *handler = bsp_step_motor_findbyname(argv[1]);
            if (handler == RT_NULL)
            {
                rt_kprintf("CANNOT find motor: %s\n", argv[1]);
                return;
            }
            bsp_step_motor_ResetPos(handler);
        }
        else
        {
            rt_kprintf("Unknown command. Please enter 'step_motror' for help\n");
        }
    }
    else
    {
        rt_kprintf("Usage:\n");
        rt_kprintf("step_motror list                \n");
        rt_kprintf("step_motror name rotate  vol              \n");
        rt_kprintf("step_motror name stop                    \n");
        rt_kprintf("step_motror name moveTo  pos             \n");
        rt_kprintf("step_motror name moveBy  tick            \n");
        rt_kprintf("step_motror name resetPos           \n");

    }
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(step_motror, step_motror Motor cmd);

#endif


