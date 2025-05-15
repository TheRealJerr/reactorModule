#include "../include/Server.h"

int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage input: " << argc << std::endl;
        ::exit(1);
    }
    reactor::Server server(std::stoi(argv[1]));

    auto on_msg_handler = [](reactor::Connection::Ptr con, std::string& msg)
    {
        std::cout << "收到了:" << msg << std::endl;
        con->sendMsg("服务端成功收到了请求");
    };
    // 连接建立成功的处理的回调函数
    auto on_con_handler = [](reactor::Connection::Ptr con)
    {
        std::cout << "连接建立成功" << std::endl;
    };
    server.setOnMsgCallBack(on_msg_handler);
    server.setOnConnectionCallBack(on_con_handler);
    server.start();
    return 0;
}