//服务器socket类，封装socket描述符及相关的初始化操作

#ifndef _SOCKET_H
#define _SOCKET_H

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

class Socket
{
private:
//服务器socket文件描述符
    int serverFd_;
public:
    Socket();
    ~Socket();

    int fd()const{
        return serverFd_;
    }

    void SetSocketOption();

    void SetReuseAddr();

    void Setnonblocking();

    bool BindAddress(int serverport);

    bool Listen();

    int Accept(struct sockaddr_in &clientaddr);

    bool Close();
};
#endif