#include "Tilc/Documents/Pdf/Pdf.h"
#include "Tilc/Utils/FileUtils.h"
#include <iostream>

Tilc::Documents::Pdf::TPDF::TPDF(const char* FilePath)
    : m_FilePath(FilePath)
{
    ReadXRef();
}

Tilc::Documents::Pdf::TPDF::~TPDF()
{
    std::cout << "~TPDF()" << std::endl;
}

void Tilc::Documents::Pdf::TPDF::ReadXRef()
{
    Tilc::TFile PdfFile(m_FilePath.c_str(), std::ios::in | std::ios::binary);
    // Będziemy wczytywali po 64KB danych z pliku
    size_t FileSize = PdfFile.GetFileSize();
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
    PdfFile.m_File.seekg(m_CurrentChunkOffset, std::ios::beg);

    if (PdfFile.Read(m_ChunkSize, m_Content) > 0)
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
                    PdfFile.m_File.seekg(m_CurrentChunkOffset, std::ios::beg);
                    PdfFile.Read(m_ChunkSize, m_Content);
                }
                else
                {
                    m_StartXRefInBuffer = m_StartXRef;
                }
                
                std::cout << m_Content.substr(m_StartXRefInBuffer, 1024) << std::endl;
            }
        }
    }
}
