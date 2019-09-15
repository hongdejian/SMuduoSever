#include "EventLoopThread.h"
#include <iostream>
#include <sstream>
//#include <queue>
//#include <mutex>  
//#include <condition_variable>

EventLoopThread::EventLoopThread()
:   th_(),
    threadid_(-1),
    threadname_("IO thread"),
    loop_(nullptr)
{
    
}

EventLoopThread::~EventLoopThread()
{
    std::cout<<"Clean up the EventLoopThread id: "<<std::this_thread::get_id()<<std::endl;
    loop_->quit();
    th_.join();
}

EventLoop* EventLoopThread::getLoop()
{
    return loop_;
}

void EventLoopThread::start(){
    th_=std::thread(&EventLoopThread::ThreadFuc,this);
}

void EventLoopThread::ThreadFuc(){
    EventLoop loop;
    loop_=&loop;

    threadid_=std::this_thread::get_id();
    std::stringstream sin;

    sin<<threadid_;
    threadname_+=sin.str();

    std::cout<<"in this thread:"<<threadname_<<std::endl;

    try
    {
        loop_->loop();
    }
    catch(std::bad_alloc&ba){
        std::cerr<<"bad_alloc caught in EventLoopThread::ThreadFunc loop: "<<ba.what()<<'\n';
    }

}