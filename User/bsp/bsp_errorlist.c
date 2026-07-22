
/*------------------------------------------------------------------------------
 * @file    BSP_ERRORLIST.C
 * @author  FIRMWARE TEAM 
 * @date    2024/04/30 15:56:40
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
#include "includes.h"
#define LOG_TAG            "fun.error"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>
LIST(error); //错误队列
struct rt_mempool error_mem;
uint8_t error_poll[12*ERROR_CODE_MAX_NUM];
/*----------------------------------------------------------------------------*/
struct {
    struct rt_mutex         _mut;   //写错误锁
    uint8_t size;           //总错误量
    uint8_t highlevel;      //最高优先级错误
}Error_Info;

// 错误队列结构体
typedef struct _Error_t{
    struct _Error_t *next;      //指向下一个错误
    uint16_t    code; 
    uint8_t     level;          //错误等级
}Error_t;

int bsp_error_list_init()
{
	rt_err_t err;

    /* 初始化内存池对象 */
    err = rt_mp_init(&error_mem, "error_mp", &error_poll[0], sizeof(error_poll), 12);
    if(err != RT_EOK)  
    {
        LOG_E("Init error mp faild");
    } 
    rt_mutex_init(&Error_Info._mut,"error_Mut",RT_IPC_FLAG_PRIO);
    Error_Info.size = 0;    //错误总量为0
    Error_Info.highlevel = ERROR_LEVEL_NORMAL;  
    list_init(error);
    return RT_EOK;
}
INIT_DEVICE_EXPORT(bsp_error_list_init);

static uint16_t _get_currentModule_baseError(uint8_t _moduleID)
{
    uint16_t _basecode; 
    switch (_moduleID)
    {
        case MONITOR_MODULE_ID:
            _basecode =  0x2000;
            break;
        case LIQUID_MODULE_ID:
            _basecode = 0x5000;
            break;
        case PHOTOELECTRIC_MODE_ID:
            _basecode = 0x9000;
            break;
        case AIR_MODE_ID:
            _basecode = 0x4000;
            break;   
        case LASER_MODULE_ID:
            _basecode = 0x3000;
            break;       
        case PEDAL_MODULE_ID:
            _basecode = 0x8000;
            break;    
        case PHACO_MODE_ID:
            _basecode = 0x6000;
            break; 
        default:
            _basecode =  0x1000;
            break;
    }
	return _basecode;
}
/**
 * @description: 查找错误代码对应的错误等级
 * @param {uint16_t} code
 * @return {*}
 */
static uint8_t _errorCode_analyse_level(uint16_t code)
{
    //判断当前故障码是否符合
    uint16_t _basecode = _get_currentModule_baseError(CURRENT_MODULE_ID);
    if(code >= _basecode)   //故障码符合要求
    {
    
        uint8_t _level = (code>>8) & 0X0F;
        uint8_t _covLel;
        if(_level >=(ERROR_LEVEL_3)*2+1)
            _covLel = ERROR_LEVEL_4;
        else if(_level >=(ERROR_LEVEL_2)*2+1)
            _covLel = ERROR_LEVEL_3;
        else if(_level >=(ERROR_LEVEL_1)*2+1)
            _covLel = ERROR_LEVEL_2;
        else
            _covLel = ERROR_LEVEL_1;
        return _covLel;
    }
    else
    {
        LOG_E("ERROR CODE IS NOT CURRENT MODULE");
        return ERROR_LEVEL_NORMAL;   
    }

}
/**
 * @description: 判断错误链表内是否有该错误了
 * @param {uint16_t} code
 * @return {uint8_t} 0:没有  1:有
 */
static uint8_t _errorList_has_code(uint16_t code)
{
    uint16_t _len = list_length(error);
    Error_t *_item = list_head(error);
    for(int i=0;i<_len;i++)
    {
        if(_item->code == code)
            return 1;
        _item = list_item_next(_item);
    }
    return 0;			
}
static void _list_sort_level_add(Error_t *_node)
{
    static Error_t *index = NULL;
    if(list_head(error) == NULL)    //第一个节点
    {
        list_add(error,_node);
        Error_t *_head = list_head(error);
        index = _head;
        Error_Info.size++;
        Error_Info.highlevel = index->level;
        return;
    }
    else
    {
        if(_node->level >=index->level)
        {
            list_push(error,_node);
        }
        else
        {
            Error_t * l = list_head(error);
            for(int i=0;i<list_length(error);i++)
            {

                if(_node->level < l->level && (_node->level >= l->next->level || l->next == NULL))
                {
                    list_insert(error,l,_node);
                }
                l = list_item_next(l);
            }
        }
        index = list_head(error);
        Error_Info.size++;
        Error_Info.highlevel = index->level;
    }

}
/**
 * @description: 向列表中添加错误
 * @param {uint32_t} code 错误代码
 * @return {*}
 */
void bsp_error_list_add(uint32_t code)
{
    rt_mutex_take(&Error_Info._mut,RT_WAITING_FOREVER); 
    //1.判断当前链表内有无该错误代码
    if(_errorList_has_code(code))   //
    {
        LOG_D("error list has this code");
        rt_mutex_release(&Error_Info._mut);
        return;
    }
	Error_t * _l = rt_mp_alloc(&error_mem,RT_WAITING_FOREVER);
	if(_l == RT_NULL)
	{
		LOG_E("rt_mp_alloc error");
        rt_mutex_release(&Error_Info._mut);
		return;
	}
    _l->code = code;
    _l->level = _errorCode_analyse_level(code); 
    //这里进行错误等级排序
    _list_sort_level_add(_l);
     rt_mutex_release(&Error_Info._mut);
}

uint16_t bsp_error_list_pop()
{
    rt_mutex_take(&Error_Info._mut,RT_WAITING_FOREVER); 
    Error_t *_head = list_pop(error);
    rt_mutex_release(&Error_Info._mut);
    if(_head == NULL)
        return ERROR_CODE_NORAML;  //空列表
    Error_Info.size--;
    Error_Info.highlevel = ((Error_t*) (list_head(error)))->level;
    rt_mp_free(_head);
    return _head->code;
}

/**
 * @description: 获取错误数据包 
 * 高8位代表当前错误数量、低8位代表错误等级
 * @return {*}
 */
uint16_t bsp_error_list_getpack()
{
    return ((((Error_Info.size)<< 8) & 0xff00) |  (Error_Info.highlevel & 0xff));
}

static void errList(int argc, char**argv)
{
    if (argc < 2)
    {
        rt_kprintf("Please  errList <add code| get | print>'\n");
        return;
    }

    if (!rt_strcmp(argv[1], "add"))
    {
        uint16_t _code = atoi(argv[2]);
        rt_kprintf("add error code : %x\n",_code);  
        bsp_error_list_add(_code);
    }
    else if (!rt_strcmp(argv[1], "get"))
    {
        rt_kprintf("get error code : %x\n",bsp_error_list_pop());
		rt_kprintf("get error heart code:%x\n",bsp_error_list_getpack());
    }
    else if (!rt_strcmp(argv[1], "print"))
    {
        uint16_t _len = list_length(error);
        Error_t *_item = list_head(error);
        for(int i=0;i<_len;i++)
        {
            rt_kprintf("code:%x",_item->code);
            rt_kprintf("level:%d",_item->level);
			rt_kprintf("\n");
            _item = list_item_next(_item);
        } 
    }
    else if(!rt_strcmp(argv[1], "test"))
    {
        bsp_error_list_add(0x3101);
        bsp_error_list_add(0x3211);
        bsp_error_list_add(0x3221);
        bsp_error_list_add(0x3321);
        bsp_error_list_add(0x3222);
        bsp_error_list_add(0x3421);
        bsp_error_list_add(0x3521);
        bsp_error_list_add(0x3522);
        bsp_error_list_add(0x3621);
        bsp_error_list_add(0x3388);
        bsp_error_list_add(0x3811);
        bsp_error_list_add(0x3761);
        rt_kprintf("test\r\n");
    }
    else
    {
        rt_kprintf("Please  errList <add code| get | print>'\n");
    }
}

MSH_CMD_EXPORT(errList, errList sample: errList <add code| get>);

