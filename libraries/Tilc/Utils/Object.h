#pragma once

#include "Tilc/Globals.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc
{

    constexpr const int32_t C_TOBJECT = 1;
    constexpr const int32_t C_TTREE = 2;

    class TObject;
    DECLSPEC void Swap(TObject** Object1, TObject** Object2);

    class DECLSPEC TObject
    {
    public:
        // konstruktor domyślny
        TObject();
        // konstruktor kopiujący
        TObject(const TObject& Object);
        virtual ~TObject();

        virtual bool Equals(TObject* Object);
        virtual Tilc::TExtString ToString();
        virtual inline bool IsString() { return false; }

        virtual operator wchar_t*();

        int64_t m_ObjectId;
        int32_t m_ClassId;

        virtual void CommonInit();
    };
}
