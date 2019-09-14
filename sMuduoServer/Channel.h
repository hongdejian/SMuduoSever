#ifndef _CHANNEL_H_
#define _CHANNEL_H_
//Channel类，用于fd_事件的注册和分发，并不拥有fd
#include <functional>

class Channel{
public:
    typedef std::function<void()> EventCallback;
    Channel();
    ~Channel();
    void handleEvent();
    void setFd(int fd){
        fd_=fd;
    }
    int getFd()const{
        return fd_;
    }
    void setEvents(uint32_t events){
        events_=events;
    }

    uint32_t getEvents()const{
        return events_;
    }
    void setReadEventCallback(const EventCallback&cb){// std::move
        ReadEventCallback=cb;
    }
    void setWriteEventCallback(const EventCallback&cb){
        WriteEventCallback=cb;
    }
    void setErrorEventCallback(const EventCallback&cb){
        ErrorEventCallback=cb;
    }
    void setCloseEventCallback(const EventCallback&cb){
        CloseEventCallback=cb;
    }
private:
    int fd_;

    uint32_t events_;

    EventCallback ReadEventCallback;
    EventCallback WriteEventCallback;
    EventCallback ErrorEventCallback;
    EventCallback CloseEventCallback;
};
#endif