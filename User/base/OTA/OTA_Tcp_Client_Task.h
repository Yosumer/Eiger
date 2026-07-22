// 以下是一个基本的 C++ 头文件示例，包含头文件保护，可避免重复包含
#ifndef OTA_TCP_CLIENT_TASK_H
#define OTA_TCP_CLIENT_TASK_H

#include <rtthread.h>

typedef void (*rx_cb_t)(void *buff, rt_size_t len);
typedef struct rt_tcpclient rt_tcpclient_t;

rt_tcpclient_t *rt_tcpclient_start(const char *hostname, rt_uint32_t port);
void rt_tcpclient_close(rt_tcpclient_t *thiz);
rt_int32_t rt_tcpclient_attach_rx_cb(rt_tcpclient_t *thiz, rx_cb_t cb);
rt_int32_t rt_tcpclient_send(rt_tcpclient_t *thiz, const void *buff, rt_size_t len);


#endif // OTA_TCP_CLIENT_TASK_H
