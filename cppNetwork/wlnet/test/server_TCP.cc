
#include <stdlib.h>
#include <stdio.h>

#include "wlnet.h"

typedef struct port_data_s
{
    int port_length;

    int buff[];
}port_data_t;

int wl_run_reactor(wl_reactor_t* reactor)
{

    struct epoll_event events[EVENTS_LENGTH] = {0};

    while (1)
    {
        int nready = epoll_wait(reactor->epfd, events, EVENTS_LENGTH, 5);

        for(int i = 0; i < nready; ++i)
        {
            int connfd = events[i].data.fd;
            wl_connect_t* conn = wl_connect_idx(reactor, connfd);

            if(events[i].events & EPOLLIN)
            {
                conn->cb(connfd, events[i].events, reactor);
            }

            if(events[i].events & EPOLLOUT)
            {
                conn->cb(connfd, events[i].events, reactor);
            }
        }
    }
}


int main(int argc, char* argv[])
{
    if(argc < 2)
        return -1;
    
    int port = atoi(argv[1]);
    int port_length = atoi(argv[2]);

    wl_reactor_t reactor;
    wl_init_reactor(&reactor);

    port_data_t* portData = (port_data_t*)malloc(sizeof(port_data_t) + sizeof(int)*port_length);
    portData->port_length = port_length;
    for (int i = 0; i < port_length; i++)
    {
        int sockfd = init_server(port + i);
        // 设置为非阻塞模式
        int flags = fcntl(sockfd, F_GETFL, 0);
        fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
        portData->buff[i] = sockfd;
        set_listener(&reactor, sockfd, accept_cb);
    }
    
    wl_run_reactor(&reactor);

    wl_dest_reactor(&reactor);

    for (int i = 0; i < port_length; i++)
    {
        close(portData->buff[i]);
    }
    free(portData);

    return 0;
}