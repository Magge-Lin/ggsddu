#include<sys/socket.h>
#include<cstring>
#include<unistd.h>
#include<netinet/in.h>
#include<pthread.h>
#include<sys/fcntl.h>
#include<stdlib.h>
#include<sys/poll.h>

#include<iostream>
using namespace std;

#define     POLL_SIZE       1024
#define     BUFFER_SIZE     1024
#define		errlog		cout << __LINE__ << " "


int main(int argc, char* argv[])
{
    if(argc < 2)
        return -1;

    int port = atoi(argv[1]);
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        errlog<<"create sockfd err."<<std::endl;
        return -1;
    }

    //非阻塞IO
    bool flags = fcntl(sockfd, F_GETFL);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    if(bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        errlog<<"bind err. sockfd:"<<sockfd<<"  port:"<<port<<std::endl;
        return -1;
    }

    if(listen(sockfd, 20) < 0)
    {
        errlog<<"listen err. sockfd:"<<sockfd<<"    port:"<<port<<std::endl;
        return -1;
    }

    errlog<<"listen port:"<<port<<std::endl;

    //poll
    struct pollfd fds[POLL_SIZE];
    fds[sockfd].fd = sockfd;
    fds[sockfd].events = POLLIN;

    int maxfd = sockfd;
    struct sockaddr_in clientAddr;
    socklen_t clientlen = sizeof(clientAddr);

    while (1)
    {
        int nready = poll(fds, maxfd+1, -1); 
        if (fds[sockfd].revents & POLLIN)
        {
            int clientfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientlen);
            if(clientfd < 0)
            {
                errlog<<"accept err. sockfd:"<<sockfd<<std::endl;
                continue;
            }

            fds[clientfd].fd = clientfd;
            fds[clientfd].events = POLLIN;

            if(clientfd > maxfd)
            {
                maxfd = clientfd;
            }
        }
        else
        {
            for(int i = sockfd+1; i <= maxfd; ++i)
            {
                if (fds[i].revents & POLLIN)
                {
                    char buff[BUFFER_SIZE] = {0};
                    int len = recv(i, buff, BUFFER_SIZE, 0);
                    if(len < 0)
                    {
                        errlog<<"recv err, sockfd:"<<sockfd<<"  clientfd:"<<i<<std::endl;
                        continue;
                    }
                    else if(len == 0)
                    {
                        errlog<<"close clientfd:"<<i<<std::endl;

                        fds[i].fd = -1;
                        fds[i].events = 0;

                        close(i);

                        continue;
                    }

                    errlog<<"recv:"<<buff<<std::endl;

                    send(i, buff, len, 0);
                }
            }
        }
    }
    
    close(sockfd);

    return 0;
}