#include <rtthread.h>
#include <sys/socket.h> /* 使用BSD socket，需要包含socket.h头文件 */
#include <sys/select.h> 
/* 添加文件系统头文件 */
#include <dfs_fs.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdev.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include "Tcp_Server_Task.h"
#include "includes.h"
#define LOG_TAG            "task.TcpServer"
#define LOG_LVL            LOG_LVL_DBG
#include <ulog.h>

#define PIPE_BUFSZ    512
#define MAX_VALUE(A,B) (A>B?A:B)



struct netdev * _netdev = RT_NULL;
tcp_server_gate_t *tcp_server;
struct rt_mempool tcp_client_mem;
uint8_t tcp_client_poll[(TCP_SERVER_MAX_LISTEN+1)*sizeof(struct  _tcp_client_handler)];

static rt_thread_t tcp_server_tid = RT_NULL;
static struct  _tcp_server_config _default_config = TCP_SERVER_DEFAULT_CONFIG;
static void client_default_disconnect_call(struct _tcp_client_handler *c)
{

    c->client_state = CLIENT_DISCONNECT;
    c->client_ip = 0;
    c->client_port = 0;
    int res = closesocket(c->client_socket);   //关闭客户端socket
    RT_ASSERT(res == 0);
    c->client_socket = -1;
}
static tcp_client_handler * tcp_server_create_client(tcp_server_gate_t *s)
{
    tcp_client_handler  *client= rt_mp_alloc(&tcp_client_mem,RT_WAITING_FOREVER);
    if(client == RT_NULL)
    {
        LOG_E("tcp_client_handler alloc failed\n");
        return RT_NULL;
    }
    client->client_state = CLIENT_INIT;
    client->client_socket = -1;
    client->client_rx_handler = s->default_rx;
    client->disconnect = client_default_disconnect_call;
    list_add(s->client_list,client);
    return client;
} 

static void tcp_server_removeAll_client(tcp_server_gate_t *s)
{
    tcp_client_handler  *client;
    do
    {
        client = (tcp_client_handler *)list_pop(s->client_list);
        rt_mp_free(client);
    } while (client != NULL );  //回收所有客户端内存
}


static rt_int32_t pipe_init(tcp_server_gate_t *thiz)
{
    char dev_name[32];
    static int pipeno = 0;
    rt_pipe_t *pipe = RT_NULL;

    if (thiz == RT_NULL)
    {
        LOG_E("pipe init param is NULL\n");
        return -1;
    }

    snprintf(thiz->pipe_name, sizeof(thiz->pipe_name), "pipe%d", pipeno++);

    pipe = rt_pipe_create(thiz->pipe_name, PIPE_BUFSZ);
    if (pipe == RT_NULL)
    {
        LOG_E("pipe create failed\n");
        return -1;
    }

    snprintf(dev_name, sizeof(dev_name), "/dev/%s", thiz->pipe_name);
		
    thiz->pipe_read_fd = open(dev_name, O_RDONLY, 0);
    if (thiz->pipe_read_fd < 0)
        goto fail_read;

    thiz->pipe_write_fd = open(dev_name, O_WRONLY, 0);
    if (thiz->pipe_write_fd < 0)
        goto fail_write;

    LOG_I("pipe init", "pipe init succeed\n");
    return 0;

fail_write:
    close(thiz->pipe_read_fd);
fail_read:
    return -1;
}

static rt_int32_t pipe_deinit(tcp_server_gate_t *thiz)
{
    int res = 0;

    if (thiz == RT_NULL)
    {
        LOG_E("pipe deinit param is NULL, pipe deinit failed\n");
        return -1;
    }

    res = close(thiz->pipe_read_fd);
    RT_ASSERT(res == 0);
    thiz->pipe_read_fd = -1;

    res = close(thiz->pipe_write_fd);
    RT_ASSERT(res == 0);
    thiz->pipe_write_fd = -1;

    rt_pipe_delete(thiz->pipe_name);

    LOG_I("pipe deinit", "pipe close succeed\n");
    return 0;
}

static rt_int32_t socket_deinit(tcp_server_gate_t *thiz)
{
    int res = 0;
    if (thiz == RT_NULL)
    {
        LOG_E("socket deinit param is NULL, socket deinit failed\n");
        return -1;
    }
    tcp_server_removeAll_client(thiz);   //清空所有客户端连接内存
    res = closesocket(thiz->server_fd);  // 关闭客户端的SOCKET
    RT_ASSERT(res == 0);
    thiz->server_fd = -1;
    LOG_E("socket deinit socket close succeed\n");
    return 0;
}

extern struct rt_messagequeue downLD_mq;       //下行数据流
static void client_default_rx_call(uint8_t *buf,uint16_t len)
{
    uint16_t _packet_cnt = len/8;
    uint8_t  _packflag = len % 8;   //理论上包的数为8的正数倍 
    if(_packflag != 0) {
        LOG_E("TCP Recv packet lost!!!");
    }
    for(uint16_t i = 0; i < _packet_cnt; i++) {
        //TODO: 处理接收到的数据    
        int result;
        /* 发送消息到消息队列中 */
        result = rt_mq_send(&downLD_mq, &buf[i*8], 8);
        if (result != RT_EOK)
        {
            LOG_D("rt_mq_send ERR\n");
        }
				rt_thread_delay(1);
    }
}

static void server_default_new_connection(uint32_t _ip,uint32_t _port)
{

}


tcp_server_gate_t * tcp_server_task_crete(tcp_server_config *_config,server_new_connection new_callback)
{

    int maxfd;
    tcp_server_gate_t  *server =  rt_calloc(1, sizeof(tcp_server_gate_t));
    if (server == RT_NULL)
    {
        LOG_E("tcp_server_gate_t alloc failed\n");
        return RT_NULL;
    }
    if(_config == RT_NULL)
        server->config = &_default_config;
    else
        server->config = _config;
    if(new_callback == RT_NULL)
        server->new_connect_Handler = server_default_new_connection;
    else
        server->new_connect_Handler = new_callback;
    server->active_clients = 0;
    server->pipe_read_fd = -1;
    server->pipe_write_fd = -1;
    server->server_fd = -1;


    /* 初始化内存池对象 */
    rt_err_t err;
    err = rt_mp_init(&tcp_client_mem, "tcp_s_mq", &tcp_client_poll[0], sizeof(tcp_client_poll), sizeof(struct  _tcp_client_handler));
    if(err != RT_EOK)  
    {
        LOG_E("Init error mp faild");
    } 
    server->client_list=rt_calloc(1,4);
    list_init(server->client_list); //初始化链表


    //初始化PIPE
    if(pipe_init(server)!= 0)
    {
        LOG_E("pipe init failed\n");
        rt_free(server);    //释放内存
        return RT_NULL;
    }

    //创建服务器socket
    if((server->server_fd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        LOG_E("create socket failed\n");
        rt_free(server);    //释放内存
        return RT_NULL;
    }
    //配置服务器参数
    server->server_addr.sin_family = AF_INET;
    server->server_addr.sin_port = htons(server->config->server_port);
    server->server_addr.sin_addr.s_addr = INADDR_ANY;
    rt_memset(&server->server_addr.sin_zero, 0, sizeof(server->server_addr.sin_zero));
    //绑定IP和端口
    if (bind(server->server_fd,(struct sockaddr *)&server->server_addr,sizeof(struct sockaddr)) < 0)
    {
        LOG_E("bind socket failed\n");
        closesocket(server->server_fd);
        return RT_NULL;
    }
    LOG_D("server created and bind sucessful\n");
    return server;
}
int8_t tcp_server_task_stop(tcp_server_gate_t *s)
{
    if (s == RT_NULL)
    {
        LOG_E("tcp serve deinit param is NULL,  tcp serve deinit failed\n");
        return -1;
    }
    if (tcp_server_tid != RT_NULL)
    {
        rt_thread_delete(tcp_server_tid);
        tcp_server_tid = RT_NULL;
    }
    if (pipe_deinit(s) != 0)
        return -1;

    if (socket_deinit(s) != 0)
        return -1;

    rt_free(s);    //释放内存

    rt_mp_detach(&tcp_client_mem); //
    

    LOG_I("tcp serve deinit succeed\n");
    return RT_EOK;
}
int8_t tcp_server_task_register_client_rx(tcp_server_gate_t *_s,client_rx _rx)
{

    if(_s == RT_NULL)
    {
        LOG_E("register_client_r Faild! param is NULL\n");
        return -1;
    }
    if(_rx == RT_NULL)  //使用默认 
    {
        _s->default_rx = client_default_rx_call;
    }
    else {
        _s->default_rx = _rx;
    }

    return RT_EOK;
}
int32_t tcp_server_task_send(tcp_server_gate_t *_s,uint8_t *_buf,uint16_t len)
{
    rt_size_t bytes = 0;
    if(_s == RT_NULL)
    {
        LOG_E("TCP Server Send Faild! param is NULL\n");
        return -1;
    }
    if (_buf == RT_NULL)
    {
        LOG_E("TCP Server send buff is NULL\n");
        return -1;
    }

    bytes = write(_s->pipe_write_fd, _buf, len);
    return bytes;
}

int tcp_server_default_Send(uint8_t* buf,uint8_t _size)
{
    return tcp_server_task_send(tcp_server, buf, _size);
}

static void tcp_server_task_entry(void *param)
{
    tcp_server_gate_t *_s = (tcp_server_gate_t *)param;

    //动态分配buf
    _s->fifo_rx_buf = rt_calloc(PIPE_BUFSZ, 1);
    if(_s->fifo_rx_buf == RT_NULL)
    {
        LOG_E("tcp_server_task_entry malloc FIFO buffer Faild\n");
        return;
    }
    _s->socket_rx_buf = rt_calloc(1024, 1);
    if(_s->socket_rx_buf == RT_NULL)
    {
        LOG_E("tcp_server_task_entry malloc TCP RX buffer Faild\n");
        return;
    }

    int max_fd , res=0;     //最大套接字
    fd_set all_set, read_set;
    socklen_t client_addr_len;
    struct sockaddr_in client_addr;
    //监听socket
    if(listen(_s->server_fd,_s->config->max_lisent) == -1)
    {
        LOG_E("listen socket failed\n");
        return;
    }
    else
    {
        LOG_I("listening...\n");
    }

    client_addr_len = sizeof(struct sockaddr_in);
    FD_ZERO(&all_set);
    FD_ZERO(&read_set);
    max_fd =  MAX_VALUE(_s->server_fd,_s->pipe_read_fd) +1;
    FD_SET(_s->server_fd, &all_set);
    FD_SET(_s->pipe_read_fd, &all_set);
    while (1)
    {
        read_set = all_set;
        /* 等待设定的网络描述符有事件发生 */
        res = select(max_fd, &read_set, RT_NULL, RT_NULL, RT_NULL);
        if(res < 0)
        {
            LOG_E("select error\n");
            continue;
        }
        //检测网线是否插拔




        //查看socket 描述符上是否有可读事件
        if (FD_ISSET(_s->server_fd,&read_set))
        {
            // 有新的客户端连接进入
            int client_fd = accept(_s->server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
            if(client_fd < 0)
            {
                LOG_E("accept error\n");
                continue;
            }
            LOG_D("new client connected fd=%d\n",client_fd);
            

            if(_s->active_clients < TCP_SERVER_MAX_LISTEN)
            {
                // 创建一个client
                int client_len = list_length(_s->client_list);
                tcp_client_handler *client = RT_NULL;
                if(client_len < TCP_SERVER_MAX_LISTEN)  //创建新的客户端
                {
                     client =  tcp_server_create_client(_s);
                }
                else       //从客户端池中寻找空闲的连接
                {
                    client = list_head(_s->client_list);
                    do
                    {
                        client = (tcp_client_handler *)list_item_next(client);

                    } while (client !=RT_NULL && client->client_socket != -1);
                }
                if(client == RT_NULL)
                {
                    LOG_E("create client failed\n"); 
                    continue;
                }
                client->client_state = CLIENT_CONNECTED;
                client->client_socket = client_fd;
                
                struct sockaddr_in peerAddr;
                socklen_t peerLen;
                char ipAddr[16];//保存点分十进制的地址
                getpeername(client_fd,(struct sockaddr *)&peerAddr,&peerLen);
                client->client_ip = peerAddr.sin_addr.s_addr;      //获取对端地址
                client->client_port = peerAddr.sin_port;                //获取对端接口

                LOG_E("connected peer address = %s:%d",inet_ntop(AF_INET, &peerAddr.sin_addr, ipAddr, sizeof(ipAddr)), ntohs(peerAddr.sin_port));

                //添加新的cfd 到 fd_Set集合中
                FD_SET(client_fd, &all_set);
                max_fd =  MAX_VALUE(max_fd-1,client_fd) +1;
                _s->active_clients++;
            }
            else
            {
                LOG_E("Active Clients is max \n");          //服务器连接达到最大
            }
        
            //没有其他套接字需要处理：这里防止重复工作，就不去执行其他任务
            if(--res)
            {
                //继续select
                continue;
            }
        }
        tcp_client_handler  *client = list_head(_s->client_list);
        uint8_t client_len = list_length(_s->client_list);
        //连接所有的客户端文件描述符
        for(uint8_t i=0;i<client_len;i++)
        {
            //判断是否在fd_set集合里面
            if(client->client_state==CLIENT_CONNECTED && FD_ISSET(client->client_socket,&read_set))
            {
                int nbytes = recv(client->client_socket,_s->socket_rx_buf,1024,0);
                if(nbytes <= 0) //socket error or disconnected
                {
                    //从集合中清除
                    FD_CLR(client->client_socket, &all_set);
                    LOG_E("client %d disconnected\n",client->client_socket);
                    client->disconnect(client);
                    _s->active_clients--;
                }
                else
                {
                    client->client_rx_handler(_s->socket_rx_buf,nbytes);
                }
            }
            client = list_item_next(client);
        }
        // 查看 PIPE 描述符上有没有可读事件
        if(FD_ISSET(_s->pipe_read_fd,&read_set))
        {

            if (_s->net_connect_state == -1)
            {
                tcp_client_handler  *client = list_head(_s->client_list);
                uint8_t client_len = list_length(_s->client_list);

                for(uint8_t i=0;i<client_len;i++)
                {
                    if(client->client_state==CLIENT_CONNECTED && client !=RT_NULL)
                    {
                        FD_CLR(client->client_socket, &all_set);
                        LOG_E("client send msg faild maybe %d disconnect",client->client_socket);
                        client->disconnect(client);
                        _s->active_clients--;
                        client = list_item_next(client);
                    }
                    client = list_item_next(client);
                }
								_s->net_connect_state = 0;
                continue;
            }
            

            int nbytes = read(_s->pipe_read_fd,_s->fifo_rx_buf,PIPE_BUFSZ);
            if(nbytes > 0)
            {
                //广播发送所有Socket
                tcp_client_handler  *client = list_head(_s->client_list);
                uint8_t client_len = list_length(_s->client_list);

                for(uint8_t i=0;i<client_len;i++)
                {
                    //判断是否在fd_set集合里面
                    if(client->client_state==CLIENT_CONNECTED && client !=RT_NULL)
                    {
                        int  n =  send(client->client_socket,_s->fifo_rx_buf,nbytes,0);
                        if(n < 0)   //发送失败
                        {
                            FD_CLR(client->client_socket, &all_set);
                            client->disconnect(client);
                            _s->active_clients--;
                            LOG_E("client send msg faild maybe %d disconnect",client->client_socket);
                        }
                    }
                    client = list_item_next(client);
                }
            }
            else
            {
                LOG_E("read from pipe failed\n");
            }
        }
    }   
    
}

int tcp_server_task_start();
static void __netdev_callback(struct netdev *netdev, enum netdev_cb_type type)
{
    LOG_E("TCP typd:%d\n",type);
    if (type == NETDEV_CB_STATUS_LINK_DOWN) //网线拔出清除所有已经连接的socket
    {
        tcp_server->net_connect_state = -1;
    }
    else if (type == NETDEV_CB_STATUS_LINK_UP)
    {
        tcp_server->net_connect_state = 0;
    }
    
}



int tcp_server_task_start()
{

    _netdev  = netdev_get_by_family(AF_INET);       //获取网卡名称
    if (_netdev == RT_NULL)
    {
        LOG_E("get netdev failed\n");
    }
    
    netdev_set_status_callback(_netdev,__netdev_callback);

    tcp_server =   tcp_server_task_crete(RT_NULL,RT_NULL);      //使用默认参数
    tcp_server_task_register_client_rx(tcp_server,RT_NULL);     //使用默认接受处理函数
    tcp_server_tid = rt_thread_create("tcp_server",tcp_server_task_entry,tcp_server,2048,10,10);
    if(tcp_server_tid == RT_NULL)
    {
        LOG_E("create tcp_server_task failed\n");
        return -1;
    }
    rt_thread_startup(tcp_server_tid);
}
INIT_APP_EXPORT(tcp_server_task_start); //自动初始化