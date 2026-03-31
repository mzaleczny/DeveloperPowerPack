#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc::Commerce
{

	class DECLSPEC TMoney
	{
	public:
        TMoney() = default;
		TMoney(double v) : m_Value(v) {}
		void operator()(Tilc::TExtString Data, const Tilc::TExtString& AppendStr = "\n");

        double m_Value;
	};

}

inline bool operator==(const Tilc::Commerce::TMoney& m1, const Tilc::Commerce::TMoney& m2)
{
	return m1.m_Value == m2.m_Value;
}
inline bool operator<(const Tilc::Commerce::TMoney& m1, const Tilc::Commerce::TMoney& m2)
{
	return m1.m_Value < m2.m_Value;
}
inline bool operator>(const Tilc::Commerce::TMoney& m1, const Tilc::Commerce::TMoney& m2)
{
	return m1.m_Value > m2.m_Value;
}
inline Tilc::Commerce::TMoney operator+(const Tilc::Commerce::TMoney& m1, const Tilc::Commerce::TMoney& m2)
{
	return Tilc::Commerce::TMoney{m1.m_Value + m2.m_Value};
}
inline Tilc::Commerce::TMoney operator-(const Tilc::Commerce::TMoney& m1, const Tilc::Commerce::TMoney& m2)
{
	return Tilc::Commerce::TMoney{m1.m_Value - m2.m_Value};
}
