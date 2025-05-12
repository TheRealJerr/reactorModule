// 这个头文件的作用是对面相过程的epoll模块进行封装，方便使用。
#pragma once
#include <unistd.h>
#include <sys/epoll.h>
#include "/home/hrj/include/Log.hpp"

namespace epollModule
{
    using namespace LogModule;
    class Epoll
    {
    public:
        Epoll()
        {
            _epoll_fd = epoll_create(1024);
            if (_epoll_fd < 0)
            {
                LOG(LogLevel::FATAL) << "epoll_create error";
                exit(1);
            }
            LOG(LogLevel::INFO) << "epoll_create success";
        }

        void addNewFd(int fd,uint32_t events)
        {
            // 添加新的事件
            struct epoll_event event;
            event.events = events;
            event.data.fd = fd;
            if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
            {
                LOG(LogLevel::FATAL) << "epoll_new error";
                exit(1);
            }
            LOG(LogLevel::INFO) << "epoll_add success";
        }
        void modifyFd(int fd,uint32_t events)
        {
            // 修改事件
            struct epoll_event event;
            event.events = events;
            event.data.fd = fd;
            if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &event) < 0)
            {
                LOG(LogLevel::FATAL) << "epoll_modify error";
                exit(1);
            }
            LOG(LogLevel::INFO) << "epoll_modify success";
        }
        void deleteFd(int fd)
        {
            // 删除事件
            if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
            {
                LOG(LogLevel::FATAL) << "epoll_delete error";
                exit(1);
            }
            LOG(LogLevel::INFO) << "epoll_delete success";
        }
        // 等待事件
        // 将等待的结果返回上层
        // 超时时间为-1时，表示一直等待
        int wait(struct epoll_event *events, int max_events, int timeout = -1)
        {
            int ret = epoll_wait(_epoll_fd, events, max_events, timeout);
            if (ret)   
            {
                LOG(LogLevel::INFO) << "epoll_wait success";
            }
            else if (ret == 0)
            {
                /* code */
                LOG(LogLevel::INFO) << "epoll_wait timeout";
            }
            else
            {
                LOG(LogLevel::FATAL) << "epoll_wait error";
                exit(1);
            }
            return ret;
        }
        ~Epoll()
        {
            close(_epoll_fd);
            LOG(LogLevel::INFO) << "epoll_close success";
        }

        int getEpollFd() const { return _epoll_fd; }
    private:
        int _epoll_fd;
    };
}