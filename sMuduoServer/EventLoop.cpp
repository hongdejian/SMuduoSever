#include "EventLoop.h"
#include <iostream>
#include <sys/eventfd.h>
#include <unistd.h>
#include <stdlib.h>

//参照muduo，实现跨线程异步唤醒
int CreateEventFd()
{
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        std::cout << "Failed in eventfd" << std::endl;
        exit(1);
    }
    return evtfd;
}

EventLoop::EventLoop()
:   pendingFunctors_(),
    channellist_(),
    activechannellist_(),
    poller_(),
    quit_(true),
    tid_(std::this_thread::get_id()),
    mutex_(),
    wakeupfd_(CreateEventFd()),
    wakeupchannel_()
{
    wakeupchannel_.setFd(wakeupfd_);
    wakeupchannel_.setEvents(EPOLLIN|EPOLLET);
    wakeupchannel_.setReadEventCallback(std::bind(&EventLoop::HandleRead,this));
    wakeupchannel_.setErrorEventCallback(std::bind(&EventLoop::HandleError,this));
}

EventLoop::~EventLoop(){
    close(wakeupfd_);
}

void EventLoop::WakeUp()
{
    uint64_t one=1;
    ssize_t n=write(wakeupfd_,(char*)(&one),sizeof(one));

}

void EventLoop::HandleRead(){
    uint64_t one=1;
    ssize_t n=read(wakeupfd_,&one,sizeof one);
}

void EventLoop::HandleError(){
    //perror("wakeup fail!");
    //exit(1);
}

void EventLoop::loop(){
    quit_=false;
    while(!quit_){
        poller_.poll(activechannellist_);

        for(Channel *pchannel:activechannellist_){
            pchannel->handleEvent();
        }
        activechannellist_.clear();
        doPendingFunctors();
    }
}