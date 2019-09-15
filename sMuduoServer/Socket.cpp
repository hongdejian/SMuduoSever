//int listen( int sockfd, int backlog);
//sockfd：用于标识一个已捆绑未连接套接口的描述字
//backlog：等待连接队列的最大长度

#include "Socket.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <cstring>

Socket::Socket()
{
    serverFd_=socket(AF_INET,SOCK_STREAM,0);
    if(serverFd_==-1){
        perror("socket create fail");
        exit(-1);
    }

    std::cout<<"server create socket"<<serverFd_<<std::endl;
}

Socket::~Socket()
{
    close(serverFd_);
    std::cout<<"server close..."<<std::endl;

}

void Socket::SetSocketOption()
{
    ;
}

void Socket::SetReuseAddr(){
    int on=1;
    setsockopt(serverFd_,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
}

void Socket::Setnonblocking()
{
    int opts=fcntl(serverFd_,F_GETFL);
    if(opts<0)
    {
        perror("fcntl(serverFd_,GETFL");
        exit(1);
    }
    if(fcntl(serverFd_,F_SETFL,opts|O_NONBLOCK)<0){
        perror("fcntl(serverFd,SETFL,opts)");
        exit(1);
    }
    std::cout<<"server setnonblocking..."<<std::endl;
}

bool Socket::BindAddress(int serverport){
    struct  sockaddr_in serveraddr;
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
    serveraddr.sin_port=htons(serverport);
    int resval=bind(serverFd_,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(resval==-1){
        close(serverFd_);
        perror("bind error");
        exit(1);
    }
    std::cout<<"server bindaddress..."<<std::endl;
    return true;
}

bool Socket::Listen()
{

    if(listen(serverFd_,8192)<0){
        perror("error listen");
        close(serverFd_);
        exit(1);
    }
    std::cout<<"server listenning..."<<std::endl;
    return true;
}

int Socket::Accept(struct sockaddr_in&clientaddr)
{
    socklen_t lengthofsockaddr=sizeof(clientaddr);
    int clientfd=accept(serverFd_,(struct sockaddr*)&clientaddr,&lengthofsockaddr);
    if(clientfd<0){
        //perror("error accept");
        if(errno == EAGAIN)
            return 0;
		//std::cout << "error accept:there is no new connection accept..." << std::endl;
        return clientfd;
    }
    return clientfd;
}

bool Socket::Close()
{
    close(serverFd_);
    std::cout<<"server close..."<<std::endl;
    return true;
}