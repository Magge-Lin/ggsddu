#include "threadpool.h"
#include "wlnet.h"

#define SERVER_PORT		8080
#define SERVER_IP		"127.0.0.1"
#define MAX_BUFFER		128
#define MAX_EPOLLSIZE	100000
#define MAX_THREAD		80
#define MAX_PORT		100

#define CPU_CORES_SIZE	8



int main(int argc, char* argv[])
{
    if(argc < 2)
        return -1;
    
    int port = atoi(argv[1]);
    int port_length = atoi(argv[2]);

	threadpool_init(); 

    wl_reactor_t reactor;
    wl_init_reactor(&reactor);

    port_data_t* portData = (port_data_t*)malloc(sizeof(port_data_t) + sizeof(int)*port_length);
    portData->port_length = port_length;
    for (int i = 0; i < port_length; i++)
    {
        int sockfd = init_server(port + i);
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