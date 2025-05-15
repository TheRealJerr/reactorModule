#include "../include/Dispatcher.h"
#include "../include/Log.h"
#include "../include/Server.h"

namespace reactor
{

    MainDispatcher::MainDispatcher(int listen_socket) : 
        _listen_socket(listen_socket)
        , _is_run(true)
    {}

    int MainDispatcher::addNewListener()
    {
        int pipefd[2] = { 0 };
        int n = ::pipe(pipefd);
        if(n < 0)
        {
            log(LogLevel::ERROR) << "pipe create fail" << strerror(errno);
            return -1;
        }
        log(LogLevel::DEBUG) << "添加新的管道成功";
        // 自己存储写端, 返回读端
        _readers.push_back(pipefd[1]);
        return pipefd[0];
    }
    void MainDispatcher::run()
    {
        if(_readers.size() == 0)
        {
            log(LogLevel::DEBUG) << "no reader";
            return;
        }
        //
        while(_is_run)
        {
            int sock_com = Accept(nullptr, _listen_socket);
            // 连接建立成功了
            // 通过轮询写入
            int n = ::write(_readers[_cur_pos], &sock_com, sizeof(int));
            if(n < 0)
            {
                log(LogLevel::DEBUG) << "轮询写入失败";
                continue;
            
            }
            //
            _cur_pos = (_cur_pos + 1) % _readers.size();
        }
    }

    void ThreadDispatcher::run()
    {
        // 通过
        while(_is_run)
        {
            struct epoll_event* events = nullptr;
            int n;
            if(_epoll.wait(events, &n))
            {
                // 如果是readfd成功就进行读取，其他就进行写入
                for(int i = 0;i < n;i++)
                {
                    int eventfd = events[i].data.fd;
                    if(eventfd == _readfd)
                        readReady();
                    else 
                        eventReady(eventfd);
                }
            }
        }
    }

    void ThreadDispatcher::readReady()
    {
        int neweventfd = 0;
        int n = ::read(_readfd,&neweventfd, sizeof(int));  // 读取四个字节
        if(n < 0)
        {
            log(LogLevel::ERROR) << "读取失败" << strerror(errno);
            return;
        }
        // 连接建立成功, 拿到了eventfd
        // 添加进入epoll模型
        _epoll.addEvent(neweventfd, EPOLLIN);
        // 构造新的通信连接
        Connection::Ptr con = std::make_shared<Connection>(neweventfd);
        // 服务端将通信连接设置成为非阻塞
        con->setNonBlock();
        // 添加进入hash中
        _cons.insert(std::make_pair(neweventfd,con));
        //
        if(_con_cb) _con_cb(con);
    }

    void ThreadDispatcher::eventReady(int eventfd)
    {
        // 事件就绪了
        if(_cons.count(eventfd) == 0)
        {
            log(LogLevel::ERROR) << "没有对应事件:" << eventfd;
            return;
        }
        auto con = _cons[eventfd];
        std::string buffer;
        con->readMsg(&buffer);

        // 读事件就绪，但是没有数据，说明对端关闭了连接
        if(buffer.empty())
        {
            // 我们首先管理连接
            con->shutDown();
            // 删除对应的管理信息
            _epoll.delEvent(eventfd, EPOLLIN);
            // 删除通信连接映射
            _cons.erase(eventfd);
        }
        //
        else if(_cb)
        {
            log(LogLevel::ERROR) << "有回调函数";
            _cb(con,buffer); // 有数据并且存在handler
        }
        else
            log(LogLevel::DEBUG) << "没有对应的回调函数";  
    }

    
}