#include "TimerManager.h"
#include <iostream>
#include <thread>
#include <ctime>
#include <ratio>
#include <chrono>
#include <unistd.h>
#include <sys/time.h>

//初始化

TimerManager* TimerManager::ptimermanager_=nullptr;
std::mutex TimerManager::mutex_;
TimerManager::GC TimerManager::gc;
const int TimerManager::slotinterval=1;
const int TimerManager::slotnum=1024;

TimerManager::TimerManager()
:   currentslot(0),
    timerwheel(slotnum,nullptr),
    running_(false),
    th_()
{

}

TimerManager::~TimerManager()
{
    stop();
}

TimerManager* TimerManager::GetTimerManagerInstance()
{
    if(ptimermanager_==nullptr)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if(ptimermanager_==nullptr){
            //改进，创建临时变量防止TimerManager还未创建就使用
            TimerManager *tmp=new TimerManager();
            ptimermanager_=tmp;
        }
    }
    return ptimermanager_;
}

void TimerManager::AddTimer(Timer* ptimer){
    if(ptimer==nullptr)
        return;
    std::lock_guard<std::mutex> lock(timewheelmutex_);
    calculateTimer(ptimer);
    addTimertoTimerWheel(ptimer);
}

void TimerManager::RemoveTimer(Timer* ptimer){
    if(ptimer==nullptr){
        return;
    }
    std::lock_guard<std::mutex> lock(timewheelmutex_);
    removeTimerFromTimerWheel(ptimer);
}

void TimerManager::AdjustTimer(Timer* ptimer){
    if(ptimer==nullptr) return;

    std::lock_guard<std::mutex> lock(timewheelmutex_);
    adjustTimerInTimerWheel(ptimer);
}

void TimerManager::calculateTimer(Timer *ptimer){
    if(ptimer==nullptr) return;

    int tick=0;
    int timeout=ptimer->timeout_;
    if(timeout<slotinterval){
        tick=1;
    }
    else{
        tick=timeout/slotinterval;
    }

    ptimer->rotation=tick/slotnum;
    ptimer->timeslot=(currentslot+tick)%slotnum;
}

void TimerManager::addTimertoTimerWheel(Timer* ptimer){
    if(ptimer==nullptr) return;

    int timeslot=ptimer->timeslot;

    if(timerwheel[timeslot]){
        ptimer->next=timerwheel[timeslot];
        timerwheel[timeslot]->prev=ptimer;
        timerwheel[timeslot]=ptimer;
    }
    else{
        timerwheel[timeslot]=ptimer;
    }
}

void TimerManager::removeTimerFromTimerWheel(Timer* ptimer){
    if(ptimer==nullptr) return;

    int timeslot=ptimer->timeslot;

    if(ptimer==timerwheel[timeslot]){
        timerwheel[timeslot]=ptimer->next;
        if(timerwheel[timeslot]!=nullptr){
            timerwheel[timeslot]->prev=nullptr;
        }
        ptimer->prev=ptimer->next=nullptr;
    }
    else{
        if(ptimer->prev==nullptr)//不在时间轮的链表中，即已经被删除了
            return;
        ptimer->prev->next=ptimer->next;
        if(ptimer->next!=nullptr){
            ptimer->next->prev=ptimer->prev;
        }
        ptimer->prev=ptimer->next=nullptr;
    }
}

void TimerManager::adjustTimerInTimerWheel(Timer* ptimer){
    if(ptimer==nullptr)
        return;

    removeTimerFromTimerWheel(ptimer);
    calculateTimer(ptimer);
    addTimertoTimerWheel(ptimer);
}

void TimerManager::checkTimer(){//执行当前slot的任务
    std::lock_guard<std::mutex> lock(timewheelmutex_);
    Timer* ptimer=timerwheel[currentslot];
    while(ptimer!=nullptr){
        if(ptimer->rotation>0){
            --ptimer->rotation;
            ptimer=ptimer->next;
        }
        else
        {
            ptimer->timercallback_();
            if(ptimer->timertype_==Timer::TimerType::TIMER_ONCE){
                Timer* ptemptimer=ptimer;
                ptimer=ptimer->next;
                removeTimerFromTimerWheel(ptimer);
            }
            else{
                Timer* ptemptimer=ptimer;
                ptimer=ptimer->next;
                adjustTimerInTimerWheel(ptemptimer);
                if(ptemptimer->timeslot==currentslot&&ptemptimer->rotation>0){

                    //如果定时器多于一转的话，需要先对rotation减1，否则会等待两个周期
                    --ptemptimer->rotation;
                }
            }
        }
        
    }
    currentslot=(++currentslot)%TimerManager::slotnum;
}

void TimerManager::checkTick()
{
    int si=TimerManager::slotinterval;
    struct timeval tv;
    gettimeofday(&tv,nullptr);
    int oldtime=(tv.tv_sec%10000)*1000+tv.tv_usec/1000;
    int time;
    int tickcount;

    while(running_){
        gettimeofday(&tv,NULL);
        time=(tv.tv_sec%10000)*1000+tv.tv_usec/1000;
        tickcount=(time-oldtime)/slotinterval;//计算两次check的时间间隔占多少个slot

        oldtime=oldtime+tickcount*slotinterval;
        for(int i=0;i<tickcount;i++){
            TimerManager::GetTimerManagerInstance()->checkTimer();
        }
        std::this_thread::sleep_for(std::chrono::microseconds(500));//milliseconds(si)时间粒度越小，延时越不精确，因为上下文切换耗时
    }
    
}


void TimerManager::start()
{
    running_=true;
    th_=std::thread(&TimerManager::checkTick,this);
}

void TimerManager::stop()
{
    running_=false;
    if(th_.joinable())
        th_.join();
}