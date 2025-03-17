#pragma once
#include <iostream>
#include <functional>
#include "Connection.hpp"
#include "reactorServer.hpp"
#define BUFFER_SIZE 1024
namespace IOConnectionModule
{
    using namespace Connection;
    using handler_t = std::function<std::string(std::string &)>;

    class IOConnection : public Connection
    {
    public:
        IOConnection(int fd) : Connection(fd)
        {
            // 现在默认不处理
            _handler = [](std::string &str) -> std::string
            {
                std::string ret = str;
                str.clear();
                return ret;
            };
        }

        virtual void recv() override
        {
            // 在listenConnection中，
            // 我们保证了sockfd是非阻塞的
            // 所以这里不用再判断是否可读
            char buffer[BUFFER_SIZE];
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            while (true)
            {
                int n = ::recv(getFd(), buffer, BUFFER_SIZE, 0);
                if (n > 0)
                {
                    buffer[n] = 0;
                    
                    _recv_buffer.append(buffer);
                }
                else if (n == 0)
                {
                    // 对端关闭连接
                    // 处理关闭连接
                    LOG(LogLevel::INFO) << "对端关闭连接";
                    handlerError();
                    return;
                }
                else
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        // do nothing and wait for next recv
                        break;
                    }
                    else
                    {
                        // error
                        handlerError();
                        return;
                    }
                }
            }
            // 处理接收到的数据
            // 调用handler处理
            LOG(LogLevel::DEBUG) << "一次读取成功";
            _send_buffer = _handler(_recv_buffer);
            getOwner()->enableEvent(getFd(), true, true);
        }

        virtual void handlerError() override
        {
            // 处理IO错误
            // 将本链接从server中移除
            getOwner()->delConnection(getFd());
            close(); // 直接关闭连接
        }    
        virtual void send() override
        {
            // 发送数据、
            while (true)
            {
                int n = ::send(getFd(), _send_buffer.c_str(), _send_buffer.size(), 0);
                if (n > 0)
                {
                    // 发送成功
                    _send_buffer.erase(0, n);
                }
                else if (n == 0)
                {
                    // 对端关闭连接
                    //  处理关闭连接
                    // handlerError();
                    // 没得写了
                    break;
                }
                else
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        // do nothing and wait for next send
                        // 缓冲区写满了,下次再来
                        break;
                    }
                    else
                    {
                        // error
                        handlerError();
                        break;
                    }
                }
            }
            // 发送完毕
            getOwner()->enableEvent(getFd(), true, false);
        }

    private:
        std::string _recv_buffer;

        // 处理接收到的字符串
        handler_t _handler;
        std::string _send_buffer;
    };
}