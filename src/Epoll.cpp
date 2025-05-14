#include "../include/Epoll.h"

namespace reactor
{
    Epoll::Epoll()
    {
        // 构建epoll模型
        _epollfd = ::epoll_create(8);
        if(_epollfd < 0)
        {
            log(LogLevel::FATAL) << "epoll create fail";
            ::exit(1);
        }
    }

    Epoll::~Epoll()
    {
        // 删除epoll模型
        ::close(_epollfd);        
    }
    // event_type表示关心读还是关心写
    // event_type 可以是 EPOLLIN || EPOLLOUT
    bool Epoll::addEvent(int newfd,uint32_t event_type)
    {
        struct epoll_event event;
        event.events = event_type | EPOLLET; // 将事件设置成边沿触发
        event.data.fd = newfd;
        //
        int n = ::epoll_ctl(_epollfd,EPOLL_CTL_ADD,newfd,&event);

        if(n < 0)
        {
            log(LogLevel::ERROR) << "epoll add fail" << strerror(errno);
            return false;
        }
        return true;
    }
    bool Epoll::delEvent(int fd,uint32_t event_type)
    {
        struct epoll_event event;
        event.events = event_type;
        event.data.fd = fd;
        //
        int n = ::epoll_ctl(_epollfd,EPOLL_CTL_DEL,fd,&event);

        if(n < 0)
        {
            log(LogLevel::ERROR) << "epoll del fail" << strerror(errno);
            return false;
        }
        return true;
    }
    bool Epoll::modifyEvent(int oldfd,uint32_t event_type)
    {
        struct epoll_event event;
        event.events = event_type | EPOLLET;
        event.data.fd = oldfd;
        //
        int n = ::epoll_ctl(_epollfd,EPOLL_CTL_DEL,oldfd,&event);

        if(n < 0)
        {
            log(LogLevel::ERROR) << "epoll modify fail" << strerror(errno);
            return false;
        }
        return true;
    }

    bool Epoll::wait(struct epoll_event* events,int* n)
    {
        int nfs = ::epoll_wait(_epollfd,_epoll_events,MAX_EVENT,_timeout);
        if(nfs < 0)
        {
            log(LogLevel::ERROR) << "epoll wait error" << strerror(errno);
            return false;
        }
        events = _epoll_events;
        *n = nfs;
        return true;
    }
}