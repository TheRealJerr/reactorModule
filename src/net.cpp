#pragma once

#include "../include/Common.h"
#include "../include/net.h"

namespace reactor
{
    void setSockOpt() {}

    void TcpSocket::createSocket()
    {
        _sock_listen = ::socket(AF_INET,SOCK_STREAM,0);
        if(_sock_listen < 0)
        {
            log(LogLevel::ERROR) << "套接字创建失败";
            ::exit(1);
        }
    }

    void TcpSocket::bindSocket()
    {
        // 将套接字进行bind
        // struct sockaddr_in sock;
        // sock.sin_addr.s_addr = INADDR_ANY;
        // sock.sin_family = AF_INET;
        // sock.sin_port = ::htons(_port);
        SockAddrInfo sock_info(_port);
        if(::bind(_sock_listen,SOCKINTOSOCK(sock_info.createSockAddrIn()),sock_info.sockLen()) < 0)
        {
            log(LogLevel::ERROR) << "bind失败";
            ::exit(1);
        }

        log(LogLevel::DEBUG) << "bind成功";
    }

    void TcpSocket::listen(int backlog)
    {
        if(_sock_listen < 0)
        {
            log(LogLevel::ERROR) << "error sock";
            ::exit(1);
        }

        int n = ::listen(_sock_listen, backlog);
        if(n < 0)
        {
            log(LogLevel::ERROR) << "listen error";
            ::exit(1);
        }

        log(LogLevel::DEBUG) << "listen成功";
    }
    Connection::Ptr TcpSocket::accept(SockAddrInfo* peer_info)
    {
        struct sockaddr_in peer;
        socklen_t peer_len;
        int sock_communicate = ::accept(_sock_listen,SOCKINTOSOCK(&peer),&peer_len);
        if(sock_communicate < 0)
        {
            log(LogLevel::ERROR) << "accept失败";
            return nullptr;
        }
        else log(LogLevel::DEBUG) << "accept成功";
        // 拿到了communicate 
        peer_info->acceptPeerInfo(&peer);
        // 拿到了peer的信息传递出去
        // 我们返回一个通信连接

        return std::make_shared<Connection>(sock_communicate);
    }

    // 后续将改成我们自己定义的msg类型
    void Connection::sendMsg(const std::string& msg)
    {
        // 将数据直接进行发送

        int n = ::send(_sock_communicate,msg.c_str(),msg.size(),0);
        if(n < 0) log(LogLevel::ERROR) << "发送失败";
        else log(LogLevel::INFO) << "发送成功";
    }
    // 关闭通信连接
    void Connection::shutDown()
    {
        ::close(_sock_communicate);
    }
}