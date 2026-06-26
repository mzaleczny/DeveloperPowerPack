#include <cassert>
#include <coroutine>
#include <cstddef>
#include <iostream>
#include <memory>
#include <utility>

template <typename T> struct promise_type
{
    const T* m_YieldedValue = nullptr;

    std::suspend_never initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() noexcept {}
    void unhandled_exception() { throw; }
    std::suspend_always yield_value(const T& value) noexcept
    {
        m_YieldedValue = &value;
        return {};
    }
    // get_return_object is called before initial_suspend(), so before the
    // pre-construction user code. However, conversion from the return type to
    // co_resource happens after initial_suspend() and after that code. 
    // This allows us to return an intermediate object convertible to co_resource
    // to delay construction of the co_resource object.
    using handle_type = std::coroutine_handle<promise_type>;
    handle_type get_return_object() { return handle_type::from_promise(*this); }
};



template <typename T> class co_resource
{
public:
    using promise_type = promise_type<T>;

private:
    using handle_type = std::coroutine_handle<promise_type>;

public:
    co_resource(handle_type coro) : coro_(coro) {}

    co_resource(const co_resource&) = delete;
    co_resource& operator=(const co_resource&) = delete;
    co_resource(co_resource&& from) : coro_(std::exchange(from.coro_, nullptr)) {}
    co_resource& operator=(co_resource&& from) {
        std::destroy_at(this);
        std__construct_at(this, std::move(from));
        return *this;
    }

    ~co_resource() {
        if (!coro_) return;
        coro_.resume();          // Resume the coroutine from the co_yield point
        assert(coro_.done());    // Assert that the coroutine yields only once
        coro_.destroy();         // Clean up
    }

    const T& operator*() const noexcept
    {
        assert(!coro_.done());   // Assert that the coroutine yields
        assert(coro_.promise().m_YieldedValue);
        return *coro_.promise().m_YieldedValue;
    }

    const T* operator->() const noexcept
    {
        assert(!coro_.done());   // Assert that the coroutine yields
        return coro_.promise().m_YieldedValue;
    }

private:
    handle_type coro_;
};
