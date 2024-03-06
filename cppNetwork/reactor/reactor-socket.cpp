#include<sys/socket.h>
#include<cstring>
#include<unistd.h>
#include<netinet/in.h>
#include<pthread.h>
#include<sys/fcntl.h>
#include<stdlib.h>
#include<sys/epoll.h>

#include<iostream>
using namespace std;

#define         BUFFER_LENGTH           1024
#define         EVENTS_LENGTH           1024
#define		    errlog		            cout << __LINE__ << " "

typedef int (*ZVCALLBACK)(int fd, int event, void* arg);

int accept_cb(int fd, int event, void* arg);
int recv_cb(int fd, int event, void* arg);
int send_cb(int fd, int event, void* arg);

typedef struct zv_connect_s
{
    int fd;
    int count;

    ZVCALLBACK cb;

    char rbuffer[BUFFER_LENGTH];
    int rc;

    char wbuffer[BUFFER_LENGTH];
    int wc;

}zv_connect_t;

typedef struct zv_connblock_s
{
    zv_connect_t* block;

    struct zv_connblock_s* next;
    
}zv_connblock_t;

typedef struct zv_reactor_s
{
    int epfd;
    int blk;

    zv_connblock_t* blockheader;
    
}zv_reactor_t;

int zv_init_reactor(zv_reactor_t* reactor)
{
    if(!reactor)
    {
        errlog<<"reactor is NULL !"<<std::endl;
        return -1;
    }

    reactor->blockheader = (zv_connblock_t*)malloc(sizeof(zv_connblock_t) + EVENTS_LENGTH*sizeof(zv_connect_t));
    if(!reactor->blockheader)
    {
        errlog<<"reactor->blockheader is NULL."<<std::endl;
        return -1;
    }

    reactor->blockheader->block = (zv_connect_t*)(reactor->blockheader + 1);
    reactor->blockheader->next = NULL;
    reactor->blk = 1;
    reactor->epfd = epoll_create(1);

    return 1;
}

int zv_dest_reactor(zv_reactor_t* reactor)
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

int set_listener(zv_reactor_t* reactor, int fd, ZVCALLBACK cb)
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
    ev.events = EPOLLIN;

    epoll_ctl(reactor->epfd, EPOLL_CTL_ADD, fd, &ev);

    return 1;
    
}

int zv_connect_block(zv_reactor_t* reactor)
{
    if(!reactor)
    {
        errlog<<"reactor is NULL."<<std::endl;
        return -1;
    }

    zv_connblock_t* blk = reactor->blockheader;
    while (blk->next)
    {
        blk = blk->next;
    }

    zv_connblock_t* block = (zv_connblock_t*)malloc(sizeof(zv_connblock_t) + EVENTS_LENGTH*sizeof(zv_connect_t));
    if(!block)
    {
        errlog<<"malloc is err."<<std::endl;
        return -1;
    }

    block->block = (zv_connect_t*)(block + 1);
    block->next = NULL;

    blk->next = block;
    reactor->blk++;
    
    return 1;

}

zv_connect_t* zv_connect_idx(zv_reactor_t* reactor, int fd)
{
    if(!reactor || !reactor->blockheader)
    {
        errlog<<"reactor is NULL or reactor->blockheader is NULL."<<std::endl;
        return NULL;
    }

    /**
     *      1、首先需要理解fd相关信息要存储在zv_connect_t数据结构中（hash表存储的）
     *      2、需要计算出当前fd对应在第几个zv_connblock_t数据结构中（链表存储的）
     *      3、每个zv_connblock_t有EVENTS_LENGTH个zv_connect_t数据结构（malloc的时候创建的）
    */
    int blockIdx = fd/EVENTS_LENGTH;
    while (blockIdx >= reactor->blk)
    {
        //如果当前fd所在的zv_connblock_t还没malloc出来，那就去malloc
        if(zv_connect_block(reactor) < 0)
        {
            errlog<<"zv_connect_block is err."<<std::endl;
            return NULL;
        }
    }

    int count = 0;
    zv_connblock_t* connblock = reactor->blockheader;
    while (count++ < blockIdx)
    {
        connblock = connblock->next;
    }

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
    if(clientfd <= 0)
    {
        errlog<<"accept is err, sockfd:"<<fd<<std::endl;
        return -1;
    }

    errlog<<"accept sockfd:"<<fd<<"     clientfd:"<<clientfd<<std::endl;

    zv_reactor_t* reactor = (zv_reactor_t*)arg;
    zv_connect_t* conn = zv_connect_idx(reactor, clientfd);
    conn->fd = clientfd;
    conn->count = BUFFER_LENGTH;
    conn->cb = recv_cb;
    
    struct epoll_event ev;
    ev.data.fd = clientfd;
    ev.events = EPOLLIN;

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
    
    zv_reactor_t* reactor = (zv_reactor_t*)arg;
    zv_connect_t* conn = zv_connect_idx(reactor, fd);
    int len = recv(fd, conn->rbuffer, conn->count, 0);
    if(len < 0)
    {
        errlog<<"recv is err."<<std::endl;
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

    errlog<<"recv:"<<conn->rbuffer<<std::endl;

    conn->rc = len;
    conn->cb = send_cb;

    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLOUT;
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

    zv_reactor_t* reactor = (zv_reactor_t*)arg;
    zv_connect_t* conn = zv_connect_idx(reactor, fd);

    int len = send(fd, conn->rbuffer, conn->rc, 0);
    if(len <= 0)
    {
        errlog<<"send is err."<<std::endl;
        return -1;
    }

    memset(conn->rbuffer, 0, conn->rc);

    conn->cb = recv_cb;
    
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;

    epoll_ctl(reactor->epfd, EPOLL_CTL_MOD, fd, &ev);

    return -1;

}

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

    zv_reactor_t reactor;
    zv_init_reactor(&reactor);

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
            zv_connect_t* conn = zv_connect_idx(&reactor, connfd);

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
    zv_dest_reactor(&reactor);

    return 0;
}