#include "wlnet.h"

#include <thread>
#include <chrono>


int wl_init_reactor(wl_reactor_t* reactor)
{
    if(!reactor)
    {
        errlog<<"reactor is NULL !"<<std::endl;
        return -1;
    }

    reactor->blockheader = (wl_connblock_t*)malloc(sizeof(wl_connblock_t) + EVENTS_LENGTH*sizeof(wl_connect_t));
    if(!reactor->blockheader)
    {
        errlog<<"reactor->blockheader is NULL."<<std::endl;
        return -1;
    }

    reactor->blockheader->block = (wl_connect_t*)(reactor->blockheader + 1);
    reactor->blockheader->next = NULL;
    reactor->blk = 1;
    reactor->epfd = epoll_create(1);

    return 1;
}

int wl_dest_reactor(wl_reactor_t* reactor)
{
    if(!reactor)
    {
        errlog<<"reactor is NULL."<<std::endl;
        return -1;
    }

    if(reactor->blockheader)
    {
        free(reactor->blockheader);
    }

    close(reactor->epfd);
    reactor = NULL;

    errlog<<"dest reactor"<<std::endl;

    return 1;
}

int init_server(int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd <= 0)
    {
        errlog<<"create sockfd is err."<<std::endl;
        return sockfd;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if(bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        errlog<<" bind is err, sockfd:"<<sockfd<<std::endl;
        close(sockfd);

        return -1;
    }

    if(listen(sockfd, 20) < 0)
    {
        errlog<<"listen is err, sockfd:"<<sockfd<<std::endl;
        close(sockfd);

        return -1;
    }

    errlog<<"listen port:"<<port<<std::endl;

    return sockfd;

}

int set_listener(wl_reactor_t* reactor, int fd, ZVCALLBACK cb)
{
    if(!reactor || !reactor->blockheader)
    {
        errlog<<"reactor is NULL or reactor->blockheader is NULL."<<std::endl;

        return -1;
    }

    reactor->blockheader->block[fd].fd = fd;
    reactor->blockheader->block[fd].cb = cb;

    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;

    epoll_ctl(reactor->epfd, EPOLL_CTL_ADD, fd, &ev);

    return 1;
    
}

int wl_connect_block(wl_reactor_t* reactor)
{
    if(!reactor)
    {
        errlog<<"reactor is NULL."<<std::endl;
        return -1;
    }

    wl_connblock_t* blk = reactor->blockheader;
    while (blk->next != NULL)
    {
        blk = blk->next;
    }

    wl_connblock_t* connblock = (wl_connblock_t*)malloc(sizeof(wl_connblock_t) + EVENTS_LENGTH*sizeof(wl_connect_t));
    if(!connblock)
    {
        errlog<<"malloc is err."<<std::endl;
        return -1;
    }

    connblock->block = (wl_connect_t*)(connblock + 1);
    connblock->next = NULL;

    blk->next = connblock;
    reactor->blk++;
    
    return 1;

}

wl_connect_t* wl_connect_idx(wl_reactor_t* reactor, int fd)
{
    pthread_mutex_lock(&m_mutex);  // 加锁
    if(!reactor || !reactor->blockheader)
    {
        errlog<<"reactor is NULL or reactor->blockheader is NULL."<<std::endl;
        return NULL;
    }

    /**
     *      1、首先需要理解fd相关信息要存储在wl_connect_t数据结构中（hash表存储的）
     *      2、需要计算出当前fd对应在第几个wl_connblock_t数据结构中（链表存储的）
     *      3、每个wl_connblock_t有EVENTS_LENGTH个wl_connect_t数据结构（malloc的时候创建的）
    */
    int blockIdx = fd/EVENTS_LENGTH;
    while (blockIdx >= reactor->blk)
    {
        //如果当前fd所在的wl_connblock_t还没malloc出来，那就去malloc
        if(wl_connect_block(reactor) < 0)
        {
            errlog<<"wl_connect_block is err."<<std::endl;
            return NULL;
        }
    }

    int count = 0;
    wl_connblock_t* connblock = reactor->blockheader;
    while (count++ < blockIdx)
    {
        connblock = connblock->next;
    }
    pthread_mutex_unlock(&m_mutex);  // 加锁

    return &connblock->block[fd%EVENTS_LENGTH];
    
}

int accept_cb(int fd, int events, void* arg)
{
    if(!arg)
    {
        errlog<<"arg is NULL."<<std::endl;
        return -1;
    }

    struct sockaddr_in clientAddr;
    socklen_t clientlen = sizeof(clientAddr);

    int clientfd = accept(fd, (struct sockaddr*)&clientAddr, &clientlen);
    if (clientfd <= 0)
    {
        errlog<<"accept is err. sockfd  is : "<<fd<<std::endl;
        return -1;
    }
    
    // errlog<<"accept sockfd:"<<fd<<"     clientfd:"<<clientfd<<std::endl;

    wl_reactor_t* reactor = (wl_reactor_t*)arg;
    wl_connect_t* conn = wl_connect_idx(reactor, clientfd);

    conn->fd = clientfd;
    conn->count = BUFFER_LENGTH;
    conn->cb = recv_cb;
    
    struct epoll_event ev;
    ev.data.fd = clientfd;
    ev.events = EPOLLIN | EPOLLET;

    epoll_ctl(reactor->epfd, EPOLL_CTL_ADD, clientfd, &ev);

    return 1;
}

int recv_cb(int fd, int events, void* arg)
{
    if(!arg)
    {
        errlog<<"arg is NULL."<<std::endl;
        return -1;
    }

    wl_reactor_t* reactor = (wl_reactor_t*)arg;
    wl_connect_t* conn = wl_connect_idx(reactor, fd);
    int len = recv(fd, conn->rbuffer, conn->count, 0);
    if(len < 0)
    {
        errlog<<"recv is err. sockfd  is : "<<fd<<std::endl;
        return -1;
    }
    else if(len == 0)
    {
        errlog<<"close clientfd:"<<fd<<std::endl;

        close(conn->fd);
        conn->fd = -1;
        conn->rc = 0;
        conn->wc = 0;
        epoll_ctl(fd, EPOLL_CTL_DEL, fd, NULL);

        close(fd);
                
        return 1;
    }

    // errlog<<"recv:"<<conn->rbuffer<<std::endl;

    conn->rc = len;
    conn->cb = send_cb;

    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLOUT | EPOLLET;
    epoll_ctl(reactor->epfd, EPOLL_CTL_MOD, fd, &ev);
    
    return 1;

}

int send_cb(int fd, int events, void* arg)
{
    if(!arg)
    {
        errlog<<"arg is NULL."<<std::endl;
        return -1;
    }
    
    wl_reactor_t* reactor = (wl_reactor_t*)arg;
    wl_connect_t* conn = wl_connect_idx(reactor, fd);

    const char str[] = "hello this is server.";

    int len = send(fd, str, strlen(str), 0);
    if(len <= 0)
    {
        errlog<<"send is err. sockfd  is : "<<fd<<std::endl;
        return -1;
    }

    // errlog<<"send :"<<str<<std::endl;

    conn->cb = recv_cb;
    
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;

    epoll_ctl(reactor->epfd, EPOLL_CTL_MOD, fd, &ev);
    
    return -1;

}
