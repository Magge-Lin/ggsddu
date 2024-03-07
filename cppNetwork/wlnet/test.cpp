
#include "wlnet.h"


int main(int argc, char* argv[])
{
    if(argc < 2)
        return -1;
    
    int port = atoi(argv[1]);

    int sockfd = init_server(port);
    if(sockfd <= 0)
    {
        errlog<<"init_server is err."<<std::endl;
        return -1;
    }

    wl_reactor_t reactor;
    wl_init_reactor(&reactor);

    if(set_listener(&reactor, sockfd, accept_cb) <= 0)
    {
        errlog<<"set_listener is err."<<std::endl;
        return -1;
    }

    struct epoll_event events[EVENTS_LENGTH] = {0};

    while (1)
    {
        int nready = epoll_wait(reactor.epfd, events, EVENTS_LENGTH, -1);

        for(int i = 0; i < nready; ++i)
        {
            int connfd = events[i].data.fd;
            wl_connect_t* conn = wl_connect_idx(&reactor, connfd);

            if(events[i].events & EPOLLIN)
            {
                conn->cb(connfd, events[i].events, &reactor);
            }

            if(events[i].events & EPOLLOUT)
            {
                conn->cb(connfd, events[i].events, &reactor);
            }
        }
    }

    close(sockfd);
    wl_dest_reactor(&reactor);

    return 0;
}