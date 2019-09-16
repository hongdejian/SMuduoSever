#ifndef _ECHO_SERVER_H_
#define _ECHO_SERVER_H_

#include<string>
#include"TcpServer.h"
#include"EventLoop.h"
#include"TcpConnection.h"

class EchoServer{
public:
    typedef std::shared_ptr<TcpConnection> spTcpConnection;

    EchoServer(EventLoop* loop,const int port,const int threadnum);
    ~EchoServer();

    void start();
private:
    void HandleNewConnection(const spTcpConnection& sptcpconn);
    void HandleMessage(const spTcpConnection& sptcpconn,std::string& s);
    void HanleSendComplete(const spTcpConnection& sptcpconn);
    void HandleClose(const spTcpConnection& sptcpconn);
    void HandleError(const spTcpConnection& sptcpconn);

    TcpServer tcpserver_;
};
#endif