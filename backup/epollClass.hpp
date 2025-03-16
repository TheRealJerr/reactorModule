// 对epoll进行封装
#include <iostream>
#include <sys/epoll.h>
#include "/home/hrj/include/Log.hpp"
#include <cstring>
namespace epollClassModule
{
    using namespace LogModule;
    class epollClass
    {
    public:
        epollClass()
        {
            // 创建epoll模型
            _epollfd = ::epoll_create(256);
            if(_epollfd < 0)
            {
                LOG(LogLevel::FATAL) << "epoll class create fail" << strerror(errno);
                exit(1); 
            }
        }
        // 向epoll模型中添加新的文件描述符
        bool insertFd(int fd,uint32_t cmd) // cmd表示读或者写
        {
            // 构建事件
            struct epoll_event i_event;
            i_event.events = cmd;
            i_event.data.fd = fd;
            int n = ::epoll_ctl(_epollfd,EPOLL_CTL_ADD,fd,&i_event);
            if(n < 0)
            {
                LOG(LogLevel::ERROR) << "epoll add " << fd << " error " << strerror(errno);
                return false;
            }
            else return false;
        }
        int Wait(struct epoll_event* events,int max_event,int timeout = -1) //默认是阻塞等待
        {
            int n = ::epoll_wait(_epollfd,events,max_event,timeout);
            if(n < 0)
            {
                LOG(LogLevel::ERROR) << "wait error:" << strerror(errno);
                return -1;
            }
            return n;
        }
    private:
        int _epollfd;
    };
}