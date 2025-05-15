#include "../include/Server.h"

namespace reactor
{
    Server::Server(uint16_t port) : 
        _port(port) , _sock(port), _main_dispacher(std::make_unique<MainDispatcher>(_sock.socket()))
    {}

    void Server::start()
    {
        int thread_size = THREAD_SIZE;
        for(int i = 0;i < thread_size;i++)
        {
            int readfd = _main_dispacher->addNewListener();
            ThreadDispatcher::Ptr ptd = std::make_shared<ThreadDispatcher>(readfd);
            _thread_dispatchers.push_back(ptd);
            ptd->setOnMsgCallBack(_cb);
            ptd->setOnConnectionCallBack(_con_cb);
            // 给线程池注册任务
            thread_task_t newtask = [ptd]()->void 
            {
                ptd->run();
            };
            // 将任务注册进入线程池
            ThreadPool::getInstance()->addNewTask(newtask);
        }
        _main_dispacher->run();
    }
}