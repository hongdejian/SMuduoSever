//TimerManager类，定时器管理，基于时间轮实现，增加删除O(1),执行可能复杂度高些，slot多的话可以降低链表长度

#ifndef _TIMER_MANAGER_H_
#define _TIMER_MANAGER_H_

#include <functional>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include "Timer.h"
class TimerManager{
public: 
    typedef std::function<void()> Callback;
    
    //单例模式，懒汉
    static TimerManager* GetTimerManagerInstance();

    void AddTimer(Timer* ptimer);

    void RemoveTimer(Timer* ptimer);

    void AdjustTimer(Timer* ptimer);

    //开启线程，定时器启动
    void start();

    void stop();
    
    //垃圾回收，程序结束的时候析构TimerManager
    class GC
    {
        public:
        ~GC(){
            if(ptimermanager_!=nullptr)
                delete ptimermanager_;
        }
    };
private:
    TimerManager();
    ~TimerManager();
    static TimerManager* ptimermanager_;

    static std::mutex mutex_;
    static GC gc;

    //时间轮相关成员
    //当前slot
    int currentslot;

    //每个slot的时间间隔,ms
    static const int slotinterval;
    //slot总数
    static const int slotnum;

    std::vector<Timer*> timerwheel;

    //时间轮互斥量
    std::mutex timewheelmutex_;

    //时间轮运行状态
    bool running_;

    //定时器线程
    std::thread th_;

    //时间轮操作内部函数
    //检查超时任务
    void checkTimer();
    
    //线程实际执行的函数
    void checkTick();

    //计算定时器参数
    void calculateTimer(Timer* ptimer);

    void addTimertoTimerWheel(Timer* ptimer);

    void removeTimerFromTimerWheel(Timer* ptimer);

    void adjustTimerInTimerWheel(Timer* ptimer);
};
#endif