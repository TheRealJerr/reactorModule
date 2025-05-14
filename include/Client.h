#pragma once
#include "Common.h"
#include "net.h"
#include "ThreadPool.h"

namespace reactor
{

    class Client
    {
        // 一个独立的线程来处理异步的消息
        void thraedHandlerMsg();
    public:

        Client(const std::string& ip,uint16_t port) :
            _sock(ip,port), _con(_sock.connect())
        {
            // 得到通信连接
            _async_handler_msg = std::thread([this]()->void
                {
                    this->thraedHandlerMsg();
                } 
            );
        }   

        Connection::Ptr getConnection()
        {
            return _con; // 获取到通信连接
        }

        ~Client()
        {
            if(_async_handler_msg.joinable())
                _async_handler_msg.join();
        }
        // 我们单独构建一个线程处理onmsgcb
        static void setOnMsgCallBack(const onMessageCallBack& cb) { _cb = cb; }
    private:
        ClientSocket _sock; 
        static reactor::onMessageCallBack _cb; // 消息处理回调
        Connection::Ptr _con;
        std::thread _async_handler_msg;
    };
}