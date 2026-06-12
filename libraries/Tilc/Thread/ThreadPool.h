#include <iostream>
#include <algorithm>
#include <atomic>
#include <thread>
#include <future>
#include "Tilc/Thread/ThreadSafeQueue.h"

class TJoinThreads
{
    std::vector<std::thread>& m_Threads;
public:
    explicit TJoinThreads(std::vector<std::thread>& threads) :
        m_Threads(threads)
    {}
    ~TJoinThreads()
    {
        for (unsigned long i = 0; i < m_Threads.size(); ++i)
        {
            if (m_Threads[i].joinable())
            {
                m_Threads[i].join();
            }
        }
    }
};

class TThreadPool
{
private:
    std::atomic_bool m_Done;
    std::atomic_bool m_WorkStarted;
    TThreadSafeQueue<std::function<void()>> m_WorkQueue;
    std::vector<std::thread> m_Threads;
    TJoinThreads m_Joiner;

    void WorkerThread()
    {
        while (!m_Done)
        {
            std::function<void()> task;
            if(m_WorkQueue.TryPop(task))
            {
                m_WorkStarted = true;
                task();
            }
            else
            {
                std::this_thread::yield();
                if (m_WorkStarted)
                {
                    m_Done = true;
                }
            }
        }
    }
public:
    TThreadPool():
        m_Done(false), m_Joiner(m_Threads), m_WorkStarted(false)
    {
        unsigned const ThreadCount = std::thread::hardware_concurrency();
        try
        {
            for(unsigned i = 0; i < ThreadCount; ++i)
            {
                m_Threads.push_back(std::thread(&TThreadPool::WorkerThread, this));
            }
        }
        catch (...)
        {
            m_Done = true;
        }
    }

    ~TThreadPool()
    {
        m_Done = true;
    }

    bool IsDone() const { return m_Done.load(); }

    template<typename FunctionType>
    void submit(FunctionType f)
    {
        m_WorkQueue.Push(std::function<void()>(f));
    }
};
