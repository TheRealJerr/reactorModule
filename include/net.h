#pragma once
#include "Common.h"
#include "Log.h"


namespace reactor
{
#define SOCKINTOSOCK(PTR) (struct sockaddr*)(PTR) 

    class Connection;

    class TcpSocket
    {
    private:
        void setSockOpt() {}

        void createSocket();

        void bindSocket();

        void listen(int backlog);


    public:

        TcpSocket(uint16_t port,int backlog = 8) : _port(port)
        {
            setSockOpt(); // 设置socket创建的选项

            createSocket(); // 创建套接字

            bindSocket(); // bind套接字

            listen(backlog); // 监听套接字
        }
        
        
        Connection::Ptr accept(SockAddrInfo* );


    private:
        uint16_t _port;
        int _sock_listen;
    };
    
    // 针对连接进行组织
    class Connection
    {
    public:
        using Ptr = std::shared_ptr<Connection>;
        Connection(int sock_communicate) : _sock_communicate(sock_communicate)
        {}

        void sendMsg(const std::string& msg);

        void shutDown();

        bool isConnected() const { return _sock_communicate >= 0; }
    private:
        int _sock_communicate;
    };


}