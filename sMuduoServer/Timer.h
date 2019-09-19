//Timer类，定时器，生命周期由用户自行管理

#ifndef _TIMER_H_
#define _TIMER_H_   

#include<functional>

class Timer{
public:
    typedef std::function<void()> Callback;
    typedef enum{
        TIMER_ONCE=0,//一次触发型
        TIMER_PERIOD//周期触发型
    }TimerType;

    //超时时间,单位ms
    int timeout_; 

    TimerType timertype_;

    Callback timercallback_;
    //定时器剩下的转数
    int rotation;
     //定时器所在的时间槽
    int timeslot;

    Timer *prev;
    Timer *next;

    Timer(int timeout,TimerType timertype,const Callback& timercallback);
    ~Timer();

    //定时器启动，加入管理器
    void start();
    //定时器撤销，从管理器中删除
    void stop();

    void adjust(int timeout,Timer::TimerType timertype, const Callback &timercallback);
private:

};
#endif