#pragma once

#include "Tilc/DllGlobals.h"
#include <Tilc/Data/DbSQLite.h>
#include "Tilc/Utils/ExtString.h"
#include <vector>

namespace Tilc::Data {
	class DECLSPEC TDataDBResources
	{
	public:
		TDataDBResources(const char* DbFileName, long _MaxResourceFileSize = 0);
		~TDataDBResources();

		// If data file exists, it will be deleted. After that creates new database file with resources table.
		int CreateResourcesDb(const char* DbFname);
		// Adds resource to the database. If specified Fname then resource blob is filled with data of the file otherwise parameter Blob is used.
		int AddResource(const char* DbFname, const char* Fname, const char* ResName, const char* ResType, const char* ResAuthor, const char* ResHomepage, int ResOrder, Tilc::TExtString& Blob);
		// If resource with ResName exists, it will be updated otherwise it will be added as in AddResource method.
		int SetResource(const char* DbFname, const char* Fname, const char* ResName, const char* ResType, const char* ResAuthor, const char* ResHomepage, int ResOrder, Tilc::TExtString& Blob);
		int SetSlotData(const char* DbFname, const char* ResName, int SlotNumber, Tilc::TExtString& Blob);

		int GetResource(const char* DbFname, int Id, Tilc::TExtString& ResOrigPath, char** OutBuffer, size_t* OutBufferSize, int SlotNumber);
		int GetResourceByName(const char* DbFname, const char* Name, Tilc::TExtString& ResOrigPath, char** OutBuffer, size_t* OutBufferSize, int SlotNumber);
		int GetReferencedResourceByName(const char* DbFname, const char* ReferencingResourceName, Tilc::TExtString& ResOrigPath, char** OutBuffer, size_t* OutBufferSize, int SlotNumber);

		int GetResourceIdOfName(const char* DbFname, const char* Name);

		int RemoveResource(const char* DbFname, int Id);
		int RemoveResourceByName(const char* DbFname, const char* ResName);

		std::vector<Tilc::TExtString> GetMusicResourcesList(const char* DbFname);
		size_t GetAllResourcesList(const char* DbFname, Tilc::Data::TDBDataRows& Rows);
		//void PlayMusicFromResource(const char* DBFilePath, const wchar_t* ResName, const wchar_t* PlayerFilePath);

		inline const char* GetDbFname() const { return DbFname.c_str(); }
	private:
		long MaxResourceFileSize;
		void* g_hChildStd_IN_Rd;
		void* g_hChildStd_IN_Wr;
		void* g_hChildStd_OUT_Rd;
		void* g_hChildStd_OUT_Wr;
		Tilc::TExtString DbFname;
	};

	extern TDataDBResources* Data;
}
