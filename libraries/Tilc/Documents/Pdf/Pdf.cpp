#include "Tilc/Documents/Pdf/Pdf.h"
#include <iostream>
#include <sstream>
#include <string>

Tilc::Documents::Pdf::TPDF::TPDF(const char* FilePath)
    : m_FilePath(FilePath)
{
    m_PdfFile.OpenFile(m_FilePath.c_str(), std::ios::in | std::ios::binary);
    ReadXRef();
    ReadRoot();
    std::cout << std::endl;
    ReadObj(m_Trailer.m_InfoI1);
}

Tilc::Documents::Pdf::TPDF::~TPDF()
{
}

void Tilc::Documents::Pdf::TPDF::ReadXRef()
{
    m_XRefTable.clear();
    // Będziemy wczytywali po 64KB danych z pliku
    size_t FileSize = m_PdfFile.GetFileSize();
    // Jesli plik jest mniejszy
    if (m_ChunkSize >= FileSize)
    {
        // to zmniejszamy rozmiar chunka do rozmiaru pliku i ustawamy się na początku pliku
        m_ChunkSize = FileSize;
        m_CurrentChunkOffset = 0;
    }
    else
    {
        // w przeciwnym razie ustawiamy się na -ChunkSize od końca liku
        m_CurrentChunkOffset = FileSize - m_ChunkSize;
    }
    m_PdfFile.m_File.seekg(m_CurrentChunkOffset, std::ios::beg);

    if (m_PdfFile.Read(m_ChunkSize, m_Content) > 0)
    {
        size_t Pos = m_Content.StrPos("\nstartxref\n", m_ChunkSize - 64);
        if (Pos != std::string::npos)
        {
            Pos += 11;
            size_t PosEnd = m_Content.StrPos("\n", Pos);
            Tilc::TExtString Tmp = m_Content.substr(Pos, PosEnd - Pos);
            if (Tmp.isInt())
            {
                m_StartXRef = std::atoi(Tmp.c_str());
                if (m_StartXRef + m_ChunkSize < FileSize)
                {
                    m_CurrentChunkOffset = m_StartXRef;
                    m_StartXRefInBuffer = 0;
                    m_PdfFile.m_File.seekg(m_CurrentChunkOffset, std::ios::beg);
                    m_PdfFile.Read(m_ChunkSize, m_Content);
                }
                else
                {
                    m_StartXRefInBuffer = m_StartXRef;
                }
                
                //std::cout << m_Content.substr(m_StartXRefInBuffer, 1024) << "\n\n" << std::endl;
                ReadXRef_ReadItems();
                //for (size_t i = 0; i < m_XRefTable.size(); ++i)
                //{
                //    std::cout << "XRefItem: " << std::get<0>(m_XRefTable[i]) << " " << std::get<1>(m_XRefTable[i]) << " " << std::get<2>(m_XRefTable[i]) << std::endl;
                //}
                ReadTrailer();
                //std::cout << m_Trailer << std::endl;
            }
        }
    }
}

int Tilc::Documents::Pdf::TPDF::ReadXRef_AdjustContentData()
{
    // Pobieramy pozycję stopki
    size_t pos = m_Content.find("trailer");
    if (pos == std::string::npos)
    {
        // Jeśli nie znaleziono jej w buforze, to nie cała Xref mieści się w buforze. Musimy więc obciąć bufor do ostatniego wystąpienia "\n", ale włączając ten znak.
        // Będziemy musieli więc wczytywać kolejne chunki danych w miarę przetwarzania bufora i xreftable
        pos = m_Content.rfind("\n");
        if (pos == std::string::npos)
        {
            return -1;
        }
        m_Content = m_Content.substr(0, pos + 1);
    }
    return 0;
}

int Tilc::Documents::Pdf::TPDF::ReadXRef_ReadItems()
{
    // Pobieramy pozycję stopki
    size_t pos = m_Content.find("xref\n");
    if (pos == std::string::npos) return -2;
    pos += 5;
    if (ReadXRef_AdjustContentData() < 0) return -1;
    Tilc::TExtString Line, XRefTableString(&m_Content[pos]);
    std::istringstream is, isLine;
    int i1, ItemsCount, Offset, i2;
    char ch;

    is.str(XRefTableString);
    std::getline(is, Line);
    isLine.str(Line);
    isLine >> i1 >> ItemsCount;
    if (ItemsCount < 1)
    {
        return -3;
    }

    m_XRefTable.reserve(ItemsCount);
    for (int i = 0; i < ItemsCount; ++i)
    {
        std::getline(is, Line);
        if (Line == "trailer")
        {
            if (i != ItemsCount - 1)
            {
                return -4;
            }
            break;
        }
        else if (Line.empty())
        {
            // doczytujemy następny chunk danych
            m_PdfFile.m_File.seekg(m_CurrentChunkOffset + m_Content.size(), std::ios::beg);
            m_CurrentChunkOffset += m_Content.size();
            m_PdfFile.Read(m_ChunkSize, m_Content);
            if (ReadXRef_AdjustContentData() < 0) return -5;
            is.str(m_Content);
            std::getline(is, Line);
        }
        isLine.str(Line);
        isLine.clear();
        isLine >> Offset >> i2 >> ch;
        m_XRefTable.emplace_back(Offset, i2, ch);
    }

    return 0;
}

int Tilc::Documents::Pdf::TPDF::ReadTrailer()
{
    size_t pos = m_Content.find("trailer\n");
    size_t TrailerStartPos = pos;
    while (pos == std::string::npos)
    {
        m_PdfFile.m_File.seekg(m_CurrentChunkOffset + m_Content.size(), std::ios::beg);
        m_CurrentChunkOffset += m_Content.size();
        m_PdfFile.Read(m_ChunkSize, m_Content);
        pos = m_Content.find("trailer\n");
        if (m_Content.empty())
        {
            break;
        }
    }
    if (pos == std::string::npos)
    {
        return -1;
    }

    pos = m_Content.find(">>", pos);
    if (pos == std::string::npos)
    {
        m_PdfFile.m_File.seekg(m_CurrentChunkOffset + TrailerStartPos, std::ios::beg);
        m_CurrentChunkOffset += TrailerStartPos;
        TrailerStartPos = 0;
        m_PdfFile.Read(m_ChunkSize, m_Content);
        pos = m_Content.find("trailer\n");
    }
    pos = m_Content.find(">>");
    if (pos == std::string::npos)
    {
        return -2;
    }

    Tilc::TExtString TrailerString(&m_Content[TrailerStartPos]);
    std::istringstream is(TrailerString), isLine;
    Tilc::TExtString Line, Item;
    while (std::getline(is, Line))
    {
        if (Line.StartsWith("<</Size"))
        {
            isLine.str(Line);
            isLine.clear();
            isLine >> Item; // "<</Size"
            isLine >> Item;
            // zmienna poniżej powinna zawierać ilość obiektów czyli wpisów w tabeli m_XRefTable
            m_Trailer.m_SizeI1 = ReadIntValueFromPairBefore(Item, "Root");
            if (m_Trailer.m_SizeI1 != m_XRefTable.size())
            {
                return -3;
            }
            isLine >> m_Trailer.m_RootObjectNumber >> m_Trailer.m_SizeI2 >> m_Trailer.m_SizeI3;
        }
        else if (Line.StartsWith("/Info"))
        {
            isLine.str(Line);
            isLine.clear();
            isLine >> Item; // "/Info"
            isLine >> m_Trailer.m_InfoI1 >> m_Trailer.m_InfoI2 >> m_Trailer.m_InfoI3;
        }
        else if (Line.StartsWith("/ID"))
        {
            isLine.str(Line);
            isLine.clear();

            isLine >> Item; // "/ID"
            isLine >> Item; // "["
            isLine >> Item; // "ID1"
            Item.StrReplace("<", "");
            Item.StrReplace(">", "");
            m_Trailer.m_ID[0] = Item;

            std::getline(is, Line);
            isLine.str(Line);
            isLine.clear();

            isLine >> Item; // "ID2"
            Item.StrReplace("<", "");
            Item.StrReplace(">", "");
            m_Trailer.m_ID[1] = Item;
        }
        else if (Line.StartsWith("/DocChecksum"))
        {
            isLine.str(Line);
            isLine.clear();

            isLine >> Item; // "/DocCheckSum"
            isLine >> Item; // "ID"
            Item.StrReplace("/", "");
            m_Trailer.m_DocChecksum = Item;
        }
    }
    return 0;
}

void Tilc::Documents::Pdf::TPDF::ReadObj(int ObjNum)
{
    //std::cout << "ObjNum: " << ObjNum << " / " << m_XRefTable.size() << std::endl;
    if (ObjNum >= 0 && ObjNum < m_XRefTable.size())
    {
        Tilc::Documents::Pdf::TPdfXRefItem& Root = m_XRefTable[ObjNum];
        int Offset = std::get<0>(Root);
        if (Offset < m_CurrentChunkOffset || Offset >= m_CurrentChunkOffset + m_Content.size())
        {
            m_PdfFile.m_File.seekg(Offset, std::ios::beg);
            m_CurrentChunkOffset = Offset;
            Offset = 0;
            m_PdfFile.Read(m_ChunkSize, m_Content);
        }
        //std::cout << "Offset: " << Offset << std::endl;
        size_t EndObjPos = m_Content.find("endobj", Offset);
        if (EndObjPos != std::string::npos)
        {
            std::cout << m_Content.substr(Offset, EndObjPos + 6 - Offset) << std::endl;
        }
    }
}

void Tilc::Documents::Pdf::TPDF::ReadRoot()
{
    ReadObj(m_Trailer.m_RootObjectNumber);
}
