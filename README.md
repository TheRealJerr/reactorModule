# reactor反应堆模式

这是通过epoll(多路转接) + 线程池 + Redis + MySql实现的高并发服务器框架

> create by hrj on 2025/5/11

核心思想

## 服务端

服务端主线程监听套接字监听到的套接字信息通过管道基于RR轮转派发给各个子线程, 每个子线程私有一份epoll模型, epoll模型管理管道的读端和通信套接字。


## 客户端

客户端分为两个线程, 发送线程和接受线程, 用户可以通过Connection模块发送信息, 同时可以设置异步消息处理回调, 设置回调完成之后, 当客户端接收到消息Client会默认通过设置好的回调函数处理接收到的消息, 如果没有设置回调函数, 消息将被丢弃。


## 代码示例

```cpp
server:

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
    // 设置消息到来的回调函数
    server.setOnMsgCallBack(on_msg_handler);

    server.start();
    return 0;
}

```

```cpp
client:

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
```

## 引用

本项目参考了陈硕的Moduo库, 具体实现细节不同, 但是思想一致, 都是基于one thread one loop的高效处理事件

## 版本

**version 1.0:**

目前版本处于1.0, 内部消息采用的原生的std::string, 同时还没有设置connection连接成功的时候的对应回调接口。




