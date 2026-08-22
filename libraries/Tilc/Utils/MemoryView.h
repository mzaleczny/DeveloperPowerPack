#pragma once

#include <ostream>
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc
{
    class DECLSPEC TMemoryView
    {
    public:
        TMemoryView(void* pointer, size_t size, size_t offset);
        virtual ~TMemoryView();

        void SetMemory(void* pointer, size_t size);
        Tilc::TExtString GetStringOfBytes(bool WithOffsets = true, bool WithASCII = true);
        Tilc::TExtString GetStringOfWords(bool WithOffsets = true, bool WithASCII = true);
        Tilc::TExtString GetStringOfDwords(bool WithOffsets = true, bool WithASCII = true);

        std::ostream& Print(std::ostream& out, Tilc::TExtString indent = "", bool DumpAsBytes = false, bool DumpAsWords = false, bool DumpAsDwords = true);
        Tilc::TExtString GetDump(Tilc::TExtString indent = "", bool DumpAsBytes = false, bool DumpAsWords = false, bool DumpAsDwords = true);
        virtual Tilc::TExtString ToString();

    private:
        void* m_Pointer;
        size_t m_Size;
        size_t m_Offset;
    };
}
