// 在我们reactor下,统一管理connection
#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include "Common.hpp"

// 声明reactorServer


#include "/home/hrj/include/Log.hpp"

// 声明reactorServer
class ReactorServer;

namespace Connection
{
    class Connection;
    using namespace LogModule;
    class Connection
    {
    public:
        Connection(int fd) : _fd(fd) {}

        int getFd() const { return _fd; }

        virtual void recv() = 0;
        
        virtual void send() = 0;

        virtual void close()
        {
            // 关闭连接
            ::close(_fd);
        }
        
        void setFd(int fd) { _fd = fd; }

        void setEvents(uint32_t events) { _events = events; }

        uint32_t getEvents() const { return _events; }

        virtual ~Connection() {};

        auto* getOwner() const { return _owner; }

        void setOwner(ReactorServer* owner) { _owner = owner; }

        virtual void handlerError() = 0;


    private:
        int _fd;
        uint32_t _events;
        ReactorServer* _owner = nullptr; // 所属的reactorServer
    };
}