#include <iostream>
#include <functional>
namespace epollServerModule
{
    class epollServer; // forward declare
}
namespace ConnectionModule
{
    
    using connection_func_t = std::function<void(void)>;

    
    class Connection
    {
    public:
        
        Connection(int confd):_confd(confd)
        {}

        void initCallFunc(connection_func_t recv,connection_func_t sendto,connection_func_t handler_err)
        {
            _recv = recv;
            _sendto = sendto;
            _handler_err = handler_err;
        }

        void recv() { _recv; }
        void sendto() { _sendto(); }
        void handlerError() { _handler_err; }

        auto* belongLink() // 表示我所属的连接
        {
            return _self;
        }
        auto Fd() { return _confd; }
        std::string& buffer() { return _buffer; }
    private:
        // 通信相关的文件描述符
        int _confd;
        // 上层的回调函数,通过上层设置,本质为了实现connection的多态
        connection_func_t _recv; // 上层读

        connection_func_t _sendto; // 上层写

        connection_func_t _handler_err; // 上层处理异常

        epollServerModule::epollServer* _self; // 回调指针指向所属的s4erver

        std::string _buffer; // 读取出来的缓冲区


    };
}