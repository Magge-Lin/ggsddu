#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <functional>
#include <deque>


class CThreadPool {
public:

    static CThreadPool& getInstance();

    void initialize(size_t numThreads);

    void submitTask(std::function<void()> task);

private:

    CThreadPool();
    ~CThreadPool();

    // 禁止拷贝构造函数和赋值操作符
    CThreadPool(const CThreadPool&) = delete;
    CThreadPool& operator=(const CThreadPool&) = delete;

    void workerThread();

private:
    bool m_initialized;
    std::vector<std::thread> m_threads;
    std::deque<std::function<void()>> m_tasks;
    
    std::mutex m_mutex;
    std::mutex m_taskMutex;
};

