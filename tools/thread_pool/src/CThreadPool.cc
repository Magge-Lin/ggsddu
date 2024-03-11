#include "CThreadPool.h"

CThreadPool::CThreadPool() 
:m_initialized(false) 
{

}
CThreadPool::~CThreadPool() 
{

}

CThreadPool& CThreadPool::getInstance() 
{
    static CThreadPool instance;
    return instance;
}

void CThreadPool::initialize(size_t numThreads) 
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_initialized) 
    {
        for (size_t i = 0; i < numThreads; ++i) 
        {
            m_threads.emplace_back([this]() { workerThread(); });
        }
        m_initialized = true;
    }
}

void CThreadPool::submitTask(std::function<void()> task) 
{
    std::lock_guard<std::mutex> lock(m_taskMutex);
    m_tasks.push_back(task);
}

void CThreadPool::workerThread() 
{
    while (true) 
    {
        std::function<void()> task;
        {
            std::lock_guard<std::mutex> lock(m_taskMutex);
            if (!m_tasks.empty()) 
            {
                task = m_tasks.front();
                m_tasks.pop_front();
            } else 
            {
                break;
            }
        }
        
        task();  // 执行任务
    }
}
