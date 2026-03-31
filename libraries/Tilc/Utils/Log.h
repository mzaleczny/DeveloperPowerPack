#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc
{

	class DECLSPEC TLog
	{
	public:
		TLog() = default;
		TLog(Tilc::TExtString FileName);
		~TLog() = default;
		void SetFile(Tilc::TExtString FileName)
		{
			m_FileName = FileName;
		}
		void operator()(Tilc::TExtString Data, const Tilc::TExtString& AppendStr = "\n");

	private:
		TExtString m_FileName;
	};

}
