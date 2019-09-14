   //    typedef union epoll_data {
        //        void    *ptr;
        //        int      fd;
        //        uint32_t u32;
        //        uint64_t u64;
        //    } epoll_data_t;

        //    struct epoll_event {
        //        uint32_t     events;    /* Epoll events */
        //        epoll_data_t data;      /* User data variable */
        //    };
#include "Poller.h"
#include <iostream>
#include <stdio.h> //perror
#include <stdlib.h> //exit
#include <unistd.h> //close
#include <errno.h>

#define EVENTNUM 4096 //最大触发事件数量
#define TIMEOUT 1000 //epoll_wait 超时时间设置
Poller::Poller()
:   pollfd_(-1),
    eventlist_(EVENTNUM),
    channelMap_(),
    mutex_()
{
    pollfd_=epoll_create(256);
    if(pollfd_==-1){
        perror("epoll_create1");
        exit(1);
    }
    std::cout<<"epoll_create"<<pollfd_<<std::endl;
}

Poller::~Poller()
{
    close(pollfd_);
}

void Poller::poll(ChannelList &activechannellist){
    int timeout=TIMEOUT;

    int nfds=epoll_wait(pollfd_,&*eventlist_.begin(),(int)eventlist_.capacity(),timeout);

    if(nfds==-1){
        perror("epoll wait error");
    }

    for(int i=0;i<nfds;i++){
        int events=eventlist_[i].events;
        Channel *pchannel=static_cast<Channel*>(eventlist_[i].data.ptr);
        int fd=pchannel->getFd();

        std::map<int,Channel*>::const_iterator iter;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            iter=channelMap_.find(fd);
        }

        if(iter!=channelMap_.end()){
            pchannel->setEvents(events);
            activechannellist.push_back(pchannel);
        }
        else{
            std::cout<<"not find channel!"<<std::endl;
        }

        if(nfds==(int)eventlist_.capacity()){
            std::cout<<"resize: "<<nfds<<std::endl;
            eventlist_.resize(2*nfds);
        }
    }
}

void Poller::AddChannel(Channel* pchannel){
    int fd=pchannel->getFd();
    struct epoll_event ev;
    ev.events=pchannel->getEvents();
    //data是联合体
    //ev.data.fd = fd;

    ev.data.ptr=pchannel;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        channelMap_[fd]=pchannel;
    }

    if(epoll_ctl(pollfd_,EPOLL_CTL_ADD,fd,&ev)==-1){
        perror("epoll add error");
        exit(1);
    }
}
void Poller::RemoveChannel(Channel* pchannel){
    int fd=pchannel->getFd();
    struct epoll_event ev;
    ev.events=pchannel->getEvents();
    ev.data.ptr=pchannel;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        channelMap_.erase(fd);
    }
    if(epoll_ctl(pollfd_,EPOLL_CTL_DEL,fd,&ev)==-1){
        perror("epoll delete error");
        exit(1);
    }
}
void Poller::UpdateChannel(Channel* pchannel){
    int fd=pchannel->getFd();
    struct epoll_event ev;
    ev.events=pchannel->getEvents();
    ev.data.ptr=pchannel;
    if(epoll_ctl(pollfd_,EPOLL_CTL_MOD,fd,&ev)==-1){
        perror("epoll update error");
        exit(1);
    }
}