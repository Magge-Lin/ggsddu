#pragma once

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

int zv_init_reactor(zv_reactor_t* reactor);
int zv_dest_reactor(zv_reactor_t* reactor);
int zv_dest_reactor(zv_reactor_t* reactor);
int init_server(int port);
int set_listener(zv_reactor_t* reactor, int fd, ZVCALLBACK cb);
int zv_connect_block(zv_reactor_t* reactor);
zv_connect_t* zv_connect_idx(zv_reactor_t* reactor, int fd);

