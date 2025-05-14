#include "../include/Server.h"

int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        std::cout << "Usage Input:" << argc << std::endl;
        ::exit(1);
    }
    reactor::Server server(std::stoi(argv[1]));

    auto on_msg_handler = [](reactor::Connection::Ptr con, std::string& msg)
    {
        std::cout << "收到了:" << msg << std::endl;
    };

    server.setOnMsgCallBack(on_msg_handler);

    server.start();
    return 0;
}