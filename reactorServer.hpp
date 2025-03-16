// 基于reactor模式的服务器端
#pragma once
#include <iostream>
#include <unordered_map>
#include "epollModule.hpp"
#include "Connection.hpp"
#include <memory>

// 最大的epoll事件数
#define MAX_CONNECTION_NUM 1024


using namespace epollModule;

using connection_ptr = std::shared_ptr<Connection::Connection>;

namespace Connection
{
    class Connection;
}
class ReactorServer
{
public:
    ReactorServer(int port) : _port(port)
    {
       
    }

    void run()
    {
        // 现在处于监听的状态
        while (_running)
        {
            // 等待epoll事件
            int ready_num = _epoll.wait(_events, MAX_CONNECTION_NUM, -1);
            // epoll wait中已经处理和返回值的error问题
            disPatch(ready_num);
        }
    }

    void disPatch(int ready_num)
    {
        // 遍历处理事件
        for(int i = 0; i < ready_num; i++)
        {
            uint32_t event = _events[i].events;
            int fd = _events[i].data.fd;
            // 多态不用判断类型，直接调用虚函数
            if(event & EPOLLERR || event & EPOLLHUP)
            {
                // 异常事件当做读写处理
                event |= EPOLLIN | EPOLLOUT;
            }
            if(isExitInConnection(fd))
            {
                // 事件存在
                auto& connection = _connections[fd];
                if(event & EPOLLIN)
                {
                    connection->recv();
                }
                if(event & EPOLLOUT)
                {
                    connection->send();
                }
            }
        }
    }

    void addNewConnection(connection_ptr connection)
    {
        if(isExitInConnection(connection->getFd()) == false)
        {
            // 注册到epoll
            _epoll.addNewFd(connection->getFd(), connection->getEvents());
            // 保存到连接池
            _connections.insert(std::make_pair(connection->getFd(), connection));
            LOG(LogLevel::INFO) << "add new connection " << connection->getFd();
        }
    }

    void delConnection(int fd)
    {
        // 判断是否存在
        if(isExitInConnection(fd) == false) return; 
        // 注销epoll
        _epoll.deleteFd(fd);
        // 从连接池中删除
        _connections.erase(fd);
        LOG(LogLevel::INFO) << "del connection " << fd;
    }
    bool isExitInConnection(int fd)
    {
        // 判断是否是退出事件
        return _connections.count(fd);
    }

    void enableEvent(int fd,bool read,bool write)
    {
        // 重新设置事件
        uint32_t events = (read ? EPOLLIN : 0) | (write ? EPOLLOUT : 0);
        _epoll.modifyFd(fd, events);
        _connections[fd]->setEvents(events);
        LOG(LogLevel::INFO) << "enable event " << fd << " read " << read << " write " << write;

    }
private:
    int _port;
    Epoll _epoll;
    std::unordered_map<int,connection_ptr> _connections;
    // 状态
    bool _running = true;
    // 定义事件缓冲区
    struct epoll_event _events[MAX_CONNECTION_NUM];
};

