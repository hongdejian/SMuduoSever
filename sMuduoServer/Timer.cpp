#include"Timer.h"
#include<sys/time.h>
#include"TimerManager.h"

Timer::Timer(int timeout,TimerType timertype,const Callback&timercallback)
:   timeout_(timeout),
    timertype_(timertype),
    timercallback_(timercallback),
    rotation(0),
    timeslot(0),
    prev(nullptr),
    next(nullptr)
{
    if(timeout<0)
        return;
    //struct timeval tv;
    //gettimeofday(&tv,NULL);
    //timeout_=(tv.tv_sec%10000)*1000+tv.tv_usec/1000+timeout;
}
Timer::~Timer(){
    stop();
}

void Timer::start(){
    TimerManager::GetTimerManagerInstance()->AddTimer(this);
}

void Timer::stop(){
    TimerManager::GetTimerManagerInstance()->RemoveTimer(this);
}

void Timer::adjust(int timeout,Timer::TimerType timertype,const Callback &timercallback){
    timeout_=timeout;
    timertype_=timertype;
    timercallback_=timercallback;
    TimerManager::GetTimerManagerInstance()->AdjustTimer(this);
}