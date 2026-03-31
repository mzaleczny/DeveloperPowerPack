#include "Tilc/Utils/Object.h"
using namespace std;

void Tilc::Swap(TObject** Object1, TObject** Object2)
{
    TObject* tmp = *Object1;
    *Object1 = *Object2;
    *Object2 = tmp;
}




Tilc::TObject::TObject()
{
    CommonInit();
}

Tilc::TObject::TObject(const TObject& Object)
{
    CommonInit();
}

void Tilc::TObject::CommonInit()
{
    static int64_t id = 0;
    id += 1;
    m_ObjectId = id;
    m_ClassId = C_TOBJECT;
}


Tilc::TObject::~TObject()
{
}


bool Tilc::TObject::Equals(Tilc::TObject* Object)
{
    return this == Object;
}

Tilc::TExtString Tilc::TObject::ToString()
{
    return Tilc::TExtString("TObject [Id: ") + m_ObjectId + "]; ClassId: " + m_ClassId;
}

Tilc::TObject::operator wchar_t*()
{
    return nullptr;
}
