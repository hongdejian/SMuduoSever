//
//EventLoopThread类，表示IO线程,执行特定任务的,线程池的是通用任务线程
#ifndef _EVENTLOOP_THREAD_H_
#define _EVENTLOOP_THREAD_H_

#include<iostream>
#include<string>
#include<thread>
#include"EventLoop.h"

class EventLoopThread{
public: 
    EventLoopThread();
    ~EventLoopThread();

    EventLoop* getLoop();

    void start();

    void ThreadFuc();

private:
    //线程成员
    std::thread th_;

    //线程id
    std::thread::id threadid_;

    //线程名字
    std::string threadname_;

    //线程运行的loop循环
    EventLoop *loop_;

};



#endif