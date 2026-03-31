#include "Tilc/Utils/FileUtils.h"
#include <iostream>
#include <sstream>
#include <fstream>

Tilc::TFile::TFile(const char* FileName, std::ios_base::openmode OpenMode)
{
    std::lock_guard<std::mutex> lock(TFile::m_MU);
    OpenFile(FileName, OpenMode);
}

Tilc::TFile::~TFile()
{
    std::lock_guard<std::mutex> lock(TFile::m_MU);
    m_File.close();
}

void Tilc::TFile::ReadLines(std::vector<Tilc::TExtString>& Lines)
{
    Lines.clear();
    if (m_File)
    {
        std::string Line;
        m_File.seekg(0, std::ios::beg);
        while (!m_File.eof())
        {
            std::getline(m_File, Line);
            Lines.push_back(Line);
        }
        m_File.clear();
    }
}

void Tilc::TFile::ReadContent(Tilc::TExtString& Content)
{
    Content = "";
    if (m_File)
    {
        m_File.seekg(0, std::ios_base::end);
        size_t DataBufferSize = m_File.tellg();
        if (DataBufferSize > 0)
        {
            Content.resize(DataBufferSize);
            m_File.seekg(0, std::ios_base::beg);
            m_File.read(Content.data(), DataBufferSize);
            m_File.clear();
        }
    }
}

void Tilc::TFile::WriteContent(const Tilc::TExtString& Content)
{
    if (m_File)
    {
        m_File.seekg(0, std::ios_base::beg);
        m_File.write(Content.data(), Content.size());
        m_File.clear();
    }
}

void Tilc::TFile::AppendContent(const Tilc::TExtString& Content)
{
    if (m_File)
    {
        m_File.seekg(0, std::ios_base::end);
        m_File.write(Content.data(), Content.size());
        m_File.clear();
    }
}

void Tilc::TFile::OpenFile(const char* FileName, std::ios_base::openmode OpenMode)
{
    m_File.open(FileName, OpenMode);
    if (!m_File)
    {
        std::cerr << "Could not open file " << FileName << ". Skipping..." << std::endl;
    }
}



Tilc::TCFile::TCFile(const char* FileName, const char* OpenMode)
{
    std::lock_guard<std::mutex> lock(TCFile::m_MU);
    OpenFile(FileName, OpenMode);
}

Tilc::TCFile::~TCFile()
{
    std::lock_guard<std::mutex> lock(TCFile::m_MU);
    fclose(m_File);
}

char* Tilc::TCFile::ReadLine(Tilc::TExtString& Line)
{
    char* Result = nullptr;
    Line = "";
    if (m_File)
    {
        char buf[256];
        while ((Result = fgets(buf, sizeof(buf), m_File)))
        {
            Line += buf;
            char LastCh = Line[Line.length() - 1];
            if (LastCh == '\n' || LastCh == '\r')
            {
                break;
            }
        }
    }
    Line.RTrim();
    return Result;
}

void Tilc::TCFile::ReadLines(std::vector<Tilc::TExtString>& Lines)
{
    Lines.clear();
    if (m_File)
    {
        Tilc::TExtString Line;
        fseek(m_File, 0, SEEK_SET);
        while (ReadLine(Line))
        {
            Lines.push_back(Line);
        }
    }
}

void Tilc::TCFile::ReadContent(Tilc::TExtString& Content)
{
    Content = "";
    if (m_File)
    {
        fseek(m_File, 0, SEEK_END);
        long DataBufferSize = ftell(m_File);
        if (DataBufferSize > 0)
        {
            Content.resize(DataBufferSize);
            fseek(m_File, 0, SEEK_SET);
            fread(Content.data(), 1, DataBufferSize, m_File);
        }
    }
}

void Tilc::TCFile::WriteContent(const Tilc::TExtString& Content)
{
    if (m_File)
    {
        fseek(m_File, 0, SEEK_SET);
        fwrite(Content.data(), 1, Content.size(), m_File);
    }
}

void Tilc::TCFile::OpenFile(const char* FileName, const char* OpenMode)
{
    m_File = fopen(FileName, OpenMode);
    if (!m_File)
    {
        std::cerr << "Error! Could not open file " << FileName << std::endl;
    }
}


bool Tilc::DirectoryExists(const Tilc::TExtString& Path)
{
    return std::filesystem::is_directory(std::filesystem::path(Path.c_str()));
}

bool Tilc::FileExists(const Tilc::TExtString& Path)
{
    return std::filesystem::is_regular_file(std::filesystem::path(Path.c_str()));
}

Tilc::TExtString Tilc::GetAppDataFolder()
{
#ifdef _WINDOWS
    constexpr const char* HOME = "USERPROFILE";
#else
    constexpr const char* HOME = "HOME";
#endif

    if (!getenv(HOME)) return ".";
    Tilc::TExtString HomeDir = getenv(HOME);

#ifdef _WINDOWS
    HomeDir += "\\AppData\\Local";
#else
    HomeDir += "/.local/bin";
#endif
    
    return std::move(HomeDir);
}

Tilc::TExtString Tilc::GetTmpFolder()
{
#ifdef _WINDOWS
    constexpr const char* HOME = "USERPROFILE";
#else
    constexpr const char* HOME = "HOME";
#endif

    if (!getenv(HOME)) return ".";
    Tilc::TExtString HomeDir = getenv(HOME);

#ifdef _WINDOWS
    HomeDir += "\\AppData\\Local\\Temp";
#else
    HomeDir += "/tmp";
#endif
    
    return std::move(HomeDir);
}

void Tilc::FilePutContents(const Tilc::TExtString& Path, const Tilc::TExtString& Content)
{
    std::ofstream File(Path.c_str());
    if (File)
    {
        File.write(Content.c_str(), Content.length());
        File.close();
    }
}

void Tilc::FilePutContentsRaw(Tilc::TExtString Path, Tilc::TExtString Content)
{
    std::fstream File(Path.c_str(), std::ios::out | std::ios::binary);
    if (File)
    {
        File.write(Content.data(), Content.size());
        File.close();
    }
}

void Tilc::FilePutContentsRaw(Tilc::TExtString Path, const char* Buffer, size_t BufferSize)
{
    std::fstream File(Path.c_str(), std::ios::out | std::ios::binary);
    if (File)
    {
        File.write(Buffer, BufferSize);
        File.close();
    }
}

Tilc::TExtString Tilc::FileGetContents(const Tilc::TExtString& Path)
{
    std::ifstream File(Path.c_str());
    if (File)
    {
        std::ostringstream ss;
        ss << File.rdbuf();
        File.close();
        return ss.str();
    }
    return "";
}

void Tilc::FileGetContents(const char* Path, char** DataBuffer, size_t* DataBufferSize)
{
    std::fstream File(Path, std::ios::in | std::ios::binary);
    File.seekg(0, std::ios_base::end);
    *DataBufferSize = File.tellg();
    if (*DataBufferSize == 0)
    {
        *DataBuffer = nullptr;
        return;
    }
    *DataBuffer = new char[*DataBufferSize];
    if (*DataBuffer)
    {
        File.seekg(0, std::ios_base::beg);
        File.read(*DataBuffer, *DataBufferSize);
    }
    File.close();
}

size_t Tilc::GetFileSize(const char* Path)
{
    size_t FileSize;
    std::fstream File(Path, std::ios::in | std::ios::binary);
    File.seekg(0, std::ios_base::end);
    FileSize = File.tellg();
    File.close();
    return FileSize;
}

Tilc::TExtString Tilc::GetFileBasename(const Tilc::TExtString& Path)
{
    size_t Pos = Path.find_last_of('\\');
    if (Pos != std::string::npos)
    {
        return Path.substr(Pos + 1);
    }
    return "";
}

Tilc::TExtString Tilc::GetFileExt(const Tilc::TExtString& Path)
{
    size_t Pos = Path.find_last_of('.');
    if (Pos != std::string::npos)
    {
        return Path.substr(Pos + 1);
    }
    return "";
}

unsigned long Tilc::AppendToFile(const TExtString& fname, const TExtString& Content)
{
    Tilc::TFile File(fname.c_str(), static_cast<std::ios_base::openmode>(std::ios::binary | std::ios::end));
    File.AppendContent(Content);
    return Content.size();
}

/*
CmzStringList* Tilc::GetDirectoryContent(TExtString dir) {
    WIN32_FIND_DATAW fd;
    void* hDir = INVALID_HANDLE_VALUE;
    unsigned long dwError = 0;
    CmzStringList *lstDirs = NULL, *lstFiles = NULL;
    CmzStringList *lstAll = NULL;

    if (dir.length() > MAX_PATH) {
        return NULL;
    }

    if (dir.lastChar() != L'\\') {
        dir += L'\\';
    }

    if (dir.lastChar() != L'*') {
        dir += L'*';
    }

    // Find the first file in the directory.
    hDir = FindFirstFileW(dir.getBuffer(), &fd);
    if (hDir == INVALID_HANDLE_VALUE) {
        ShowLastSystemError();
        return NULL;
    }

    lstDirs = new CmzStringList();
    lstFiles = new CmzStringList();
    lstAll = new CmzStringList();
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            lstDirs->append(CmzString(fd.cFileName));
        } else {
            lstFiles->append(CmzString(fd.cFileName));
        }
    } while (FindNextFileW(hDir, &fd) != 0);

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        FindClose(hDir);
        ShowLastSystemError();
    }

    FindClose(hDir);

    lstAll->append(lstDirs);
    lstAll->append(lstFiles);

    delete lstDirs;
    delete lstFiles;

    return lstAll;
}

CmzStringList* Tilc::GetDirectoryDirs(TExtString dir) {
    WIN32_FIND_DATAW fd;
    void* hDir = INVALID_HANDLE_VALUE;
    unsigned long dwError = 0;
    CmzStringList *lstDirs = NULL;

    if (dir.length() > MAX_PATH) {
        return NULL;
    }

    if (dir.lastChar() != L'\\') {
        dir += L'\\';
    }

    if (dir.lastChar() != L'*') {
        dir += L'*';
    }

    // Find the first file in the directory.
    hDir = FindFirstFileW(dir.getBuffer(), &fd);
    if (hDir == INVALID_HANDLE_VALUE) {
        ShowLastSystemError();
        return NULL;
    }

    lstDirs = new CmzStringList();
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            lstDirs->append(CmzString(fd.cFileName));
        }
    } while (FindNextFileW(hDir, &fd) != 0);

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        FindClose(hDir);
        ShowLastSystemError();
    }

    FindClose(hDir);

    return lstDirs;
}

CmzStringList* Tilc::GetDirectoryFiles(TExtString dir, TExtString ext) {
    WIN32_FIND_DATAW fd;
    void* hDir = INVALID_HANDLE_VALUE;
    unsigned long dwError = 0;
    CmzStringList *lstFiles = NULL;
    BOOL checkExtension = ext.length() > 0;

    if (dir.length() > MAX_PATH) {
        return NULL;
    }

    if (dir.lastChar() != L'\\') {
        dir += L'\\';
    }

    if (dir.lastChar() != L'*') {
        dir += L'*';
    }

    // Find the first file in the directory.
    hDir = FindFirstFileW(dir.getBuffer(), &fd);
    if (hDir == INVALID_HANDLE_VALUE) {
        ShowLastSystemError();
        return NULL;
    }

    lstFiles = new CmzStringList();
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            if (checkExtension) {
                if (ext == CmzString(fd.cFileName).strAfterLast(L'.')) {
                    lstFiles->append(CmzString(fd.cFileName));
                }
            } else {
                lstFiles->append(CmzString(fd.cFileName));
            }
        }
    } while (FindNextFileW(hDir, &fd) != 0);

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        FindClose(hDir);
        ShowLastSystemError();
    }

    FindClose(hDir);

    return lstFiles;
}

TExtString Tilc::GetExt(TExtString path) {
    return path.strAfterLast(L'.');
}

TExtString Tilc::GetPathname(TExtString path) {
    // if we have only disk drive path ex. 'C:\\', then we return empty string
    if (path.length() < 4) {
        return COMMON_EMPTY_STRING;
    }

    if (path.contains(L'/') || path.contains(L'\\')) {
        return path.strAfterLastChar(L"/\\");
    }
    return path;
}

TExtString Tilc::GetPathname(TExtString path, TExtString rtrimExt) {
    TExtString result = GetPathname(path);

    if (result == rtrimExt) {
        return COMMON_EMPTY_STRING;
    }

    size_t resultLen = result.length();
    size_t rtrimextLen = rtrimExt.length();

    if (resultLen == rtrimextLen) {
        return result;
    }

    if (resultLen > rtrimextLen) {
        wchar_t* pend = result.getBuffer();
        pend = pend + resultLen - rtrimextLen;
        if (rtrimExt == pend) {
            return result.substr(0, resultLen - rtrimextLen);
        }
        return result;
    }

    return COMMON_EMPTY_STRING;
}

TExtString Tilc::GetPathnameNoExt(TExtString path) {
    TExtString result = GetPathname(path);
    TExtString rtrimExt = CmzString(L".") + GetExt(result);

    return GetPathname(result, rtrimExt);
}

TExtString Tilc::GetBasename(TExtString path) {
    TExtString realPath = RealPath(path);
    if (FileExists(realPath)) {
        realPath.cutAtLastChar(L"/\\");
        return realPath;
    } else {
        if (DirectoryExists(realPath)) {
            return realPath;
        }

        // Not existing path - if has dot then assuming file else directory
        if (realPath.contains(L".")) {
            realPath.cutAtLastChar(L"/\\");
        }

        return realPath;
    }

    // we should not be here
    return realPath;
}

TExtString Tilc::GetParentDir(TExtString path) {
    if (path == L"\\\\") {
        return COMMON_EMPTY_STRING;
    }
    size_t pathLen = path.length();
    if (pathLen == 1 || ((pathLen == 2 || pathLen == 3) && path.getCharAt(1) == L':')) {
        return COMMON_EMPTY_STRING;
    }

    TExtString realPath = RealPath(path);
    realPath.cutAtLastChar(L"/\\");
    wchar_t* buf = realPath.getBuffer();
    size_t bufLen = wcslen(buf);

    if (bufLen == 1 && buf[0] == L'\\') {
        realPath += L'\\';
        return realPath;
    }

    if (bufLen == 2 && buf[1] == L':') {
        realPath += L'\\';
        return realPath;
    }

    return realPath;
}

VOID Tilc::TruncateEndingPathSeparator(CmzString& path) {
    path.rtrimAllChars(L"/\\");
}

VOID Tilc::TruncateStartingPathSeparator(CmzString& path) {
    path.ltrimAllChars(L"/\\");
}

VOID Tilc::TrimPathSeparators(CmzString& path) {
    path.rtrimAllChars(L"/\\");
    path.ltrimAllChars(L"/\\");
}

TExtString Tilc::RealPath(TExtString path) {
    wchar_t* buf = path.getBuffer();
    if (!buf || buf[0] == NULL) {
        return path;
    }

    CmzStringList* pathParts = GetPathnameParts(path);
    unsigned long pathPartsCount = pathParts->size();

    TExtString resultPath = COMMON_EMPTY_STRING;
    unsigned long i = 0;
    TExtString ppart;

    while (i < pathPartsCount) {
        ppart = pathParts->getAt(i);
        i++;

        if (resultPath == COMMON_EMPTY_STRING) {
            if (ppart == L"\\\\") {
                resultPath = ppart;
                continue;
            }
            if (ppart.length() > 1 && ppart.getCharAt(1) == L':') {
                if (ppart.length() > 2 && ppart.getCharAt(2) == L'\\') {
                    resultPath = ppart;
                    continue;
                }
                resultPath = ppart + L"\\";
                continue;
            }

            resultPath = GetCurrentDirectory() + L"\\" + ppart;
            continue;
        }

        if (ppart == L".") {
            continue;
        }

        TruncateEndingPathSeparator(resultPath);

        if (ppart == L"..") {
            if (resultPath.containsOneOfChars(L"/\\")) {
                resultPath.cutAtLastChar(L"/\\");
            }
            continue;
        }

        resultPath += CmzString(L"\\") + ppart;
    }
    delete pathParts;

    return resultPath;
}

CmzStringList* Tilc::GetPathnameParts(TExtString path) {
    if (path.startsWith(L"\\\\")) {
        TExtString sub = path.substr(2);
        CmzStringList* lst = sub.explodeAtChar(L"/\\");
        lst->addAsFirst(L"\\\\");
        return lst;
    }

    return path.explodeAtChar(L"/\\");
}

BOOL Tilc::CreateDirectory(TExtString dir, BOOL createAll) {
    if (!createAll) {
        CreateDirectoryW((LPCWSTR)dir.getBuffer(), (LPSECURITY_ATTRIBUTES)NULL);
        if (!DirectoryExists(dir)) {
            return FALSE;
        }
        return TRUE;
    }

    TExtString realPath = RealPath(dir);
    CmzStringList* dirs = GetPathnameParts(realPath);
    if (!dirs) {
        return FALSE;
    }
    long count = dirs->size();

    if (count > 0) {
        TExtString partDir = dirs->getAt(0);
        if (!PathEndsWithPathSeparator(partDir)) {
            partDir += L"\\";
        }

        for (long i = 1; i < count; i++) {
            if (i == 1) {
                partDir += dirs->getAt(i);
            } else {
                partDir += CmzString(L"\\") + dirs->getAt(i);
            }

            if (!DirectoryExists(partDir)) {
                CreateDirectoryW((LPCWSTR)partDir.getBuffer(), (LPSECURITY_ATTRIBUTES)NULL);
                if (!DirectoryExists(partDir)) {
                    delete dirs;
                    return FALSE;
                }
            }
        }

        delete dirs;
        return TRUE;
    }

    delete dirs;
    return FALSE;
}

BOOL Tilc::DeleteDirectory(TExtString dir) {
    WIN32_FIND_DATAW fd;
    void* hDir = INVALID_HANDLE_VALUE;
    unsigned long dwError = 0;
    BOOL result = TRUE;

    TExtString allFilesMask = dir;
    TExtString item;

    if (!DirectoryExists(dir)) {
        return FALSE;
    }

    AddPathPart(allFilesMask, L"*");
    hDir = FindFirstFileW(allFilesMask.getBuffer(), &fd);
    if (hDir == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) {
                continue;
            }
            item = dir;
            AddPathPart(item, fd.cFileName);
            //wprintf(L"Deleting subdirectory: %s\n", item.getBuffer());
            DeleteDirectory(item);
        } else {
            item = dir;
            AddPathPart(item, fd.cFileName);
            SetFileAttributesW(item.getBuffer(), FILE_ATTRIBUTE_NORMAL);
            //wprintf(L"Deleting file: %s\n", item.getBuffer());
            DeleteFileW(item.getBuffer());
        }
    } while (FindNextFileW(hDir, &fd) != 0);
    FindClose(hDir);

    //wprintf(L"Deleting directory: %s\n", dir.getBuffer());
    result = RemoveDirectoryW(dir.getBuffer());
    return result;
}

BOOL Tilc::PathEndsWithPathSeparator(CmzString& path) {
    return path.endsWith(L"/\\");
}

BOOL Tilc::PathStartsWithPathSeparator(CmzString& path) {
    return path.startsWith(L"/\\");
}

VOID Tilc::AddPathPart(CmzString& path, TExtString partPathname) {
    TrimPathSeparators(partPathname);
    if (PathEndsWithPathSeparator(path)) {
        path += partPathname;
    } else {
        path += CmzString(L"\\") + partPathname;
    }
}

TExtString Tilc::GetWindowsDirectory() {
    wchar_t buf[MAX_PATH];
    ::GetWindowsDirectoryW((wchar_t*)buf, MAX_PATH);
    return buf;
}

TExtString Tilc::GetSystemDirectory() {
    wchar_t buf[MAX_PATH];
    ::GetSystemDirectoryW((wchar_t*)buf, MAX_PATH);
    return buf;
}
*/
