#pragma once
#include "Tilc/DllGlobals.h"
#include <cstddef>
#include <vector>
#include <bitset>
#include <algorithm>
#include <stdexcept>

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

	DECLSPEC class TBitVector
    {
      std::vector<bool> bv;
    public:
      TBitVector() = default;
      TBitVector(TBitVector const&) = default;
      TBitVector(TBitVector&&) = default;
      TBitVector& operator=(TBitVector const&) = default;
      TBitVector& operator=(TBitVector&&) = default;

      TBitVector(std::vector<bool> const& bv) : bv(bv) {}
      bool operator[](size_t const i) { return bv[i]; }

      inline bool any() const {
         for (auto b : bv) if (b) return true;
         return false;
      }

      inline bool all() const {
         for (auto b : bv) if (!b) return false;
         return true;
      }

      inline bool none() const { return !any(); }

      inline size_t count() const {
         return std::count(bv.cbegin(), bv.cend(), true);
      }

      inline size_t size() const { return bv.size(); }

      inline TBitVector& add(bool const value) {
         bv.push_back(value);
         return *this;
      }

      inline TBitVector& remove(size_t const index) {
         if (index >= bv.size())
            throw std::out_of_range("Index out of range");
         bv.erase(bv.begin() + index);
         return *this;
      }

      inline TBitVector& set(bool const value = true) {
         for (size_t i = 0; i < bv.size(); ++i)
            bv[i] = value;
         return *this;
      }

      inline TBitVector& set(size_t const index, bool const value = true) {
         if (index >= bv.size())
            throw std::out_of_range("Index out of range");
         bv[index] = value;
         return *this;
      }

      inline TBitVector& reset() {
         for (size_t i = 0; i < bv.size(); ++i) bv[i] = false;
         return *this;
      }

      inline TBitVector& reset(size_t const index) {
         if (index >= bv.size())
            throw std::out_of_range("Index out of range");
         bv[index] = false;
         return *this;
      }

      inline TBitVector& flip() {
         bv.flip();
         return *this;
      }

      std::vector<bool>& data() { return bv; }
   };
}
