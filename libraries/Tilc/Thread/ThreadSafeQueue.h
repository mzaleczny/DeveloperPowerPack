#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

template<typename T>
class TThreadSafeQueue
{
private:
    mutable std::mutex m_Mutex;
    std::queue<T> m_DataQueue;
    std::condition_variable m_DataCond;
public:
    TThreadSafeQueue()
    {}

    void Push(T new_value)
    {
        std::lock_guard<std::mutex> lk(m_Mutex);
        m_DataQueue.push(std::move(new_value));
        m_DataCond.notify_one();
    }

    void WaitAndPop(T& value)
    {
        std::unique_lock<std::mutex> lk(m_Mutex);
        m_DataCond.wait(lk, [this] { return !m_DataQueue.empty(); });
        value = std::move(m_DataQueue.front());
        m_DataQueue.pop();
    }

    std::shared_ptr<T> WaitAndPop()
    {
        std::unique_lock<std::mutex> lk(m_Mutex);
        m_DataCond.wait(lk, [this] { return !m_DataQueue.empty(); });
        std::shared_ptr<T> res(std::make_shared<T>(std::move(m_DataQueue.front())));
        m_DataQueue.pop();
        return res;
    }

    bool TryPop(T& value)
    {
        std::lock_guard<std::mutex> lk(m_Mutex);
        if (m_DataQueue.empty())
        {
            return false;
        }
        value = std::move(m_DataQueue.front());
        m_DataQueue.pop();
        return true;
    }

    std::shared_ptr<T> TryPop()
    {
        std::lock_guard<std::mutex> lk(m_Mutex);
        if (m_DataQueue.empty())
        {
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> res(std::make_shared<T>(std::move(m_DataQueue.front())));
        m_DataQueue.pop();
        return res;
    }

    bool Empty() const
    {
        std::lock_guard<std::mutex> lk(m_Mutex);
        return m_DataQueue.empty();
    }
};
