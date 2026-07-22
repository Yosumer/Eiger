#include <rtthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/select.h>
#include <string.h>
#include <finsh.h>
#include <fcntl.h>
#define DBG_TAG "tcp_client"
#define DBG_LVL DBG_INFO
#include <ulog.h>
typedef struct
{
    uint8_t data[8];
    uint32_t len;
}S_MSG, *P_MSG;
static rt_mq_t mq = RT_NULL;
static uint8_t client_connect_state = 0;
static void tcp_client_thread_entry(void* parameter)
{
    int sockfd;
    struct sockaddr_in server_addr;
    S_MSG msg;
    SOCKET_INIT:
    client_connect_state = 0;   
    rt_thread_delay(100);       //重连等待100ms
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if( sockfd < 0 )
    {
        LOG_E("create socket failed.");
        return;
    }
    rt_memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("192.168.176.11");
    server_addr.sin_port = htons(1060);
    if( connect(sockfd, (struct sockaddr*)(&server_addr), sizeof(server_addr)) < 0 )
    {
        LOG_E("connect failed");
        closesocket(sockfd);
        goto SOCKET_INIT;
    }
    client_connect_state = 1;   //连接成功
    while(1)
    {
        if( rt_mq_recv(mq, &msg, sizeof(msg), RT_WAITING_FOREVER) != RT_EOK )
        {
            continue;
        }
        if( send(sockfd, msg.data, msg.len, 0) <= 0 )
        {
            closesocket(sockfd);
            goto SOCKET_INIT;
        }
    }    
}
int tcp_client_no_block_send(const uint8_t *data, uint32_t len)
{
    RT_ASSERT(mq!=RT_NULL);
    RT_ASSERT(data!=RT_NULL);
    if( client_connect_state == 1)
    {
        S_MSG msg;    
        if( len > sizeof(msg.data) )
        {
            LOG_E("send len overflow");
            return -1;
        }
        msg.len = len;
        rt_memcpy(msg.data, data, len);
        if( rt_mq_send(mq, &msg, sizeof(msg)) == -RT_EFULL )
        {
            LOG_E("tcp client msg overflow!");
        }
        return 0;
    }
    return 0;
}
static int tcp_client_init(void)
{
    mq = rt_mq_create("tcp_mq", sizeof(S_MSG), 125, RT_IPC_FLAG_FIFO);
    if (mq == RT_NULL) 
    {
        LOG_E("create message queue failed");
        return -1;
    }
    rt_thread_t tcp_client_thread = rt_thread_create("tcp_client", tcp_client_thread_entry, RT_NULL, 2048, 20, 10);
    if (tcp_client_thread == RT_NULL) 
    {
        LOG_E("create thread failed");
        return -1;
    }
    rt_thread_startup(tcp_client_thread);
    return 0;
}
 INIT_ENV_EXPORT(tcp_client_init);
