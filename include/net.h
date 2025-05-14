#pragma once
#include "Common.h"
#include "Log.h"


namespace reactor
{
#define SOCKINTOSOCK(PTR) (struct sockaddr*)(PTR) 

    // 第一版,没有涉及自己的string类型,直接使用std::string

    // 针对连接进行组织
    // 通信连接可以发送数据, 也可以接受数据
    class Connection
    {
    public:
        using Ptr = std::shared_ptr<Connection>;
        Connection(int sock_communicate) : _sock_communicate(sock_communicate)
        {
            setNonBlock(_sock_communicate);
        }

        void sendMsg(const std::string& msg);

        void readMsg(std::string* msg);
        
        void shutDown();

        bool isConnected() const { return _sock_communicate >= 0; }

        ~Connection() { if(_is_close == false) shutDown(); }

    private:
        int _sock_communicate;
        bool _is_close = false;
    };

    using onMessageCallBack = std::function<void(Connection::Ptr,std::string&)>;

    class ServerSocket
    {
    private:
        void setSockOpt() {}

        void createSocket();

        void bindSocket();

        void listen(int backlog);

    public:

        ServerSocket(uint16_t port,int backlog = 8) : _port(port)
        {
            setSockOpt(); // 设置socket创建的选项

            createSocket(); // 创建套接字

            bindSocket(); // bind套接字

            listen(backlog); // 监听套接字
        }
        uint16_t port() const { return _port; }

        int socket() const { return _sock_listen; }
        
        Connection::Ptr accept(SockAddrInfo* );

        void setOnMsgCallBack(const onMessageCallBack& cb) { _on_msg_cb = cb; }

        void start();
    private:
        uint16_t _port;
        int _sock_listen;
        std::atomic<bool> _is_run;
        onMessageCallBack _on_msg_cb;
    };
    
    

    // 作为服务端需要对服务基础进行组织
    class ClientSocket
    {
    private:
        void createSocket();
        // 服务端不需要bind
    public:
        Connection::Ptr connect();

        ClientSocket(const std::string& ip, uint16_t port) : 
            _ip(ip), _port(port), _is_run(false)
        {
            createSocket();
        }

        void start();

        void setOnMsgCallBack(const onMessageCallBack& cb)
        {
            _on_msg_cb = cb;
        }

        uint16_t port() const { return _port; }
        int socket() const { return _sockfd; }

    private:
        
        const std::string _ip;
        uint16_t _port;
        std::atomic<bool> _is_run;
        int _sockfd;

        onMessageCallBack _on_msg_cb;
    };
    
    // 全局钉钉accept函数

    int Accept(SockAddrInfo* info, int sockfd);

}