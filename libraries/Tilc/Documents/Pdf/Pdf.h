#pragma once


#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/FileUtils.h"

#include <iostream>
#include <tuple>

namespace Tilc
{
    namespace Documents
    {
        namespace Pdf
        {
            using TPdfXRefItem = std::tuple<int, int, char>;
            struct TPdfTrailer
            {
                int m_RootObjectNumber;
                int m_SizeI1, m_SizeI2;
                char m_SizeI3;
                int m_InfoI1, m_InfoI2;
                char m_InfoI3;
                Tilc::TExtString m_ID[2];
                Tilc::TExtString m_DocChecksum;

                friend std::ostream& operator<<(std::ostream& out, const TPdfTrailer& trailer)
                {
                    out << "Size: " << trailer.m_SizeI1 << "/Root" << " " << trailer.m_RootObjectNumber << " " << trailer.m_SizeI2 << " " << trailer.m_SizeI3 << std::endl;
                    out << "Info: " << trailer.m_InfoI1 << " " << trailer.m_InfoI2 << " " << trailer.m_InfoI3 << std::endl;
                    out << "ID: " << trailer.m_ID[0] << " " << trailer.m_ID[1] << std::endl;
                    out << "DocCheckSum: " << trailer.m_DocChecksum;
                    return out;
                }
            };

            class DECLSPEC TPDF
            {
            public:
                TPDF(const char* FilePath);
                virtual ~TPDF();
            protected:
                Tilc::TFile m_PdfFile;
                // offset of XRef table in pdf file
                size_t m_StartXRef{};
                // offset of XRef table in read in buffer stored in m_Content
                size_t m_StartXRefInBuffer;
                // offset of current chunk read in and stored in m_Content related to begin of pdf file
                size_t m_CurrentChunkOffset{};
                size_t m_ChunkSize{ 64 * 1024 };
                // File path that is being processed
                Tilc::TExtString m_FilePath;
                // currrent chunk, data buffer read in from pdf file
                Tilc::TExtString m_Content;
                // XRefTable items
                std::vector<TPdfXRefItem> m_XRefTable;
                TPdfTrailer m_Trailer;

                inline int ReadIntValueFromPairBefore(Tilc::TExtString Item, const Tilc::TExtString& ValName)
                {
                    if (size_t pos = Item.find("/" + ValName); pos != std::string::npos)
                    {
                        Item = Item.substr(0, pos);
                        return std::atoi(Item.c_str());
                    }
                    return static_cast<int>(std::string::npos);
                }
                // Read XRefTable
                void ReadXRef();
                int ReadXRef_AdjustContentData();
                int ReadXRef_ReadItems();

                int ReadTrailer();

                void ReadObj(int ObjNum);
                void ReadRoot();
            };
        }
    }
}
