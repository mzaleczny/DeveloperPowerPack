#pragma once
#include "Tilc/DllGlobals.h"
#include <cstddef>
#include <vector>
#include <bitset>

namespace Tilc
{

	template <typename T>
	bool InArray(T* Array, size_t ArraySize, T Elem)
	{
		for (size_t i = 0; i < ArraySize; ++i)
		{
			if (Array[i] == Elem)
			{
				return true;
			}
		}
		return false;
	}

	template <size_t N>
	std::vector<unsigned long> BitsetToVectorUlong(std::bitset<N> bs)
	{
		auto result = std::vector<unsigned long> {};
		auto const size = 8 * sizeof(unsigned long);
		auto const mask = std::bitset<N>{ static_cast<unsigned long>(-1)};
		auto totalbits = 0;
		while (totalbits < N)
		{
			auto value = (bs & mask).to_ulong();
			result.push_back(value);
			bs >>= size;
			totalbits += size;
		}
		return result;
	}
}
