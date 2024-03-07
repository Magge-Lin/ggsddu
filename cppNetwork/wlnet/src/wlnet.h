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

typedef struct wl_connect_s
{
    int fd;
    int count;

    ZVCALLBACK cb;

    char rbuffer[BUFFER_LENGTH];
    int rc;

    char wbuffer[BUFFER_LENGTH];
    int wc;

}wl_connect_t;

typedef struct wl_connblock_s
{
    wl_connect_t* block;

    struct wl_connblock_s* next;
    
}wl_connblock_t;

typedef struct wl_reactor_s
{
    int epfd;
    int blk;

    wl_connblock_t* blockheader;
    
}wl_reactor_t;

int accept_cb(int fd, int event, void* arg);
int recv_cb(int fd, int event, void* arg);
int send_cb(int fd, int event, void* arg);

int wl_init_reactor(wl_reactor_t* reactor);
int wl_dest_reactor(wl_reactor_t* reactor);
int wl_dest_reactor(wl_reactor_t* reactor);
int init_server(int port);
int set_listener(wl_reactor_t* reactor, int fd, ZVCALLBACK cb);
int wl_connect_block(wl_reactor_t* reactor);
wl_connect_t* wl_connect_idx(wl_reactor_t* reactor, int fd);

