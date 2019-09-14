#ifndef _POLLER_H_
#define _POLLER_H_

#include <vector>
#include <mutex>
#include <map>
#include <sys/epoll.h>
#include "Channel.h"

class Poller{
public:
    typedef std::vector<Channel*> ChannelList;

    int pollfd_;

    std::vector<struct epoll_event> eventlist_;
    std::map<int,Channel*> channelMap_;

    std::mutex mutex_;

    Poller();
    ~Poller();


    void poll(ChannelList &activeChannelList);

    void AddChannel(Channel *pchannel);
    void RemoveChannel(Channel *pchannel);
    void UpdateChannel(Channel *pchannel);
};

#endif