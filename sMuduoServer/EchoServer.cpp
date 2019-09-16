#include"EchoServer.h"
#include<iostream>
#include<functional>
//回射服务器 只用写业务逻辑就好
//底层拥有的tcpserver_(TCPServer)自动接收连接，并创建TCPConnection用于与客户端通信
EchoServer::EchoServer(EventLoop* loop,const int port,const int threadnum)
:   tcpserver_(loop,port,threadnum)
{
    //业务函数注册
    //注册新连接回调函数
    tcpserver_.SetNewConnCallback(std::bind(&EchoServer::HandleNewConnection,this,std::placeholders::_1));
    //注册数据回调函数
    tcpserver_.SetMessageCallback(std::bind(&EchoServer::HandleMessage,this,std::placeholders::_1,std::placeholders::_2));
    //注册数据发送完成回调函数
    tcpserver_.SetSendCompleteCallback(std::bind(&EchoServer::HanleSendComplete,this,std::placeholders::_1));
    //注册连接关闭回调函数
    tcpserver_.SetCloseCallback(std::bind(&EchoServer::HandleClose,this,std::placeholders::_1));
    //注册连接异常回调函数  
    tcpserver_.SetErrorCallback(std::bind(&EchoServer::HandleError,this,std::placeholders::_1));
}
EchoServer::~EchoServer()
{

}

void EchoServer::start()
{
    tcpserver_.start();
}

void EchoServer::HandleNewConnection(const spTcpConnection& sptcpconn)
{
    std::cout<<"New Connection Come in"<<std::endl;
}
void EchoServer::HandleMessage(const spTcpConnection& sptcpconn,std::string&s){
    std::string msg;
    msg.swap(s);
    msg.insert(0,"reply msg:");
    sptcpconn->Send(msg);
}

void EchoServer::HanleSendComplete(const spTcpConnection& sptcpconn){
    std::cout<<" Message send complete"<<std::endl;
}

void EchoServer::HandleClose(const spTcpConnection&sptcapconn){
    std::cout<<"EchoServer conn close"<<std::endl;
}

void EchoServer::HandleError(const spTcpConnection& sptcpconn){
    std::cout<<"EchoServer error"<<std::endl;
}