#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include "/home/hrj/include/Log.hpp"

#define DEFAULT_BUFFER_SIZE 4096 // 默认给buffer的大小是4096

enum
{
    SOCK_ERR,
    BIND_ERR,
    WRONG_SOCK,
    LISTEN_ERR,
    ACCEPT_ERR,
};

namespace SockAddrModule
{
    // this module is for use a class to conclude sockaddr to help us to bind or other important function
    using namespace LogModule;

#define DEFAULT_IP_SIZE 1024
#define LISTEN_SIZE 8
    // this interface class have two function
    // 1. if you pass by sockaddr_in,it can analyse and fill the ip and the port
    // 2. if you just pass by port, it can also fill the ip and the port, it can help you to fill the sockaddr_in which may be you want
    class SockAddr
    {
    public:
        SockAddr():_len(sizeof(_sockaddr)){
            ::memset(&_sockaddr,0,sizeof(_sockaddr));
        }
        SockAddr(uint16_t port)
        {
            InitSockAddr(port);
        }

        SockAddr(const struct sockaddr_in & sockaddr)
        {
            InitSockAddr(sockaddr);
        }

        // 通过接受到的sockaddr_in构建本地的Sockaddr
        void InitSockAddr(const struct sockaddr_in &sockaddr)
        {
            _sockaddr = sockaddr;
            _port = ::ntohs(sockaddr.sin_port);
            char buffer[DEFAULT_IP_SIZE] = {0};
            const char *ip = ::inet_ntop(AF_INET, &sockaddr, buffer, sizeof(buffer));
            // On success, inet_ntop() returns a non-null pointer to dst.  NULL is returned if there was an error, with errno set to indicate the error.
            if (ip == nullptr)
            {
                LOG(LogLevel::FATAL) << "inet to host fail";
                ::exit(1);
            }
            //
            _ip = buffer;
            _len = sizeof(_sockaddr);
        }
        // 
        void InitSockAddr(uint16_t port)
        {
            _port = port;
            _ip = "";

            // fill the sockaddr
            ::memset(&_sockaddr, 0, sizeof(_sockaddr));
            _sockaddr.sin_family = AF_INET;
            _sockaddr.sin_port = ::htons(_port);
            _sockaddr.sin_addr.s_addr = INADDR_ANY;

            _len = sizeof(_sockaddr);
        }

        std::string Ip() const { return _ip; }
        uint16_t Port() const { return _port; }
        struct sockaddr *Convert() const { return (struct sockaddr *)(&_sockaddr); }
        struct sockaddr_in& Content() { return _sockaddr; }
        socklen_t& SockAddrLen() { return _len; }
        std::string getSockAddrInfo() const { return _ip + " : " + std::to_string(_port); }
        
    private:
        struct sockaddr_in _sockaddr;
        uint16_t _port;
        std::string _ip;
        socklen_t _len;
    };

    bool Bind(SockAddr &sockaddr, int sockfd)
    {
        int n = ::bind(sockfd, sockaddr.Convert(), sockaddr.SockAddrLen());
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind error:" << strerror(errno);
            ::exit(BIND_ERR);
        }
        return true;
    }

    bool Listen(int sockfd)
    {
        int n = ::listen(sockfd,LISTEN_SIZE);
        if (n < 0)
        {
            LOG(LogLevel::ERROR) << "listen fail:" << strerror(errno);
            ::exit(LISTEN_ERR);
        }
        LOG(LogLevel::DEBUG) << "sockfd : " << sockfd;
        LOG(LogLevel::DEBUG) << "listen successful";
        return true; // 监听成功
    }

    
}