#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_


#include <iostream>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include "Poller.h"
#include "Channel.h"

class EventLoop{
public:
    typedef std::function<void()> Functor;
    typedef std::vector<Channel*> ChannelList;
    EventLoop();
    ~EventLoop();

    void loop();
    void quit(){
        quit_=true;
    }
    void AddChannelToPoller(Channel* pchannel){
        poller_.AddChannel(pchannel);
    }

    void RemoveChannelFromPoller(Channel* pchannel){
        poller_.RemoveChannel(pchannel);
    }
    void UpdateChannelInPoller(Channel* pchannel){
        poller_.UpdateChannel(pchannel);
    }
    std::thread::id GetThreadId() const{
        return tid_;
    }

    //异步唤醒loop
    void WakeUp();

    //唤醒loop后的读回调
    void HandleRead();

    //唤醒loop后的错误处理回调
    void HandleError();

    void AddFunctor(Functor functor){
        {
            std::lock_guard<std::mutex> lock(mutex_);
            pendingFunctors_.push_back(functor);
        }
        WakeUp();//跨线程唤醒，worker线程唤醒IO线程
    }

    void doPendingFunctors(){
        std::vector<Functor> functorlist;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            functorlist.swap(pendingFunctors_);
        }

        for(Functor& functor:functorlist){
            functor();
        }
        functorlist.clear();
    }
private:
    std::vector<Functor> pendingFunctors_;
    
    ChannelList activechannellist_;

    ChannelList channellist_;

    Poller poller_;
    bool quit_;
    std::thread::id tid_;

    std::mutex mutex_;
    int wakeupfd_;

    Channel wakeupchannel_;
};
#endif