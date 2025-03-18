#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <functional>
#include <condition_variable>
#include "/home/hrj/include/Log.hpp"
#include <vector>

namespace ThreadPool
{
    using namespace LogModule;
    using task_t = std::function<void()>;

    class ThreadPool
    {
        friend class ThreadPoolFactory;
        static void worker(ThreadPool* owner)
        {
            // 正在运行或者任务队列非空线程都要处于启动的状态
            while(owner->running_ || !owner->tasks_.empty())
            {
                // 申请mutex
                std::unique_lock<std::mutex> lock(owner->mutex_);
                // 等待任务队列非空
                while(owner->tasks_.empty() && owner->running_)
                {
                    owner->wait_threads_num_++;
                    owner->cv_.wait(lock);
                    // if this thread is notified by stop() function, then break out of the loop
                    owner->wait_threads_num_--;
                    if(!owner->running_)
                    {   
                        break;
                    }
                }
                // 等待成功
                // 有两种情况
                // 1. 任务队列非空，则取出队首任务，执行任务
                // 2. 任务队列为空，则线程结束
                // 就算线程结束，也要
                if(owner->tasks_.empty() && owner->running_ == false) 
                    return;  
                auto task = owner->tasks_.front();
                owner->tasks_.pop();
                // 释放mutex
                lock.unlock();
                // 执行任务
                task();
            }

        }
        ThreadPool(int numThreads) : numThreads_(numThreads)
        {}
    public:
        
        // start thread pool
        void start()
        {
            // 构建线程池
            for(int i = 0; i < numThreads_; ++i)
            {
                threads_.emplace_back(&ThreadPool::worker,this);
            }
        }
        // add task to tasts_queue
        template <class Func,class ...Args>
        void addTaskToQueue(Func&& f,Args&&... args)
        {
            // 将函数bind
            task_t newtask = std::bind(std::forward<Func>(f),std::forward<Args>(args)...);
            // 申请mutex
            std::unique_lock<std::mutex> lock(mutex_);
            // 将任务放入队列
            tasks_.push(newtask);
            //释放mutex
            lock.unlock();
            // 通知所有沉睡的线程
            if(wait_threads_num_ > 0)
                cv_.notify_all();
            // we don't care the return value   
        }
        // stop thread pool
        void stop()
        {
            running_ = false;
            // 通知所有线程
            cv_.notify_all();
            // 等待所有线程结束
            for(auto& thread : threads_)
            {
                thread.join();
            }
            LOG(LogLevel::INFO) << "thread pool stopped successfully";
        }
        // 
        ~ThreadPool()
        {
            if(running_)
            {
                stop();
            }
        }
    private:
        // thread pool
        std::vector<std::thread> threads_;
        // tasks queue aims to store tasks
        std::queue<task_t> tasks_;
        // this mutex aims to protect tasks_ queue
        std::mutex mutex_;
        // thread num of thread pool
        const int numThreads_;
        // threadpool status
        bool running_ = true;
        // condition variable aims to notify threads when tasks_ queue is not empty
        // if we don't use condition variable, it will cause busy waiting
        // this condition variable aims to reduce busy waiting
        std::condition_variable cv_;
        // wait threads num
        int wait_threads_num_ = 0;
    };
    //  设计模式：工厂模式

    // 工厂模式的优点：
    // 1. 隐藏了创建对象的细节，客户端只需要知道如何获取对象，而不需要知道对象的创建过程。
    // 2. 简化了对象创建过程，客户端只需要调用工厂方法，而不需要知道对象的具体类型。
    // 3. 允许向工厂方法传递参数，以便为创建的对象提供不同的配置。
    class ThreadPoolFactory
    {
    public:
        static ThreadPool* createThreadPool(int numThreads)
        {

            // allow factory to create thread pool with different implementation
            return new ThreadPool(numThreads);
        }
        static void destroyThreadPool(ThreadPool* pool)
        {
            if(!pool) return;
            delete pool;
        }
    };

} 
