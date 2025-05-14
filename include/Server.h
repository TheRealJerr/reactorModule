#pragma once
#include "Common.h"
#include "Dispatcher.h"

#include "net.h"

namespace reactor
{

    class Server
    {
    public:
        Server(uint16_t port);
        
        void start();
        
        void setOnMsgCallBack(const onMessageCallBack& cb) { _cb = cb; }
    private:
        onMessageCallBack _cb;
        uint16_t _port;
        ServerSocket _sock;
        MainDispatcher _main_dispacher;
        std::vector<ThreadDispatcher::Ptr> _thread_dispatchers;
    };
}
