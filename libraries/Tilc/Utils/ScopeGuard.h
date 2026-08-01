#pragma once

namespace Tilc
{
    class TScopeGuardBase
    {
    public:
        TScopeGuardBase() = default;
        void Commit() noexcept { m_Commit = true; }

        TScopeGuardBase(TScopeGuardBase& other) : m_Commit(other.m_Commit)
        {
            // Disarms the original guard - prevent source object to call its RollbackFunction in its destructor
            other.Commit();
        }
        ~TScopeGuardBase() {}

    protected:
        bool m_Commit = false;

    private:
        TScopeGuardBase& operator=(const TScopeGuardBase&) = delete;
    };

    template <typename RollbackFunc, typename Arg>
    class TScopeGuard : public TScopeGuardBase
    {
    public:
        TScopeGuard(RollbackFunc& func, Arg& arg) : m_RollbackFunc(func), m_Arg(arg)
        {
        }
        TScopeGuard(TScopeGuard& other) : TScopeGuardBase(other), m_RollbackFunc(other.m_RollbackFunc), m_Arg(other.m_Arg)
        {
        }
        TScopeGuard(TScopeGuard&& other) : TScopeGuardBase(other), m_RollbackFunc(other.m_RollbackFunc), m_Arg(other.m_Arg)
        {
            other.m_Rollabck = {};
            other.m_Arg = {};
        }
        ~TScopeGuard()
        {
            if (!m_Commit) m_RollbackFunc(m_Arg);
        }
    private:
        const RollbackFunc& m_RollbackFunc;
        Arg& m_Arg;
    };

    template <typename RollbackFunc, typename Arg>
    TScopeGuard<RollbackFunc, Arg> MakeGuard(RollbackFunc func, Arg& arg)
    {
        return TScopeGuard<RollbackFunc, Arg>(func, arg);
    }
}
