#pragma once

#include "Tilc/Utils/ExtString.h"
#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <fstream>
#include <filesystem>

namespace Tilc
{
    // obiekt typu RAII -> Rescource Aquisition Is Initialization
    class DECLSPEC TFile
    {
    public:
        TFile(const char* FileName, std::ios_base::openmode OpenMode = std::ios::in);
        ~TFile();
        void ReadLines(std::vector<Tilc::TExtString>& Lines);
        void ReadContent(Tilc::TExtString& Content);
        void WriteContent(const Tilc::TExtString& Content);
        void AppendContent(const Tilc::TExtString& Content);

        std::fstream m_File;

    private:
        std::mutex m_MU;
        void OpenFile(const char* FileName, std::ios_base::openmode OpenMode = std::ios::in);
    };


    // obiekt typu RAII -> Rescource Aquisition Is Initialization
    class DECLSPEC TCFile
    {
    public:
        TCFile(const char* FileName, const char* OpenMode = "r");
        ~TCFile();
        char* ReadLine(Tilc::TExtString& Line);
        void ReadLines(std::vector<Tilc::TExtString>& Lines);
        void ReadContent(Tilc::TExtString& Content);
        void WriteContent(const Tilc::TExtString& Content);

        FILE* m_File;

    private:
        std::mutex m_MU;
        void OpenFile(const char* FileName, const char* OpenMode = "r");
    };

	DECLSPEC bool DirectoryExists(const Tilc::TExtString& Path);
	DECLSPEC bool FileExists(const Tilc::TExtString& Path);
	DECLSPEC TExtString GetAppDataFolder();
	DECLSPEC TExtString GetTmpFolder();
	DECLSPEC void FilePutContents(const Tilc::TExtString& Path, const Tilc::TExtString& Content);
	DECLSPEC void FilePutContentsRaw(Tilc::TExtString Path, Tilc::TExtString Content);
	DECLSPEC void FilePutContentsRaw(Tilc::TExtString Path, const char* Buffer, size_t BufferSize);
	DECLSPEC Tilc::TExtString FileGetContents(const Tilc::TExtString& Path);
	DECLSPEC void FileGetContents(const char* Path, char** DataBuffer, size_t* DataBufferSize);
	DECLSPEC size_t GetFileSize(const char* Path);
	DECLSPEC Tilc::TExtString GetFileBasename(const Tilc::TExtString& Path);
	DECLSPEC Tilc::TExtString GetFileExt(const Tilc::TExtString& Path);
    DECLSPEC inline Tilc::TExtString GetRealPath(const Tilc::TExtString& Path)
    {
        return std::filesystem::path(Path.c_str()).lexically_normal().string();
    }

	DECLSPEC unsigned long AppendToFile(const TExtString& fname, const TExtString& Content);
    DECLSPEC inline TExtString GetCurrentDirectory()
    {
        return std::filesystem::current_path().string();
    }
	/*
	DECLSPEC std::list<TExtString> GetDirectoryContent(const TExtString& dir);
	DECLSPEC std::list<TExtString> GetDirectoryDirs(const TExtString& dir);
	DECLSPEC BOOL CreateDirectory(const TExtString& dir, BOOL createAll = FALSE);
	// Warning!!!
	// Function below removes recursively all directory without prompting. If read-only or system files encounters
	// then removes these attributes and deletes this files. Cannot delete files if opened by another app or
	// directories that are in use.
	DECLSPEC BOOL DeleteDirectory(const TExtString& dir);
	// W poniższej funkcji rozszerzenie podajemy bez kropki
	DECLSPEC std::list<TExtString> GetDirectoryFiles(const TExtString& dir, const TExtString& ext = "");
	// Returns path extension without '.' (string after last '.')
	DECLSPEC TExtString GetExt(const TExtString& path);
	// Returns only last path part ex. only filename (with extension) or last directory.
	DECLSPEC TExtString GetPathname(const TExtString& path);
	// Returns only last path part ex. only filename or last directory without specified extension.
	DECLSPEC TExtString GetPathname(const TExtString& path, const TExtString& rtrimExt);
	// Returns only last path part ex. only filename or last directory without any extension.
	DECLSPEC TExtString GetPathnameNoExt(const TExtString& path);
	DECLSPEC BOOL PathEndsWithPathSeparator(TExtString& path);
	DECLSPEC BOOL PathStartsWithPathSeparator(TExtString& path);
	// adds extra path part to the specified path. If path not ends with PathSeparator, then the PathSeparator
	// is inserted.
	DECLSPEC VOID AddPathPart(TExtString& path, const TExtString& partPathname);
	// Splits path into array of directories. No matter if path exist.
	DECLSPEC std::list<TExtString> GetPathnameParts(const TExtString& path);
	DECLSPEC VOID TruncateEndingPathSeparator(TExtString& path);
	DECLSPEC VOID TruncateStartingPathSeparator(TExtString& path);
	DECLSPEC VOID TrimPathSeparators(TExtString& path);
	// Returns full directory name of the path. If path is the file then full parent directory is returned. Else
	// RealPath(path) is returned.
	DECLSPEC TExtString GetBasename(const TExtString& path);
	// Returns full parent directory name of the path. If path is the file then full parent directory is returned.
	// Else RealPath('path\\..') is returned.
	DECLSPEC TExtString GetParentDir(const TExtString& path);
	// if path is not a full path ex. 'c:\test.txt' but a partial path ex. 'test.txt' or '..\\..\\dir\\test.txt' then
	// returned value is: GetCurrentDirectory() + parse_and_apply('..') + if_required(\\) + path,
	// else path is returned
	DECLSPEC TExtString RealPath(const TExtString& path);
	DECLSPEC TExtString GetWindowsDirectory();
	DECLSPEC TExtString GetSystemDirectory();
	*/
}
