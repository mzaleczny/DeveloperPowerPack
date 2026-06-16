#pragma once
#include <limits>

namespace Tilc::Utils
{
    constexpr const unsigned int UINT_LONGONG_MAX = std::numeric_limits<unsigned long long int>::max();

    template <typename T> concept HasXOR = requires(T a, T b) { a^ b; };
    template <typename T> concept Assignable = requires(T a, T b) { T(std::move(b)); b = std::move(a); };

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

}
