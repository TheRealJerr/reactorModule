#include "../include/Dispatcher.h"
#include "../include/Log.h"
namespace reactor
{
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
            struct epoll_event* events;
            int n;
            if(_epoll.wait(events, &n))
            {
                // 如果是readfd成功就进行读取，其他就进行写入
                log(LogLevel::DEBUG) << "epoll模型等待成功";
                int eventfd = events[n].data.fd;
                if(eventfd == _readfd)
                    readReady();
                else 
                    eventReady(eventfd);
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
        // 添加进入epoll模型
        _epoll.addEvent(neweventfd, EPOLLIN);
        // 构造新的通信连接
        Connection::Ptr con = std::make_shared<Connection>(neweventfd);
        // 添加进入hash中
        _cons.insert(std::make_pair(neweventfd,con));
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
        if(_cb) _cb(con,buffer);
    }

    
}