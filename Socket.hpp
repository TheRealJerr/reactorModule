// 这个类的目的是想要利用多态实现TCP和UDP的统一

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
// 定义套接字的类
// 用于在传输层协议的使用，我们可以通过这个类直接创建一个tcp或者udp的套接字,抹平底层的差异
#include "/home/hrj/include/Log.hpp"

#include "./SockAddr.hpp"

namespace SocketModule
{
    using namespace SockAddrModule;

    #define SOCKETERROR

#ifdef SOCKETERROR
    enum 
    {
        CREATE_SOCK_ERROR,
        BIND_SOCK_ERROR,
    };
#endif

    class Socket
    {
    public:
        Socket(uint16_t port):_port(port)
        {}
        uint16_t Port() const { return _port; }
        // 创建套接字
        virtual bool createSocket() = 0;
        // 绑定套接字
        virtual bool bindSocket() = 0;
        // 监听套接字
        virtual bool listenSocket() = 0;
        // Accept三次握手，四次挥手
        virtual int accept() = 0;
        // 接受消息
        bool createTCPSocket()
        {
            return createSocket() \
                && bindSocket()  \
                && listenSocket();
        }
        // 构建UDP套接字
        bool createUDPSocket()
        {
            return createSocket() \
                && bindSocket(); 
        }
        // 远端用户的信息
        virtual std::string getPeerInfo() const = 0;
        virtual int getSocket() const = 0;
        bool closeSock(int sockfd)
        {
            int n = ::close(sockfd);
            if(n < 0)
            {
                LOG(LogLevel::ERROR) << "close fail:" << strerror(errno);
                return false;
            }
            return true;
        }
    private:
        uint16_t _port;
    };

    class UDPSocket : public Socket
    {
    public:
        UDPSocket(uint16_t port):Socket(port),_peer(port)
        {};
        // 创建套接字
        virtual bool createSocket() override
        {
            int sockfd = ::socket(AF_INET,SOCK_DGRAM,0);
            if(sockfd < 0)
            {
                perror("SOCKET ERROR");
                ::exit(CREATE_SOCK_ERROR);
            }   
            _sockfd = sockfd;
            return true;
        }
        // 绑定套接字
        virtual bool bindSocket() override
        {
            SockAddr sar;
            // 构建sockaddr
            sar.InitSockAddr(Port());
            return Bind(sar,_sockfd);
        }
        // 监听套接字
        virtual bool listenSocket() { return false; }; // UDP不需要实现这个功能
        virtual int accept() { return -1; }; // UDP不需要这个功能
        
        virtual int getSocket() const { return _sockfd; }
    private:
        int _sockfd;  // 套接字
        SockAddr _peer; // 用户的信息
    };

    class TCPSocket : public Socket
    {
    public:
        TCPSocket(int port):Socket(port),_local(port)
        {}
        virtual bool createSocket()
        {
            int sockfd = ::socket(AF_INET,SOCK_STREAM,0); // 流式传输
            if(sockfd < 0)
            {
                LOG(LogLevel::FATAL) << "create socket error:" << strerror(errno);
                return false;
            }
            else LOG(LogLevel::DEBUG) << "create successfully";
            _sockfd = sockfd;
            return true;
        }
        // 绑定套接字
        virtual bool bindSocket()
        {
            // 将本地信息绑定
            return Bind(_local,_sockfd);
        }
        // 监听套接字
        virtual bool listenSocket()
        {
            // 将我们绑定的套接字进行监听
            return Listen(_sockfd);
        
        }
        // Accept三次握手，四次挥手
        virtual int accept()
        {
            _sock_communicate = ::accept(_sockfd,_peer.Convert(),&_peer.SockAddrLen());
            if(_sock_communicate < 0)
            {
                LOG(LogLevel::FATAL) << "accept fail:" << strerror(errno);
                return false;
            }
            else LOG(LogLevel::DEBUG) << "accept sucessfully";
            return _sock_communicate;
        }
        
        std::string getPeerInfo() const 
        {
            return _peer.getSockAddrInfo();
        } 

        virtual int getSocket() const { return _sockfd; }
    private:
        SockAddr _local;
        SockAddr _peer;
        int _sockfd;
        int _sock_communicate;
    };
}
