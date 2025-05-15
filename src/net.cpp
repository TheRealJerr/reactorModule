
#include "../include/Common.h"
#include "../include/net.h"

namespace reactor
{
    void setSockOpt() 
    {
        
    }

    void ServerSocket::createSocket()
    {
        _sock_listen = ::socket(AF_INET,SOCK_STREAM,0);
        if(_sock_listen < 0)
        {
            log(LogLevel::ERROR) << "套接字创建失败";
            ::exit(1);
        }
    }

    void ServerSocket::bindSocket()
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

    void ServerSocket::listen(int backlog)
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
    Connection::Ptr ServerSocket::accept(SockAddrInfo* peer_info)
    {
        struct sockaddr_in peer;
        socklen_t peer_len = 0;
        int sock_communicate = ::accept(_sock_listen,SOCKINTOSOCK(&peer),&peer_len);
        if(sock_communicate < 0)
        {
            log(LogLevel::ERROR) << "accept失败" << strerror(errno);
            return nullptr;
        }
        else 
        {
            peer_info->acceptPeerInfo(&peer);
            log(LogLevel::DEBUG) << "ip:" << peer_info->ip() \
            << " port:" << peer_info->port() << " accept成功";
        }
        log(LogLevel::DEBUG) << "得到了通信套接字:" << sock_communicate;
        // 拿到了communicate 
        // 拿到了peer的信息传递出去
        // 我们返回一个通信连接

        return std::make_shared<Connection>(sock_communicate);
    }

    // 后续将改成我们自己定义的msg类型
    void Connection::sendMsg(const std::string& msg)
    {
        // 将数据直接进行发送
        log(LogLevel::DEBUG) << "通过" << _sock_communicate << "发送数据";
        int n = ::send(_sock_communicate,msg.c_str(),msg.size(),0);
        if(n < 0) log(LogLevel::ERROR) << "发送失败 " << strerror(errno);
        else log(LogLevel::INFO) << "发送成功";
    }
    // 关闭通信连接
    void Connection::shutDown()
    {
        if(_is_close == false)
        {
             log(LogLevel::DEBUG) << "关闭了" << _sock_communicate << "套接字";
            ::close(_sock_communicate);
        }
    }

    void ClientSocket::createSocket()
    {
        _sockfd = ::socket(AF_INET,SOCK_STREAM,0);
        if(_sockfd < 0)
        {
            log(LogLevel::ERROR) << "套接字创建失败";
            ::exit(1);
        }
        int optval = 1;
        // 由于我们现在是debug阶段，直接将socket设置成REUSE模式
        setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    }


    void Connection::readMsg(std::string* msg)
    {
        log(LogLevel::DEBUG) << "通过" << _sock_communicate << "读取数据";
        char buffer[READ_MAX_SIZE] = { 0 };
        while(true)
        {
            int n = ::recv(_sock_communicate,buffer,READ_MAX_SIZE - 1,0);
            if(n == EAGAIN || n == EWOULDBLOCK)
            {
                log(LogLevel::ERROR) << "读取完成";
                return;
            }
            else if(n == 0) 
            {
                shutDown(); // 关闭连接
                // 
                log(LogLevel::INFO) << "对端关闭了连接";
            }
            else if(n > 0)
            {
                buffer[n] = 0;
                *msg += buffer;
            }
            else
            {
                // n < 0
                // 读取错误
                log(LogLevel::ERROR) << "recv error";
                return;
            }
        }
    }
    Connection::Ptr ClientSocket::connect()
    {
        // 进行连接
        SockAddrInfo sock(_ip,_port);
        // 直接进行connect
        int n = ::connect(_sockfd,SOCKINTOSOCK(sock.createSockAddrIn()),sock.sockLen());
        if(n < 0)
        {
            log(LogLevel::ERROR) << "connect " << _ip << ":" << _port << " error " << strerror(errno);
            return nullptr;
        }
        else log(LogLevel::DEBUG) << "connect成功"; 
        return std::make_shared<Connection>(_sockfd);
    }

    void ServerSocket::start()
    {
        // 主线程
    }
    
    int Accept(SockAddrInfo* info,int sockfd)
    {
        struct sockaddr_in peer;
        socklen_t peer_len = 0;
        int sock_communicate = ::accept(sockfd,SOCKINTOSOCK(&peer),&peer_len);
        if(sock_communicate < 0)
        {
            log(LogLevel::ERROR) << "accept失败" << strerror(errno);
            return -1;
        }
        else 
        {
            if(info) info->acceptPeerInfo(&peer);
        }
        log(LogLevel::DEBUG) << "得到了通信套接字:" << sock_communicate;

        return sock_communicate;
    }

}