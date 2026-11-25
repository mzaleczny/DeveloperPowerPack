#include "Tilc/Utils/Log.h"
#include "Tilc/Utils/FileUtils.h"
#include <fstream>

#include "Time.h"

namespace Tilc
{
	TLog::TLog(Tilc::TExtString FileName) : m_FileName(FileName)
	{
	}

	void TLog::operator()(Tilc::TExtString DateTime, const Tilc::TExtString& AppendStr)
	{
		TFile File(m_FileName.c_str(), std::ios::out | std::ios::app);
		File.AppendContent(DateTime + ": " + AppendStr + "\n");
	}

	void TLog::operator()(const Tilc::TExtString& AppendStr)
	{
		TFile File(m_FileName.c_str(), std::ios::out | std::ios::app);
		File.AppendContent(Tilc::GetCurrentDateTime() + ": " + AppendStr + "\n");
	}
}
