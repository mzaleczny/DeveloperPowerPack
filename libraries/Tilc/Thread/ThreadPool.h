#pragma once

#include <iostream>
#include <algorithm>
#include <atomic>
#include <thread>
#include <future>
#include <functional>
#include "Tilc/DllGlobals.h"
#include "Tilc/Thread/ThreadSafeQueue.h"

namespace Tilc::Thread
{
    class DECLSPEC TJoinThreads
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

    class DECLSPEC TThreadPool
    {
    private:
        std::atomic_bool m_Done;
        std::atomic_bool m_AllThreadsAdded;
        TThreadSafeQueue<std::function<void()>> m_WorkQueue;
        std::vector<std::thread> m_Threads;
        TJoinThreads m_Joiner;

        void WorkerThread()
        {
            while (!m_Done)
            {
                std::function<void()> task;
                if (m_WorkQueue.TryPop(task))
                {
                    task();
                }
                else
                {
                    if (m_AllThreadsAdded)
                    {
                        m_Done = true;
                    }
                    else
                    {
                        std::this_thread::yield();
                    }
                }
            }
        }
    public:
        TThreadPool() :
            m_Done(false), m_Joiner(m_Threads), m_AllThreadsAdded(false)
        {
            unsigned const ThreadCount = std::thread::hardware_concurrency();
            try
            {
                for (unsigned i = 0; i < ThreadCount; ++i)
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
        bool SetAllThreadsAdded(bool Value) { return m_AllThreadsAdded = Value; }

        template<typename FunctionType>
        void submit(FunctionType f)
        {
            m_WorkQueue.Push(std::function<void()>(f));
        }
    };
}
