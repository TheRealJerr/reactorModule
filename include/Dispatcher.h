#pragma once

// 通过epoll模型组织派发数据


#include "Common.h"
#include "Epoll.h"
#include "net.h"
#include "ThreadPool.h"
#include "Server.h"
// main dispathcher负责监听套接字, 并且写入eventfd中

namespace reactor
{
    // 通过监听套接字accept后传递给ThreadDispacher
    // 通过RR轮询的方式实现的连接的派发
    class MainDispatcher
    {
    public:

        MainDispatcher(int listen_socket);

        int addNewListener();

        void run();
    private:
        int _listen_socket;
        bool _is_run;
        std::vector<int> _readers;
        int _cur_pos = 0;
        // 处理连接回调接口
    };


    class ThreadDispatcher
    {
    public:
        using Ptr = std::shared_ptr<ThreadDispatcher>;
        
        ThreadDispatcher(int readfd) : _readfd(readfd)
        {
            _epoll.addEvent(_readfd, EPOLLIN);
        }

        void run();
        // 读取就绪
        void readReady();
        // 套接字数据就绪
        void eventReady(int eventfd); // 套接字的读取就绪

        void setOnConnectionCallBack(const onConnectionCallBack& cb) { _con_cb = cb; }
        
        void setOnMsgCallBack(const onMessageCallBack& cb) { _cb = cb; }
    private:
        int _readfd; // 读取套接字
        std::unordered_map<int,Connection::Ptr> _cons; // 存储通信连接
        Epoll _epoll;
        bool _is_run = true;
        onConnectionCallBack _con_cb;
        onMessageCallBack _cb;
    };
}