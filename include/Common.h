#pragma once
#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <memory>
#include "Log.h"


namespace reactor
{
#define IP_SIZE 24
    // 组织struct sockaddr_in

    // 1. 能够直接通过Sockaddr构建一个struct sockaddr_in
    // 2. 能够通过收到的Sockaddr组织一个SockAddr

    class SockAddrInfo
    {
    public:
        SockAddrInfo(uint16_t port) : 
             _port(port)
        {}

        struct sockaddr_in * createSockAddrIn()
        {
            _sock = std::make_unique<sockaddr_in>();
            _sock->sin_addr.s_addr = INADDR_ANY;
            _sock->sin_family = AF_INET;
            _sock->sin_port = ::htons(_port);

            return _sock.get();
        }
        // 收到对端的信息并且初始化自身
        void acceptPeerInfo(struct sockaddr_in* peer)
        { 
            char ip_str[IP_SIZE] = { 0 };
            _ip = ::inet_ntop(AF_INET,peer,ip_str,IP_SIZE);
            _port = ::ntohs(peer->sin_port);
        }

        const std::string ip() const { return _ip; }

        uint16_t port() const { return _port; }

        socklen_t sockLen() const { return sizeof(*_sock); }
    private:
        std::string _ip;
        uint16_t _port;

        std::unique_ptr<sockaddr_in> _sock;
    };
}