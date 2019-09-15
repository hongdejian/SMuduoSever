#include"EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *mainloop,int threadnum)
:   mainLoop_(mainloop),
    threadNum_(threadnum),
    threadlist_(),
    index_(0)
{
//threadlist_
//包含很多EventLoopThread,但是mainLoop所在线程不在threadlist_里
    for(int i=0;i<threadNum_;i++){
        EventLoopThread *peventloopthread=new EventLoopThread;
        threadlist_.push_back(peventloopthread);
    }
}

EventLoopThreadPool::~EventLoopThreadPool(){
    std::cout<<"Clean up the EventLoopThreadPool"<<std::endl;
    for(int i=0;i<threadNum_;i++){
        delete threadlist_[i];
    }
    threadlist_.clear();
}

void EventLoopThreadPool::start(){
    if(threadNum_>0){
        for(int i=0;i<threadNum_;i++){
            threadlist_[i]->start();
        }
    }
}

EventLoop* EventLoopThreadPool::getNextLoop(){
    if(threadNum_>0){
        EventLoop *loop=threadlist_[index_]->getLoop();
        index_=(index_+1)%threadNum_;
        return loop;
    }
    else{
        return mainLoop_;
    }
}