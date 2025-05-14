#pragma once
#include "Common.h"

// 定义线程池模块
// 针对一组线程进行组织
// 通过单例模式实现线程池
namespace reactor
{
#define THREAD_SIZE 4 // 默认开四个线程

    using thread_task_t = std::function<void(void)>;

    // 想要拿到返回值
    class ThreadPool    
    {
        // 现成默认执行的工作
        static void threadTask(ThreadPool* self)
        {
           // 首先保证没有任务其次才是退出

           
            while(true)
            {
                std::unique_lock<std::mutex> lock(self->_mtx);
                // 没有任务并且不运行了, 线程就可以退出了
                if(self->_tasks.size() == 0 && self->_is_run == false) break;

                // 还有任务或者正在运行
                if(self->_tasks.size() > 0)
                {
                    auto task = self->_tasks.front();
                    self->_tasks.pop();
                    lock.unlock();
                    // 
                    task();
                }

                else 
                {
                    // 没有任务, 并且还在工作
                    self->_sleep_thread_size++;
                    self->_cond.wait(lock);
                    self->_sleep_thread_size--;
                }
            }
        }


    public:
        static std::unique_ptr<ThreadPool>& getInstance()
        {
            if(_self.get() == nullptr)
                _self = std::make_unique<ThreadPool>();
            return _self;
        }
        
        static void setThreadSize(size_t size) { _thread_size = size; }
        // 定义线程的个数
        ThreadPool() : 
            _is_run(true)
        {
            _threads.resize(_thread_size);

            for(auto& thread : _threads)
                thread = std::thread(threadTask,this);
        }

        void stop()
        {
            // 只有正在运行，才要停止
            if(_is_run == true)
            {
                // 
                _is_run = false;
                if(_sleep_thread_size) _cond.notify_all();
            }

            for(auto& thread : _threads)
                if(thread.joinable())
                {
                    log(LogLevel::DEBUG) << "线程合并成功";
                    thread.join();
                }
                
        }

        template <class Func,class ...Args>
        void addNewTask(Func&& func,Args&&... args)
        {
            std::unique_lock<std::mutex> lokc(_mtx);
            auto task = std::bind(std::forward<Func>(func),std::forward<Args>(args)...);
            _tasks.push(std::move(task));

            if(_sleep_thread_size) _cond.notify_one(); // 唤醒沉睡的线程
        }

        ~ThreadPool() { stop(); }
        
    private:

        // 现成的阻塞任务队列
        std::queue<thread_task_t> _tasks;        

        std::mutex _mtx;// 保护任务队列
        std::vector<std::thread> _threads;
        static inline size_t _thread_size = THREAD_SIZE;

        std::condition_variable _cond; // 当任务为零的时候, 条件管理线程
        std::atomic<bool> _is_run;

        size_t _sleep_thread_size; // 记录沉睡线程的个数

        // 懒汉模式

        static inline std::unique_ptr<ThreadPool> _self = nullptr; 
    };

}