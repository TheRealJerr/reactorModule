#pragma once 

#include "./Connection.hpp"
#include "reactorServer.hpp"
#include <memory>
#include "IOConnection.hpp"
// listenconnecntion 是对connection的继承,recv负责接收listen中的task,并且将listen成功的数据放入reactor模型中
namespace ListenConnection
{
    using namespace Connection;
    class ListenConnection : public Connection
    {
    public:
        ListenConnection(int port) : Connection(-1) , _port(port)
        {
            // 创建套接字bind
            int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
            if(sockfd < 0)
            {
               LOG(LogLevel::FATAL) << "create socket error";
               exit(1);
            }
            // 监听套接字listen
            struct sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(_port);
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
            if(::bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
            {
                LOG(LogLevel::FATAL) << "bind socket error";
                exit(1);
            }
            if(::listen(sockfd, 1024) < 0)
            {
                LOG(LogLevel::FATAL) << "listen socket error";
                exit(1);
            }
            setFd(sockfd);
            NON_BLOCKING(sockfd);//将监听的套接字设置成非阻塞
        }
        
        virtual void recv() override
        {
            // 接收listen中的task
            // listen套接字度读就绪
            while(true)
            {
                struct sockaddr_in peeraddr;
                socklen_t peerlen = sizeof(peeraddr);
                int newfd = ::accept(getFd(), (struct sockaddr*)&peeraddr, &peerlen);
                if(newfd > 0)
                {
                    // 将新的连接封装填写进入reactor模型中
                    NON_BLOCKING(newfd); // 设置新的连接为非阻塞
                    auto con = std::make_shared<IOConnectionModule::IOConnection>(newfd);
                    // 设置读取2025年3月17日12:14:09
                    con->setEvents(EPOLLIN);
                    con->setOwner(getOwner());
                    // 添加进入
                    getOwner()->addNewConnection(con);
                    LOG(LogLevel::INFO) << "new connection " << newfd;
                }

                else
                {
                    // 由于这里是非阻塞读取，所以要通过errnor判断是否是因为没有数据可读
                    if(errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        LOG(LogLevel::DEBUG) << "no data to read";
                        break;
                    }
                    else if(errno == EINTR)
                    {
                        LOG(LogLevel::FATAL) << "accept breack by signam";
                        break;
                    }
                    else 
                    {
                        LOG(LogLevel::FATAL) << "accept error";
                        break;
                    }
                }
            }
            std::cout << "listen recv end" << std::endl;
        }
        // listen套接字没有写的环节
        virtual void send() override {}

        virtual void handlerError() override
        {
            // 处理IO错误
            // 将本链接从server中移除
            getOwner()->delConnection(getFd());
            close(); // 直接关闭连接
            perror("Listen error");
        }
    private:
        int _port;
    };
}