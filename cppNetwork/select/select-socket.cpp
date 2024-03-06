#include<sys/socket.h>
#include<cstring>
#include<unistd.h>
#include<netinet/in.h>
#include<pthread.h>
#include<sys/fcntl.h>
#include<stdlib.h>
#include<sys/select.h>

#include<iostream>
using namespace std;

#define     BUFFER_LENGTH           1024
#define		errlog		cout<<__LINE__<<" "


int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        errlog<<"please input port."<<std::endl;
        return -1;
    }
    
    int port = atoi(argv[1]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        errlog<<"create sockfd err."<<std::endl;
        return -1;
    }

    //非阻塞IO      -- 仅作联系使用~
    bool flags = fcntl(sockfd, F_GETFL);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    if(bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        errlog<<"bind sockfd err."<<std::endl;
        return -1;
    }

    if(listen(sockfd, 20) < 0)
    {
        errlog<<"listen err.    port:"<<port<<std::endl;
        return -1;
    }

    errlog<<"listen port:"<<port<<std::endl;

    fd_set rfds,rset;
    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);

    int maxfd = sockfd;

    while (1)
    {
        rset = rfds;
        int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if(FD_ISSET(sockfd, &rset))
        {
            struct sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            int clientfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientLen);
            if(clientfd <= 0)
            {
                errlog<<"accept err. sockfd:"<<sockfd<<std::endl;
                continue;
            }
            errlog<<"accept sockfd:"<<sockfd<<"     clientfd:"<<clientfd<<std::endl;

            FD_SET(clientfd, &rfds);    //这里需要绑定在rfds而不是rset

            if(clientfd > maxfd)
            {
                maxfd = clientfd;
            }
        }
        else
        {
            for(int i = sockfd + 1; i <= maxfd; ++i)
            {
                if(FD_ISSET(i, &rset))
                {
                    char buff[BUFFER_LENGTH] = {0};
                    int len = recv(i, buff, BUFFER_LENGTH, 0);
                    if(len <= 0)
                    {
                        errlog<<"close clientfd:"<<i<<std::endl;
                        close(i);
                        FD_CLR(i,&rfds);        //这里需要在rfds解除绑定
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
