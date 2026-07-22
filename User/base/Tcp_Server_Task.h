#ifndef TCP_SERVER_TASK_H
#define TCP_SERVER_TASK_H



#include <stdint.h>  // 引入stdint.h头文件，包含有有符号整型的定义
#include <sys/socket.h> /* 使用BSD socket，需要包含socket.h头文件 */
#include <netdb.h>

#define TCP_SERVER_DEFAULT_PORT  25090
#define TCP_SERVER_MAX_LISTEN    5 

#define TCP_SERVER_DEFAULT_CONFIG       \
    {                            \
        .max_lisent = TCP_SERVER_MAX_LISTEN,        \
        .server_port = TCP_SERVER_DEFAULT_PORT, \
    }                                   



enum client_State
{
     CLIENT_INIT = 0,
     CLIENT_CONNECTED,
     CLIENT_DISCONNECT,
};

typedef void (*client_rx)(uint8_t *buf,uint16_t len);
typedef void (*server_new_connection)(uint32_t _ip,uint32_t _port);

typedef struct _tcp_client_handler{
	struct _tcp_client_handler *next;			//指向下一个链表
    enum client_State client_state;   //  0:Init 1:connected 2:disconnected
    uint16_t  client_port; // 客户端连接端口
    uint32_t  client_ip;   // 客户端IP地址
    int client_socket;	    //客户端通信socket
    void (*client_rx_handler)(uint8_t *buf,uint16_t len); // 接收处理函数
    void (*disconnect)(struct _tcp_client_handler *c);   //断连回调函数               
}tcp_client_handler;



typedef struct  _tcp_server_config {
    uint8_t max_lisent;	    //最大监听数量
    uint16_t server_port;   //服务器端口
}tcp_server_config;

typedef struct _tcp_server_gate_t{

    int net_connect_state;	        // 网卡连接状态
    int server_fd;                  // 服务器套接字
    int pipe_read_fd;               // 监控通信管道 
    int pipe_write_fd;              // 写管道
    struct sockaddr_in server_addr;         //服务器IP地址
    tcp_server_config   *config;    // 服务器配置参数
    server_new_connection   new_connect_Handler;
    void ** client_list;            // 服务器链表
    client_rx   default_rx;         // 服务器默认连接的客户端连接函数  
    uint8_t active_clients;         //以及连接的客户端
    uint8_t * fifo_rx_buf;  
    uint8_t * socket_rx_buf;
    char  pipe_name[8];
}tcp_server_gate_t;

extern tcp_server_gate_t *tcp_server;

tcp_server_gate_t * tcp_server_task_crete(tcp_server_config *_config,server_new_connection new_callback);
int8_t tcp_server_task_stop(tcp_server_gate_t *s);
int8_t tcp_server_task_register_client_rx(tcp_server_gate_t *_s,client_rx _rx);
int32_t tcp_server_task_send(tcp_server_gate_t *_s,uint8_t *_buf,uint16_t len);

int tcp_server_default_Send(uint8_t* buf,uint8_t _size);
#endif

