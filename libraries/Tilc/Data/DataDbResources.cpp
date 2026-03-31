#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "Tilc/Utils/ExtString.h"
#include <Tilc/Utils/SystemUtils.h>
#include <Tilc/Utils/FileUtils.h>
#include <Tilc/Utils/Crypto.h>
#include "Tilc/Data/DataDbResources.h"
#include <filesystem>
#include "SDL3/SDL.h"

static char CryptoKeyPhrase[] = "+=The Vulkan API is a low-overhead, explicit, cross-platform graphics API that provides applications with direct control over the GPU, maximizing application performance.";

Tilc::Data::TDataDBResources* Tilc::Data::Data = nullptr;

Tilc::Data::TDataDBResources::TDataDBResources(const char* DbFileName, long _MaxResourceFileSize)
    : DbFname(DbFileName), MaxResourceFileSize(_MaxResourceFileSize),
      g_hChildStd_IN_Rd(NULL), g_hChildStd_IN_Wr(NULL), g_hChildStd_OUT_Rd(NULL), g_hChildStd_OUT_Wr(NULL)
{
}

Tilc::Data::TDataDBResources::~TDataDBResources()
{

}

int Tilc::Data::TDataDBResources::CreateResourcesDb(const char* DbFname) {
    if (Tilc::FileExists(DbFname))
    {
        std::filesystem::remove(DbFname);
    }
    Tilc::Data::TDBSQLite* DB = new Tilc::Data::TDBSQLite(DbFname);

    const char *Sql = "CREATE TABLE resources (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, type TEXT, resorigpath TEXT, author TEXT, homepage TEXT, resorder INTEGER, data BLOB, data_slot_1 BLOB, data_slot_2 BLOB, data_slot_3 BLOB);"
                "CREATE INDEX \"idx_resources_name\" ON resources(name);"
                "CREATE INDEX \"idx_resources_name_resolution\" ON resources(name,resolution);";
    int Result = DB->CreateTable(Sql);
    delete DB;
    return Result;
}

int Tilc::Data::TDataDBResources::AddResource(const char* DbFname, const char* Fname, const char* ResName, const char* ResType, const char* ResAuthor, const char* ResHomepage, int ResOrder, Tilc::TExtString& Blob) {
    Tilc::Data::TDBSQLite DB(DbFname);
    char* buffer = NULL;
    size_t buffer_size = 0;
    int id = 0;

    if (!Tilc::FileExists(DbFname))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Plik projektu nie istnieje.");
        return -1;
    }

    size_t DataSize = 0;
    char* DataBuffer = nullptr;
    // jesli podano nazwe pliku z zasobem
    if (strlen(Fname) > 0)
    {
        if (!Tilc::FileExists(Fname))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Plik projektu nie istnieje.");
            return -2;
        }

        if (MaxResourceFileSize > 0)
        {
			DataSize = Tilc::GetFileSize(Fname);
            if (DataSize > MaxResourceFileSize)
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Zbyt duży plik z zasobem.");
                return -3;
			}
		}

        Tilc::FileGetContents(Fname, &DataBuffer, &DataSize);
    }
    else
    {
		DataSize = Blob.size();
		DataBuffer = new char[DataSize];
        if (DataBuffer)
        {
            memcpy(DataBuffer, Blob.data(), DataSize);
        }
	}

    if (DataBuffer)
    {
        const char* Sql = "INSERT INTO resources (name, type, resorigpath, author, homepage, resorder, data) VALUES(?,?,?,?,?,?,?)";
        Tilc::StringXorStr(DataBuffer, DataSize, CryptoKeyPhrase);

        Tilc::Data::TDBFieldTypes Types = {
            Tilc::Data::EDBFieldType::EDBFT_TEXT, Tilc::Data::EDBFieldType::EDBFT_TEXT, Tilc::Data::EDBFieldType::EDBFT_TEXT,
            Tilc::Data::EDBFieldType::EDBFT_TEXT, Tilc::Data::EDBFieldType::EDBFT_TEXT, Tilc::Data::EDBFieldType::EDBFT_INT,
            Tilc::Data::EDBFieldType::EDBFT_BLOB
        };
        Tilc::TStringVector Values = { ResName, ResType, Fname, "", "", "100", "" };
        Values[6].append(DataBuffer, DataSize);
        int InsertedId = DB.Insert(Sql, Types, Values);
        delete[] DataBuffer;
        return InsertedId;
    }

    return -4;
}

int Tilc::Data::TDataDBResources::SetResource(const char* DbFname, const char* Fname, const char* ResName, const char* ResType, const char* ResAuthor, const char* ResHomepage, int ResOrder, Tilc::TExtString& Blob)
{
    Tilc::Data::TDBSQLite DB(DbFname);
    char* buffer = NULL;
    size_t buffer_size = 0;
    int id = 0;

    if (!Tilc::FileExists(DbFname))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Plik projektu nie istnieje.");
        return -1;
    }

    if (strlen(Fname) > 0 && !Tilc::FileExists(Fname))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Plik z zasobem nie istnieje.");
        return -2;
    }

    Tilc::TExtString Sql = "SELECT id FROM resources WHERE name=? LIMIT 1";
    Tilc::Data::TDBFieldTypes Types = {Tilc::Data::EDBFieldType::EDBFT_TEXT, Tilc::Data::EDBFieldType::EDBFT_TEXT};
    Tilc::TStringVector Values = {ResName};
    Tilc::TStringVector FieldNames;
    Tilc::Data::TDBDataRows Rows;
    DB.Select(Sql.c_str(), Types, Values, Rows);
    if (Rows.size() != 1)
    {
        return AddResource(DbFname, Fname, ResName, ResType, ResAuthor, ResHomepage, ResOrder, Blob);
    }
    size_t DataSize = 0;
    char* DataBuffer = nullptr;
    if (strlen(Fname) > 0)
    {
        if (MaxResourceFileSize > 0)
        {
            DataSize = Tilc::GetFileSize(Fname);
            if (DataSize > MaxResourceFileSize)
            {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Zbyt duży plik z zasobem.");
                return -3;
            }
        }

        Tilc::FileGetContents(Fname, &DataBuffer, &DataSize);
	}
    else
    {
        DataSize = Blob.size();
        DataBuffer = new char[DataSize];
        if (DataBuffer)
        {
            memcpy(DataBuffer, Blob.data(), DataSize);
        }
	}
    if (DataBuffer)
    {
        Tilc::StringXorStr(DataBuffer, DataSize, CryptoKeyPhrase);

        Tilc::TStringVector Values = { ResType, Fname, ResAuthor, ResHomepage, std::to_string(ResOrder), "", Rows[0][0] };
        Values[5].append(DataBuffer, DataSize);
        DB.Update("UPDATE resources SET type=?, resorigpath=?, author=?, homepage=?, resorder=?, data=? WHERE id=?",
            {
                Tilc::Data::EDBFieldType::EDBFT_TEXT, Tilc::Data::EDBFieldType::EDBFT_TEXT, Tilc::Data::EDBFieldType::EDBFT_TEXT,
                Tilc::Data::EDBFieldType::EDBFT_TEXT, Tilc::Data::EDBFieldType::EDBFT_INT, Tilc::Data::EDBFieldType::EDBFT_BLOB,
                Tilc::Data::EDBFieldType::EDBFT_INT
            },
            Values);
        delete[] DataBuffer;
    }
    return std::stoi(Rows[0][0]);
}

int Tilc::Data::TDataDBResources::SetSlotData(const char* DbFname, const char* ResName, int SlotNumber, Tilc::TExtString& Blob)
{
    size_t DataSize = Blob.size();
    char* DataBuffer = new char[DataSize];
    if (DataBuffer)
    {
        Tilc::Data::TDBSQLite DB(DbFname);
        const char* Sql;
        if (SlotNumber == 1)
        {
            Sql = "UPDATE resources SET data_slot_1=? WHERE name=?";
        }
        else if (SlotNumber == 2)
        {
			Sql = "UPDATE resources SET data_slot_2=? WHERE name=?";
		}
        else if (SlotNumber == 3)
        {
			Sql = "UPDATE resources SET data_slot_3=? WHERE name=?";
		}
        else
        {
			return -1;
		}


        memcpy(DataBuffer, Blob.data(), DataSize);
        Tilc::StringXorStr(DataBuffer, DataSize, CryptoKeyPhrase);

        Tilc::TStringVector Values = { "", ResName };
        Values[0].append(DataBuffer, DataSize);
        DB.Update(Sql, { Tilc::Data::EDBFieldType::EDBFT_BLOB, Tilc::Data::EDBFieldType::EDBFT_TEXT }, Values);
        delete[] DataBuffer;
        return 0;
    }
    return -2;
}

int Tilc::Data::TDataDBResources::GetResource(const char* DbFname, int Id, Tilc::TExtString& ResOrigPath, char** OutBuffer, size_t* OutBufferSize, int SlotNumber)
{
    Tilc::Data::TDBSQLite DB(DbFname);
    Tilc::Data::TDBDataRows Rows;

    if (OutBuffer)
    {
        *OutBuffer = NULL;
    }
    if (OutBufferSize)
    {
        *OutBufferSize = 0;
    }

    const char* Sql = "SELECT id, data, resorigpath FROM resources WHERE id=? LIMIT 1";
    if (SlotNumber == 1) {
        Sql = "SELECT id, data_slot_1, resorigpath FROM resources WHERE id=? LIMIT 1";
    }
    if (SlotNumber == 2) {
        Sql = "SELECT id, data_slot_2, resorigpath FROM resources WHERE id=? LIMIT 1";
    }
    if (SlotNumber == 3) {
        Sql = "SELECT id, data_slot_3, resorigpath FROM resources WHERE id=? LIMIT 1";
    }

    DB.Select(Sql, { Tilc::Data::EDBFieldType::EDBFT_INT }, { std::to_string(Id) }, Rows);
    if (Rows.size() != 1)
    {
        return -1;
    }

    ResOrigPath = Rows[0][2];
    if (OutBuffer)
    {
        *OutBuffer = new char[Rows[0][1].size()];
        *OutBufferSize = Rows[0][1].size();
        if (*OutBuffer)
        {
            memcpy(*OutBuffer, Rows[0][1].c_str(), *OutBufferSize);
            Tilc::StringXorStr(*OutBuffer, *OutBufferSize, CryptoKeyPhrase);
        }
        else
        {
            *OutBuffer = NULL;
            *OutBufferSize = 0;
        }
    }

    return std::stoi(Rows[0][0]);
}

int Tilc::Data::TDataDBResources::GetResourceByName(const char* DbFname, const char* Name, Tilc::TExtString& ResOrigPath, char** OutBuffer, size_t* OutBufferSize, int SlotNumber)
{
    Tilc::Data::TDBSQLite DB(DbFname);
    Tilc::Data::TDBDataRows Rows;

    if (OutBuffer)
    {
        *OutBuffer = NULL;
    }
    if (OutBufferSize)
    {
        *OutBufferSize = 0;
    }

    const char* Sql = "SELECT id, data, resorigpath FROM resources WHERE name=? LIMIT 1";
    if (SlotNumber == 1) {
        Sql = "SELECT id, data_slot_1, resorigpath FROM resources WHERE name=? LIMIT 1";
    }
    if (SlotNumber == 2) {
        Sql = "SELECT id, data_slot_2, resorigpath FROM resources WHERE name=? LIMIT 1";
    }
    if (SlotNumber == 3) {
        Sql = "SELECT id, data_slot_3, resorigpath FROM resources WHERE name=? LIMIT 1";
    }

    DB.Select(Sql, { Tilc::Data::EDBFieldType::EDBFT_TEXT }, { Name }, Rows);
    if (Rows.size() != 1)
    {
        return -1;
    }

    ResOrigPath = Rows[0][2];
    if (OutBuffer)
    {
        *OutBuffer = new char[Rows[0][1].size()];
        *OutBufferSize = Rows[0][1].size();
        if (*OutBuffer)
        {
            memcpy(*OutBuffer, Rows[0][1].c_str(), *OutBufferSize);
            Tilc::StringXorStr(*OutBuffer, *OutBufferSize, CryptoKeyPhrase);
        }
        else
        {
            *OutBuffer = NULL;
            *OutBufferSize = 0;
        }
    }

    return std::stoi(Rows[0][0]);
}

int Tilc::Data::TDataDBResources::GetReferencedResourceByName(const char* DbFname, const char* ReferencingResourceName, Tilc::TExtString& ResOrigPath, char** OutBuffer, size_t* OutBufferSize, int SlotNumber)
{
    char* LocalBuffer = nullptr;
    size_t LocalBufferSize = 0U;
    int ResId = GetResourceByName(DbFname, ReferencingResourceName, ResOrigPath, &LocalBuffer, &LocalBufferSize, 0);
    if (ResId > 0)
    {
        if (*LocalBuffer)
        {
            Tilc::TExtString ReferencedId(LocalBuffer, LocalBufferSize);
            int Id = std::stoi(ReferencedId);
            delete[] LocalBuffer;
            LocalBuffer = nullptr;
            ResId = GetResource(DbFname, Id, ResOrigPath, OutBuffer, OutBufferSize, 0);
            if (ResId > 0)
            {
                return ResId;
            }
        }
    }
    return 0;
}

int Tilc::Data::TDataDBResources::GetResourceIdOfName(const char* DbFname, const char* Name)
{
    Tilc::Data::TDBSQLite DB(DbFname);
    Tilc::Data::TDBDataRows Rows;

    const char* Sql = "SELECT id FROM resources WHERE name=? LIMIT 1";
    DB.Select(Sql, { Tilc::Data::EDBFieldType::EDBFT_TEXT }, { Name }, Rows);
    if (Rows.size() != 1)
    {
        return -1;
    }

    return std::stoi(Rows[0][0]);
}

std::vector<Tilc::TExtString> Tilc::Data::TDataDBResources::GetMusicResourcesList(const char* DbFname)
{
    std::vector<Tilc::TExtString> Result;
    Tilc::Data::TDBSQLite DB(DbFname);
    Tilc::Data::TDBDataRows Rows;
    //const char* Sql = "SELECT name, resorigpath FROM resources WHERE type=?";
    const char* Sql = "SELECT name FROM resources WHERE type=?";

    DB.Select(Sql, { Tilc::Data::EDBFieldType::EDBFT_TEXT }, { "Audio" }, Rows);

    for (Tilc::Data::TDBDataRow& Row : Rows)
    {
		//Result.push_back(Row[0] + " [ " + Row[1] + " ]");
        Result.push_back(Row[0]);
	}

    return Result;
}


size_t Tilc::Data::TDataDBResources::GetAllResourcesList(const char* DbFname, Tilc::Data::TDBDataRows& Rows)
{
    std::vector<Tilc::TExtString> Result;
    Tilc::Data::TDBSQLite DB(DbFname);
    const char* Sql = "SELECT id, name, type, resorigpath, resorder FROM resources ORDER BY id ASC";

    DB.Select(Sql, Rows);

    return Rows.size();
}

int Tilc::Data::TDataDBResources::RemoveResource(const char* DbFname, int Id)
{
    Tilc::Data::TDBSQLite DB(DbFname);
    return DB.Delete("DELETE FROM resources WHERE id=?", Id);
}

int Tilc::Data::TDataDBResources::RemoveResourceByName(const char* DbFname, const char* ResName)
{
    Tilc::Data::TDBSQLite DB(DbFname);
    Tilc::Data::TDBFieldTypes Types = { Tilc::Data::EDBFieldType::EDBFT_TEXT };
    Tilc::TStringVector Values = { ResName };
    return DB.ExecQuery("DELETE FROM resources WHERE name=?", Types, Values);
}

/*
void Tilc::Data::TDataDBResources::PlayMusicFromResource(const char* DBFilePath, const wchar_t* ResName, const wchar_t* PlayerFilePath)
{
    char* ResBlob = nullptr;
    size_t ResBlobSize = 0;
    Tilc::TExtString ResOrigPath;

    Tilc::TExtString RN;
    RN.FromWideString(ResName, CP_UTF8);
    int ResId = GetResourceByName(DBFilePath, RN.c_str(), ResOrigPath, &ResBlob, &ResBlobSize, 0);
    if (ResId > 0)
    {
        //PlaySound((LPCTSTR)ResBlob, NULL, SND_MEMORY | SND_ASYNC);
        // Set the bInheritHandle flag so pipe handles are inherited.
        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        // Create a pipe for the child process's STDOUT
        if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
        {
            Tilc::ShowDetailedLastSystemError("StdoutRd CreatePipe");
        }
        else
        {
            // Ensure the read handle to the pipe for STDOUT is not inherited.
            if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
            {
                Tilc::ShowDetailedLastSystemError("Stdout SetHandleInformation");
            }
            else
            {
                // Create a pipe for the child process's STDIN. 
                if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
                {
                    Tilc::ShowDetailedLastSystemError("Stdin CreatePipe");
                }
                else
                {
                    // Ensure the write handle to the pipe for STDIN is not inherited. 
                    if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
                    {
                        Tilc::ShowDetailedLastSystemError("Stdin SetHandleInformation");
                    }
                    else
                    {
                        TCHAR szCmdline[MAX_PATH]{ 0 };
                        PROCESS_INFORMATION piProcInfo;
                        STARTUPINFO siStartInfo;
                        BOOL bSuccess = FALSE;
                        
                        wcsncat_s(reinterpret_cast<wchar_t*>(szCmdline), MAX_PATH * sizeof(TCHAR), PlayerFilePath, MAX_PATH - 1);
                        // Set up members of the PROCESS_INFORMATION structure.
                        ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

                        // Set up members of the STARTUPINFO structure. 
                        // This structure specifies the STDIN and STDOUT handles for redirection.
                        ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
                        siStartInfo.cb = sizeof(STARTUPINFO);
                        siStartInfo.hStdError = g_hChildStd_OUT_Wr;
                        siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
                        siStartInfo.hStdInput = g_hChildStd_IN_Rd;
                        siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

                        // Create the child process.
                        bSuccess = CreateProcess(NULL,
                            szCmdline,     // command line
                            NULL,          // process security attributes
                            NULL,          // primary thread security attributes
                            TRUE,          // handles are inherited
                            0,             // creation flags
                            NULL,          // use parent's environment
                            NULL,          // use parent's current directory
                            &siStartInfo,  // STARTUPINFO pointer
                            &piProcInfo);  // receives PROCESS_INFORMATION

                        // If an error occurs, exit the application.
                        if (!bSuccess)
                        {
                            Tilc::ShowDetailedLastSystemError("CreateProcess");
                        }
                        else
                        {
                            // Close handles to the child process and its primary thread.
                            // Some applications might keep these handles to monitor the status
                            // of the child process, for example. 
                            CloseHandle(piProcInfo.hProcess);
                            CloseHandle(piProcInfo.hThread);

                            // Close handles to the stdin and stdout pipes no longer needed by the child process.
                            // If they are not explicitly closed, there is no way to recognize that the child process has ended.
                            CloseHandle(g_hChildStd_OUT_Wr);
                            CloseHandle(g_hChildStd_IN_Rd);

                            DWORD dwWritten;
                            DWORD ChunkSize = 4096;
                            DWORD Count = static_cast<DWORD>(ResBlobSize / ChunkSize);
                            DWORD Rest = static_cast<DWORD>(ResBlobSize % ChunkSize);
                            unsigned long i;
                            for (i = 0; i < Count; i++)
                            {
                                bSuccess = WriteFile(g_hChildStd_IN_Wr, ResBlob + i * ChunkSize, ChunkSize, &dwWritten, NULL);
                                if (!bSuccess)
                                {
                                    Tilc::ShowDetailedLastSystemError("CreateProcess");
                                    break;
                                }
                            }
                            if (bSuccess && Rest > 0)
                            {
                                bSuccess = WriteFile(g_hChildStd_IN_Wr, ResBlob + i * ChunkSize, Rest, &dwWritten, NULL);
                            }
                            CloseHandle(g_hChildStd_IN_Wr);
                        }
                    }
                }
            }
        }
    }
    if (ResBlob)
    {
        delete[] ResBlob;
    }
}
*/
