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
#include "Common.h"
#include <functional>
#include <atomic>
#include <sys/epoll.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <future>
#include <fcntl.h>
#include <unordered_map>

namespace reactor
{
// ip字符串的默认长度
#define IP_SIZE 24
// 一次性从套接字中读取的最大的长度
#define READ_MAX_SIZE 1024 
    // 组织struct sockaddr_in

    // 1. 能够直接通过Sockaddr构建一个struct sockaddr_in
    // 2. 能够通过收到的Sockaddr组织一个SockAddr

    class SockAddrInfo
    {
    public:

        SockAddrInfo() = default;
        SockAddrInfo(uint16_t port) : 
             _port(port)
        {}

        SockAddrInfo(const std::string& ip,uint16_t port) : 
            _ip(ip), _port(port)
        {}
        
        struct sockaddr_in* createSockAddrIn()
        {
            _sock = std::make_unique<struct sockaddr_in>();
            memset(_sock.get(), 0, sizeof(struct sockaddr_in));  // 清空结构体
            
            _sock->sin_family = AF_INET;
            _sock->sin_port = htons(_port);
            
            if(_ip.empty()) {
                _sock->sin_addr.s_addr = INADDR_ANY;
            } else {
                if(inet_pton(AF_INET, _ip.c_str(), &(_sock->sin_addr)) != 1) {
                    // 错误处理
                    log(LogLevel::FATAL) << "Invalid IP address format";
                    return nullptr;
                }
            }
            
            return _sock.get();  // 注意调用者不应长期持有此指针
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

        socklen_t sockLen() const { return sizeof(struct sockaddr_in); }
    private:
        std::string _ip;
        uint16_t _port;

        std::unique_ptr<sockaddr_in> _sock;
    };


    inline void setNonBlock(int fd)
    {
        if(fd < 0)
        {
            log(LogLevel::ERROR) << "Error fd:" << fd;
            return;
        }
        int fl = ::fcntl(fd,F_GETFL,0);
        if(fl < 0)
        {
            log(LogLevel::ERROR) << "fnctl error:" << strerror(errno);
            return;
        }
        log(LogLevel::INFO) << fd << "设置非阻塞成功";
        ::fcntl(fd,F_SETFL,fl | O_NONBLOCK);
    }
}