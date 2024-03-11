#include <chrono>
#include <unistd.h>
#include <vector>
#include <mutex>

#include "CThreadPool.h"

static std::vector<unsigned long> s_vec;
std::mutex s_lock;

void do_task() 
{
    std::lock_guard<std::mutex> lock(s_lock);
    pthread_t threadId = pthread_self();
    if (s_vec.empty())
    {
        std::cout<<"start push."<<std::endl;
        s_vec.push_back((unsigned long)threadId);
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        std::cout<<"s_vec.push_back : "<<(unsigned long)threadId<<std::endl;
    }
    else
    {
        std::cout<<"start erase."<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        s_vec.erase(s_vec.begin());
        std::cout<<"s_vec.erase and s_vec siza: "<<s_vec.size()<<std::endl;
    }

    std::cout<<"end."<<std::endl;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

int main() 
{
    // 使用线程池执行任务
    size_t numThreads = 8;  // 线程池中线程数量
    CThreadPool::getInstance().initialize(numThreads);

    for (;;) 
    {
        CThreadPool::getInstance().submitTask(do_task);
    }
    
    // 等待所有任务完成
    std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}
