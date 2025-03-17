#include <iostream>
#include <Log.hpp>
#include "ListenConnection.hpp"
#include "reactorServer.hpp"
#include "IOConnection.hpp"

using namespace ListenConnection;
using namespace IOConnectionModule;

void Usage(int n)
{
    if(n != 2)
        std::cout << "Usage Input: need port number" << std::endl;
}
int main(int argc,char* argv[])
{
    if(argc != 2) Usage(argc);

    int port = std::stoi(argv[1]);
     // 创建监听套接字
    connection_ptr listen_connection = std::make_shared<ListenConnection::ListenConnection>(port);
    //  // 注册监听套接字到epoll
    //  _epoll.addNewFd(listen_connection->getFd(), EPOLLIN | EPOLLET);
    //  // 保存监听套接字
    //  _connections.insert(std::make_pair(listen_connection->getFd(), listen_connection));
    //  //
    std::unique_ptr<ReactorServer> server = std::make_unique<ReactorServer>(port);
    
    listen_connection->setEvents(EPOLLIN);
    listen_connection->setOwner(server.get());
    LOG(LogLevel::INFO) << "ReactorServer start listen on port " << port;
    server->addNewConnection(listen_connection);

    server->run();
    return 0;
}