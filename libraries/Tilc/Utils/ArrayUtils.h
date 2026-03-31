#pragma once
#include "Tilc/DllGlobals.h"

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

}
