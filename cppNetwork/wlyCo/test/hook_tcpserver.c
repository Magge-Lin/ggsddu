

#include <arpa/inet.h>

#include "nty_coroutine.h"


void server_reader(void *arg)
{
    int fd = *(int *)arg;
    int ret = 0;

    while (1)
    {
        char buf[1024] = {0};
        ret = recv(fd, buf, 1024, 0);
        if (ret > 0)
        {
            printf("read form server: %. %s\n", ret, buf);

            ret = send(fd, buf, strlen(buf), 0);
            if (ret == -1)
            {
                close(fd);
                break;
            }
        }
        else if (ret == 0)
        {
            close(fd);
            break;
        }
    }
}

void server(void *arg)
{
    unsigned short port = *(unsigned short*)arg;
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        printf("socket is err. \n");
        return ;
    }
    
    struct sockaddr_in local, remote;
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = INADDR_ANY;
    bind(fd, (struct sockaddr*)&local, sizeof(struct sockaddr_in));

    listen(fd, 20);
    printf("listen port : %d \n", port);
    
    while (1)
    {
        socklen_t len = sizeof(struct sockaddr_in);
        int clientfd = accept(fd, (struct sockaddr*)&remote, &len);

        nty_coroutine *read_co;
        nty_coroutine_create(&read_co, server_reader, &clientfd);
    }
}


int main(int argc, char *argv[])
{
    int port = atoi(argv[1]);

    nty_coroutine *co = NULL;
    nty_coroutine_create(&co, server, &port);

    nty_schedule_run();
}