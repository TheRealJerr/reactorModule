#include "../include/Client.h"

int main(int argc,char* argv[])
{
    // 默认进行本机环回

    if(argc != 2)
    {
        std::cout << "Usage Input:" << argc << std::endl;
        ::exit(1);
    }
    auto async_msg_cb = [](reactor::Connection::Ptr con,std::string& msg)
    {
        // 
        std::cout << "客户端收到了消息:" << msg << std::endl;
    };
    reactor::Client::setOnMsgCallBack(async_msg_cb);
    reactor::Client client("127.0.0.1", std::stoi(argv[1]));
    auto con = client.getConnection(); // 得到通信连接
    while(true)
    {
        std::string buffer; 
        std::cout << "你想要输入的数据的是什么:";
        std::cin >> buffer;
        con->sendMsg(buffer);
    }
    
    return 0;
}