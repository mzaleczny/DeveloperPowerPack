#include "Tilc/Utils/Log.h"
#include "Tilc/Utils/FileUtils.h"
#include <fstream>

namespace Tilc
{
	TLog::TLog(Tilc::TExtString FileName) : m_FileName(FileName)
	{
	}

	void TLog::operator()(Tilc::TExtString Data, const Tilc::TExtString& AppendStr)
	{
		TFile File(m_FileName.c_str(), std::ios::out | std::ios::app);
		File.AppendContent(Data + AppendStr);
	}
}
