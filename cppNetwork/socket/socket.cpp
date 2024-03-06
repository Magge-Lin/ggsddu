#include<sys/socket.h>
#include<cstring>
#include <unistd.h>
#include<netinet/in.h>
#include<sys/fcntl.h>
#include<stdlib.h>

#include<iostream>
using namespace std;


#define     BUFFER_LENGTH           1024
#define		errlog		cout<<__LINE__<<" "

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        errlog<<"argc < 2"<<std::endl;
        return 0;
    }
    
    int port = atoi(argv[1]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd <= 0)
    {
        errlog<<"create sockfd err."<<std::endl;
        return -1;
    }

    //非阻塞IO
    // int flags = fcntl(sockfd, F_GETFL, 0);
    // fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0 , sizeof(struct sockaddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    if(bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr)) < 0)
    {
        errlog<<"bind is err."<<std::endl;
        return -1;
    }

    if(listen(sockfd, 20) < 0)
    {
        errlog<<"listen is err."<<std::endl;
        return -1;
    }

    errlog<<"listen port:"<<port<<std::endl;

    struct sockaddr_in clientAddr;
    socklen_t clientlen = sizeof(clientAddr);
    
    int clientfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientlen);
    
    errlog<<"accept clientfd:"<<clientfd<<std::endl;

    char buf[BUFFER_LENGTH] = {0};
    memset(buf, 0, BUFFER_LENGTH);

    int buflength = recv(clientfd, buf, BUFFER_LENGTH, 0);
    if(buflength < 0)
    {
        errlog<<"recv is err. clientfd = "<<clientfd<<std::endl;
        return -1;
    }
    else if(0 == buflength)
    {
        //close clientfd
        errlog<<"close clientfd: "<<clientfd<<std::endl;
        close(clientfd);
        return 0;
    }
    else
    {
        errlog<<"buf:"<<buf<<std::endl;
        //do something
    }

    send(clientfd, buf, buflength, 0);

    close(sockfd);

    return 0;
}
