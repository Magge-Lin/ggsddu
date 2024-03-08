
#include <stdlib.h>
#include <stdio.h>

#include "wlnet.h"

typedef struct port_data_s
{
    int port_length;

    int buff[];
}port_data_t;


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