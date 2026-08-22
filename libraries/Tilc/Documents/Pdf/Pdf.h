#pragma once


#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc
{
    namespace Documents
    {
        namespace Pdf
        {
            class DECLSPEC TPDF
            {
            public:
                TPDF(const char* FilePath);
                virtual ~TPDF();
            protected:
                size_t m_StartXRef{}, m_StartXRefInBuffer;
                size_t m_CurrentChunkOffset{};
                size_t m_ChunkSize{ 64 * 1024 };
                Tilc::TExtString m_FilePath;
                Tilc::TExtString m_Content;
                void ReadXRef();
            };
        }
    }
}
