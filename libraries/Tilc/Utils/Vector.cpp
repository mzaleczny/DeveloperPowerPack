#include "Tilc/Utils/Vector.h"

Tilc::TVector2D<float> Tilc::Normalize(const Tilc::TVector2D<float>& v)
{
    Tilc::TVector2D<float> Result{ v };
    float Len = Result.Length();
    Result.x /= Len;
    Result.y /= Len;
    return Result;

}

Tilc::TVector3D<float> Tilc::Normalize(const Tilc::TVector3D<float>& v)
{
    Tilc::TVector3D<float> Result{ v };
    float Len = Result.Length();
    Result.x /= Len;
    Result.y /= Len;
    Result.z /= Len;
    return Result;

}

Tilc::TVector4D<float> Tilc::Normalize(const Tilc::TVector4D<float>& v)
{
    Tilc::TVector4D<float> Result{ v };
    float Len = Result.Length();
    Result.x /= Len;
    Result.y /= Len;
    Result.w /= Len;
    Result.y /= Len;
    return Result;
}

bool Tilc::PointInsideTriangle(const TVector3D<float>& Point, const TVector3D<float>& TriA, const TVector3D<float>& TriB, const TVector3D<float>& TriC)
{
    if (
        (CrossProduct(Point - TriA, TriB - TriA).z > 0.0f) &&
        (CrossProduct(Point - TriB, TriC - TriB).z > 0.0f) &&
        (CrossProduct(Point - TriC, TriA - TriC).z > 0.0f)
    )
    {
        return true;
    }
    return false;
}
