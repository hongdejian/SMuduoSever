#include "ThreadPool.h"
#include <deque>
#include <unistd.h>

ThreadPool::ThreadPool(int threadnum)
:   started_(false),
    threadNum_(threadnum),
    threadList_(),
    taskQueue_(),
    mutex_(),
    condtion_()
{

}

ThreadPool::~ThreadPool()
{
    std::cout<<"Clean up the ThreadPool"<<std::endl;
    stop();

    for(int i=0;i<threadNum_;i++){
        threadList_[i]->join();
    }
    for(int i=0;i<threadNum_;i++){
        delete threadList_[i];
    }
    threadList_.clear();
}

void ThreadPool::start()
{
    if(threadNum_>0)
    {
        started_=true;

        for(int i=0;i<threadNum_;i++){
            std::thread *pthread=new std::thread(&ThreadPool::ThreadFunc,this);
            threadList_.push_back(pthread);
        }
    }
}

void ThreadPool::stop()
{
    started_=false;
    condtion_.notify_all();
}

void ThreadPool::AddTask(Task task){
    {
        std::lock_guard<std::mutex> lock(mutex_);
        taskQueue_.push(task);
    }
    condtion_.notify_one();
}

void ThreadPool::ThreadFunc()
{
    std::thread::id tid=std::this_thread::get_id();
    std::stringstream sin;
    sin<<tid;
    std::cout<<"work thread is running :"<<tid<<std::endl;

    Task task;
    while(started_){
        //std::cout<<"ThreadPool started."<<std::endl;
        task=nullptr;
        {
            std::unique_lock<std::mutex> lock(mutex_);//unique_lock支持解锁又上锁情况
            while(taskQueue_.empty()&&started_)
            {
                condtion_.wait(lock);
            }
            if(!started_){
                break;
            }
            std::cout<<"wake up "<<tid<<std::endl;
            std::cout<<"size :"<<taskQueue_.size()<<std::endl;
            task=taskQueue_.front();
            taskQueue_.pop();
        }
        //FixMe::task must in while(stated_)!@!!
        if(task!=nullptr){

            
            try
            {
                task();
                std::cout<<"task over"<<std::endl;
            }
            catch(const std::bad_alloc& e)
            {
                std::cerr <<"bad_alloc caught in ThreadPool:: ThreadFunc task :"<< e.what() << '\n';
                exit(1);
            }
            //task();//task中的IO过程可以使用协程优化，让出CPU
        }
    }
    
}

// 测试
// class Test
// {
// private:
//     /* data */
// public:
//     Test(/* args */);
//     ~Test();
//     void func()
//     {
//         std::cout << "Work in thread: " << std::this_thread::get_id() << std::endl;
//     }
// };

// Test::Test(/* args */)
// {
// }

// Test::~Test()
// {
// }


// int main()
// {
//     Test t;
//     ThreadPool tp(2);
//     tp.Start();
//     for (int i = 0; i < 1000; ++i)
//     {
//         std::cout << "addtask" << std::this_thread::get_id() << std::endl;
//         tp.AddTask(std::bind(&Test::func, &t));
//     }
// }