#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include <chrono>

namespace Tilc::Commerce
{

	class DECLSPEC TMessage
	{
	public:
		TMessage() {}
		TMessage(const Tilc::TExtString& Subject, const Tilc::TExtString& Body,
            const Tilc::TExtString& From,
            std::chrono::time_point<std::chrono::system_clock> DateTime)
            : m_Subject(Subject), m_Body(Body), m_From(From), m_DateTime(DateTime)
            {}

	private:
		Tilc::TExtString m_Subject;
		Tilc::TExtString m_Body;
		Tilc::TExtString m_From;
        std::chrono::time_point<std::chrono::system_clock> m_DateTime;
	};

}
