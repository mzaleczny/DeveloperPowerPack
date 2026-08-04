#pragma once

#include "Tilc/Globals.h"

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
        TScopeGuard(RollbackFunc&& func, Arg&& arg) : m_RollbackFunc(std::move(func)), m_Arg(std::move(arg))
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
            if (!m_Commit)
            {
                try {
                    m_RollbackFunc(m_Arg);
                }
                catch (...)
                {
                }
            }
        }
    private:
        const RollbackFunc m_RollbackFunc;
        Arg m_Arg;
    };

    template <typename RollbackFunc, typename Arg>
    TScopeGuard<RollbackFunc, Arg> MakeGuard(RollbackFunc func, Arg& arg)
    {
        return TScopeGuard<RollbackFunc, Arg>(func, arg);
    }

    template <typename RollbackMemberFunc, typename Obj, typename Arg>
    class TScopeGuardObj : public TScopeGuardBase
    {
    public:
        TScopeGuardObj(RollbackMemberFunc& func, Obj& obj, Arg& arg) : m_RollbackMemberFunc(func), m_Obj(obj), m_Arg(arg)
        {
        }
        TScopeGuardObj(RollbackMemberFunc&& func, Obj&& obj, Arg&& arg) : m_RollbackMemberFunc(std::move(func)), m_Obj(std::move(obj)), m_Arg(std::move(arg))
        {
        }
        TScopeGuardObj(TScopeGuardObj& other) : TScopeGuardBase(other), m_RollbackMemberFunc(other.m_RollbackMemberFunc), m_Obj(other.m_Obj), m_Arg(other.m_Arg)
        {
        }
        TScopeGuardObj(TScopeGuardObj&& other) : TScopeGuardBase(other), m_RollbackMemberFunc(other.m_RollbackMemberFunc), m_Obj(other.m_Obj), m_Arg(other.m_Arg)
        {
            other.m_RollbackMemberFunc = {};
            other.m_Obj = {};
            other.m_Arg = {};
        }
        ~TScopeGuardObj()
        {
            if (!m_Commit)
            {
                try
                {
                    m_Obj.*m_RollbackMemberFunc(m_Arg);
                }
                catch (...)
                {
                }
        }
    private:
        const RollbackMemberFunc m_RollbackMemberFunc;
        Obj m_Obj;
        Arg m_Arg;
    };

    template <typename RollbackMemberFunc, typename Obj, typename Arg>
    TScopeGuardObj<RollbackMemberFunc, Obj, Arg> MakeGuard(RollbackMemberFunc func, Obj& obj, Arg& arg)
    {
        return TScopeGuardObj<RollbackMemberFunc, Obj, Arg>(func, obj, arg);
    }


    template <typename Arg>
    struct TScopeGuardOnExit {
        Arg m_Arg{};
    };
    template <typename Func, typename Arg>
    TScopeGuard<Func, Arg> operator+(TScopeGuardOnExit<Arg> s, Func&& func) {
        return TScopeGuard<Func, Arg>(std::forward<Func>(func), std::move(s.m_Arg));
    }
}


// use of macro below:
//     ON_SCOPE_EXIT { S.finalize(); };
#define ON_SCOPE_EXIT auto UNIQUE_ANON_VAR(SCOPE_EXIT_STATE) = Tilc::TScopeGuardOnExit<int>() + [&](int arg)
// use of macro below:
//     ON_SCOPE_EXIT_WITH_VALUE(12) { S.finalize(); };
#define ON_SCOPE_EXIT_WITH_VALUE(Value) auto UNIQUE_ANON_VAR(SCOPE_EXIT_STATE) = Tilc::TScopeGuardOnExit<decltype(Value)>({Value}) + [&](decltype(Value) arg)

// use of macro below:
//     ON_SCOPE_EXIT { S.finalize(); };
#define NAMED_ON_SCOPE_EXIT(NAME) auto NAME = Tilc::TScopeGuardOnExit<int>() + [&](int arg)
// use of macro below:
//     ON_SCOPE_EXIT_WITH_VALUE(12) { S.finalize(); };
#define NAMED_ON_SCOPE_EXIT_WITH_VALUE(NAME, Value) auto NAME = Tilc::TScopeGuardOnExit<decltype(Value)>({Value}) + [&](decltype(Value) arg)
