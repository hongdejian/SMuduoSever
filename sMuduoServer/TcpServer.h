#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <functional>
#include <string>
#include <map>
#include <mutex>
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "TcpConnection.h"
#include "EventLoopThreadPool.h"

#define MAXCONNECTION 20000
class TcpServer{
public:
    typedef std::shared_ptr<TcpConnection> spTcpConnection;

    typedef std::function<void(const spTcpConnection&,std::string&)> MessageCallback;
    typedef std::function<void(const spTcpConnection&)> Callback;
    TcpServer(EventLoop *loop,const int port,const int threadnum=0);
    ~TcpServer();

    void start();

    //业务函数注册
    //注册新连接回调函数
    void SetNewConnCallback(const Callback&cb){
        newConnectioncallback_=cb;
    }

    //注册数据回调函数
    void SetMessageCallback(const MessageCallback&cb){
        messageCallback_=cb;
    }
    //注册数据发送完成回调函数
    void SetSendCompleteCallback(const Callback&cb){
        sendCompleteCallback_=cb;
    }

    //注册连接关闭回调函数
    void SetCloseCallback(const Callback&cb){
        closeCallback_=cb;
    }
    //注册连接异常回调函数 
    void SetErrorCallback(const Callback&cb){
        errorCallback_=cb;
    }
private:
    Socket serversocket_;
    
    //主loop
    EventLoop *loop_;
    Channel serverchannel_;
    int connCount_;

    std::map<int,spTcpConnection> tcpconnMap_;
    std::mutex mutex_;

    EventLoopThreadPool eventloopthreadpool_;

    //业务接口回调函数
    Callback newConnectioncallback_;
    MessageCallback messageCallback_;
    Callback sendCompleteCallback_;
    Callback closeCallback_;
    Callback errorCallback_;

    //服务器对新连接连接处理的函数
    void OnNewConnection();

    //移除TCP连接函数
    void RemoveConnection(const std::shared_ptr<TcpConnection> sptcpconnection);

    //服务器socket的异常处理函数
    void OnConnectionError();
};
#endif