#pragma once

template <typename T>
class TSmallQueue
{
public:
    TSmallQueue() = default;
    ~TSmallQueue() = default;

    bool push(T i)
    {
        if (m_Front - m_Tail > m_BufSize) return false;
        buf_[(++m_Front) & (m_BufSize - 1)] = i;
        return true;
    }

    T front() const
    {
        return buf_[m_Tail & (m_BufSize - 1)];
    }

    void pop()
    {
        ++m_Tail;
    }

    size_t size() const
    {
        return m_Front - m_Tail;
    }
    
    bool empty() const
    {
        return m_Front == m_Tail;
    }

protected:
    static constexpr size_t m_BufSize = 16;
    static_assert((m_BufSize & (m_BufSize - 1)) == 0, "Buffer size must be a power of 2");
    T buf_[m_BufSize];
    size_t m_Front = 0;
    size_t m_Tail = 0;
};
