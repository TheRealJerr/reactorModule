#pragma once
#include "Common.h"
// 针对epoll模型进行组织

namespace reactor
{
#define MAX_EVENT 128
    class Epoll
    {
    public:
        Epoll();

        // 添加, 删除, 修改
        bool addEvent(int,uint32_t);
        
        bool delEvent(int,uint32_t);
        
        bool modifyEvent(int,uint32_t);
        //

        void setTimeOut(int timeout) { _timeout = timeout; }

        bool wait(struct epoll_event*&,int* n);
        ~Epoll();


    private:
        struct epoll_event _epoll_events[MAX_EVENT];
        int _epollfd;
        int _timeout = -1;
    };

}