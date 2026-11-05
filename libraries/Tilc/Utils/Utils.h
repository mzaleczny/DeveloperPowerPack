#pragma once
#include <limits>
#include <utility>
#include <thread>

namespace Tilc
{
    constexpr const unsigned long long int UINT_LONGLONG_MAX = std::numeric_limits<unsigned long long int>::max();

    template <typename T> concept HasXOR = requires(T a, T b) { a ^ b; };
    template <typename T> concept Assignable = requires(T a, T b) { T(std::move(b)); b = std::move(a); };
    template <typename T> concept Comparable = requires(T a, T b) { a < b; };

    template <HasXOR T> void Swap(T& x, T& y)
    {
        x = x ^ y;
        y = x ^ y;
        x = x ^ y;
    }

    template <Assignable T> void Swap(T& x, T& y)
        requires (!HasXOR<T>)
    {
        T tmp(std::move(x));
        x = std::move(y);
        y = std::move(tmp);
    }

    // Parallel map. This function is much faster than parallel version of standard one from std library.
    template <typename Iter, typename F>
    void ParallelMap(Iter begin, Iter end, F&& f)
    {
        auto size = std::distance(begin, end);

        if (size <= 10000)
        {
            std::transform(begin, end, begin, std::forward<F>(f));
        }
        else
        {
            auto no_of_threads = std::thread::hardware_concurrency();
            auto part = size / no_of_threads;
            auto last = begin;

            std::vector<std::thread> threads;
            for (unsigned i = 0; i < no_of_threads; ++i)
            {
                if (i == no_of_threads - 1)
                {
                    last = end;
                }
                else
                {
                    std::advance(last, part);
                }

                threads.emplace_back([=, &f] {std::transform(begin, last, begin, std::forward<F>(f)); });

                begin = last;
            }

            for (auto & t : threads) t.join();
        }
    }

    // Parallel accumulate. This function is much faster than parallel version of standard one from std library.
    template <typename Iter, typename R, typename F>
    auto ParallelAccumulate(Iter begin, Iter end, R init, F&& op)
    {
        auto size = std::distance(begin, end);

        if (size <= 10000)
        {
            return std::accumulate(begin, end, init, std::forward<F>(op));
        }
        else
        {
            auto no_of_threads = std::thread::hardware_concurrency();
            auto part = size / no_of_threads;
            auto last = begin;

            std::vector<std::thread> threads;
            std::vector<R> values(no_of_threads);
            for (unsigned i = 0; i < no_of_threads; ++i)
            {
                if (i == no_of_threads - 1)
                {
                    last = end;
                }
                else
                {
                    std::advance(last, part);
                }

                threads.emplace_back([=, &op](R& result) {result = std::accumulate(begin, last, R{}, std::forward<F>(op)); },
                   std::ref(values[i]));

                begin = last;
            }

            for (auto & t : threads) t.join();

            return std::accumulate(std::begin(values), std::end(values), init, std::forward<F>(op));
        }
    }
}
