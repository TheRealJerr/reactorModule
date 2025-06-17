#pragma once
#include "Common.h"
#include "Dispatcher.h"

#include "net.h"

namespace reactor
{
    // 声明类, 解决循环调用
    class MainDispatcher;
    class ThreadDispatcher;


    class Server
    {
    public:
        Server(uint16_t port);
        
        void start();
        
        void setOnMsgCallBack(const onMessageCallBack& cb) { _cb = cb; }

        void setOnConnectionCallBack(const onConnectionCallBack& cb) { _con_cb = cb; }

        bool isConnectionCallBackAvailable() { return _con_cb.operator bool(); }

        onConnectionCallBack& connectionCallBack() { return _con_cb; }

        size_t userCount() { return Connection::getUserCount(); }// 得到当前用户连接的个数
    private:
        onMessageCallBack _cb;
        onConnectionCallBack _con_cb;
        uint16_t _port;
        ServerSocket _sock;
        std::unique_ptr<MainDispatcher> _main_dispacher;
        std::vector<std::shared_ptr<ThreadDispatcher>> _thread_dispatchers;
    };
}
