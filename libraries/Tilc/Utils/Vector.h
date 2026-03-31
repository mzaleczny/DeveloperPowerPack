#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

#include "Tilc/DllGlobals.h"

namespace Tilc
{
    template <typename T>
    class TVector2D
    {
    public:
        union
        {
            T Coord[2];
            struct
            {
                T x;
                T y;
            };
        };

        TVector2D() : x{}, y{} {};
        TVector2D(T _x, T _y) : x(_x), y(_y) {}
        TVector2D(const TVector2D<T>& v) : x(v.x), y(v.y) {}

        float& operator[](int i)
        {
            return ((&x)[i]);
        }
        const float& operator[](int i) const
        {
            return ((&x)[i]);
        }

        TVector2D<T>& operator=(const TVector2D<T>& V)
        {
            x = V.x;
            y = V.y;
            return *this;
        }

        bool operator==(const TVector2D<T>& V) const
        {
            return (x == V.x) && (y == V.y);
        }

        TVector2D<T> operator-() const
        {
            return { -x, -y };
        }

        TVector2D<T> operator+(const TVector2D<T>& V) const
        {
            return { x + V.x, y + V.y };
        }

        TVector2D<T> operator-(const TVector2D<T>& V) const
        {
            return { x - V.x, y - V.y };
        }

        TVector2D<T> operator*(float Value) const
        {
            return { x * Value, y * Value };
        }
        TVector2D<T>& operator*=(float Value) const
        {
            x *= Value;
            y *= Value;
            return *this;
        }
        TVector2D<T> operator/(float Value) const
        {
            Value = 1.0f / Value;
            return { x * Value, y * Value };
        }
        TVector2D<T>& operator/=(float Value) const
        {
            Value = 1.0f / Value;
            x *= Value;
            y *= Value;
            return *this;
        }

        float Length() const
        {
            return sqrtf(x * x + y * y);
        }

        float SquaredLength() const
        {
            return x * x + y * y;
        }

        TVector2D<T>& Normalize()
        {
            float Len = Length();
            x /= Len;
            y /= Len;
            return *this;
        }

        TVector2D<T>& Negate()
        {
            x = -x;
            y = -y;
            return *this;
        }
    };


    template <typename T>
    class TVector3D
    {
    public:
        union
        {
            T Coord[3];
            struct
            {
                T x;
                T y;
                T z;
            };
        };

        float& operator[](int i)
        {
            return ((&x)[i]);
        }
        const float& operator[](int i) const
        {
            return ((&x)[i]);
        }

        TVector3D() : x{}, y{}, z{} {};
        TVector3D(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
        TVector3D(const TVector3D<T>& v) : x(v.x), y(v.y), z(v.z) {}

        TVector3D<T>& operator=(const TVector3D<T>& V)
        {
            x = V.x;
            y = V.y;
            z = V.z;
            return *this;
        }

        bool operator==(const TVector3D<T>& V) const
        {
            return (x == V.x) && (y == V.y) && (z == V.z);
        }

        TVector3D<T> operator-() const
        {
            return { -x, -y, -z };
        }

        TVector3D<T> operator+(const TVector3D<T>& V) const
        {
            return { x + V.x, y + V.y, z + V.z };
        }

        TVector3D<T> operator-(const TVector3D<T>& V) const
        {
            return { x - V.x, y - V.y, z - V.z };
        }
        
        TVector3D<T> operator*(float Value) const
        {
            return { x * Value, y * Value, z * Value };
        }
        TVector3D<T>& operator*=(float Value) const
        {
            x *= Value;
            y *= Value;
            z *= Value;
            return *this;
        }
        TVector3D<T> operator/(float Value) const
        {
            Value = 1.0f / Value;
            return { x * Value, y * Value, z * Value };
        }
        TVector3D<T>& operator/=(float Value) const
        {
            Value = 1.0f / Value;
            x *= Value;
            y *= Value;
            z *= Value;
            return *this;
        }

        TVector3D<T>& operator+=(const TVector3D<T>& V)
        {
            x += V.x;
            y += V.y;
            z += V.z;
            return *this;
        }

        float Length() const
        {
            return sqrtf(x * x + y * y + z * z);
        }

        float SquaredLength() const
        {
            return x * x + y * y + z * z;
        }

        TVector3D<T>& Normalize()
        {
            float Len = Length();
            x /= Len;
            y /= Len;
            z /= Len;
            return *this;
        }

        TVector3D<T>& Negate()
        {
            x = -x;
            y = -y;
            z = -z;
            return *this;
        }
    };


    template <typename T>
    class TVector4D
    {
    public:
        union
        {
            T Coord[4];
            struct
            {
                T x;
                T y;
                T z;
                T w;
            };
        };

        TVector4D() : x{}, y{}, z{}, w{} {};
        TVector4D(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
        TVector4D(const TVector4D<T>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

        float& operator[](int i)
        {
            return ((&x)[i]);
        }
        const float& operator[](int i) const
        {
            return ((&x)[i]);
        }

        TVector4D<T>& operator=(const TVector4D<T>& V)
        {
            x = V.x;
            y = V.y;
            z = V.z;
            w = V.w;
            return *this;
        }

        operator TVector3D<T> () const
        {
            return { x, y, z };
        }

        bool operator==(const TVector4D<T>& V) const
        {
            return (x == V.x) && (y == V.y) && (z == V.z) && (w == V.w);
        }
        
        TVector4D<T> operator-() const
        {
            return { -x, -y, -z, -w };
        }

        TVector4D<T> operator+(const TVector4D<T>& V) const
        {
            return { x + V.x, y + V.y, z + V.z, w + V.w };
        }

        TVector4D<T> operator-(const TVector4D<T>& V) const
        {
            return { x - V.x, y - V.y, z - V.z, w - V.w };
        }
        
        TVector4D<T> operator*(float Value) const
        {
            return { x * Value, y * Value, z * Value, w * Value };
        }
        TVector3D<T>& operator*=(float Value) const
        {
            x *= Value;
            y *= Value;
            z *= Value;
            w *= Value;
            return *this;
        }
        TVector4D<T> operator/(float Value) const
        {
            Value = 1.0f / Value;
            return { x * Value, y * Value, z * Value, w * Value };
        }
        TVector3D<T>& operator/=(float Value) const
        {
            Value = 1.0f / Value;
            x *= Value;
            y *= Value;
            z *= Value;
            w *= Value;
            return *this;
        }

        float Length() const
        {
            return sqrtf(x * x + y * y + z * z + w * w);
        }

        float SquaredLength() const
        {
            return x * x + y * y + z * z + w * w;
        }

        TVector4D<T>& Normalize()
        {
            float Len = Length();
            x /= Len;
            y /= Len;
            z /= Len;
            w /= Len;
            return *this;
        }

        TVector4D<T>& Negate()
        {
            x = -x;
            y = -y;
            z = -z;
            w = -w;
            return *this;
        }
    };




    template <typename T>
    inline TVector2D<T> operator -(const TVector2D<T>& v)
    {
        return { -v.x, -v.y };
    }

    template <typename T>
    inline TVector3D<T> operator -(const TVector3D<T>& v)
    {
        return { -v.x, -v.y, -v.z };
    }

    template <typename T>
    inline TVector4D<T> operator -(const TVector4D<T>& v)
    {
        return { -v.x, -v.y, -v.z, -v.w };
    }

    template <typename T>
    float DotProduct(const TVector2D<T>& V1, const TVector2D<T>& V2)
    {
        return V1.x * V2.x + V1.y * V2.y;
    }

    template <typename T>
    float DotProduct(const TVector3D<T>& V1, const TVector3D<T>& V2)
    {
        return V1.x * V2.x + V1.y * V2.y + V1.z * V2.z;
    }

    template <typename T>
    float DotProduct(const TVector4D<T>& V1, const TVector4D<T>& V2)
    {
        return V1.x * V2.x + V1.y * V2.y + V1.z * V2.z + V1.w * V2.w;
    }

    template <typename T>
    float CosinusOfAngleBetweenVectors(const TVector2D<T>& V1, const TVector2D<T>& V2)
    {
        return DotProduct(V1, V2) / (V1.Length() * V2.Length());
    }

    template <typename T>
    float CosinusOfAngleBetweenNormalizedVectors(const TVector2D<T>& V1, const TVector2D<T>& V2)
    {
        return DotProduct(V1, V2);
    }

    template <typename T>
    float CosinusOfAngleBetweenVectors(const TVector3D<T>& V1, const TVector3D<T>& V2)
    {
        return DotProduct(V1, V2) / (V1.Length() * V2.Length());
    }

    template <typename T>
    float CosinusOfAngleBetweenNormalizedVectors(const TVector3D<T>& V1, const TVector3D<T>& V2)
    {
        return DotProduct(V1, V2);
    }

    template <typename T>
    float CosinusOfAngleBetweenVectors(const TVector4D<T>& V1, const TVector4D<T>& V2)
    {
        return DotProduct(V1, V2) / (V1.Length() * V2.Length());
    }
    template <typename T>
    float CosinusOfAngleBetweenNormalizedVectors(const TVector4D<T>& V1, const TVector4D<T>& V2)
    {
        return DotProduct(V1, V2);
    }

    template <typename T>
    float TripleDotProduct(const TVector3D<T>& u, const TVector3D<T>&v, const TVector3D<T>&w)
    {
        return (u.x * (v.y * w.z - v.z * w.y)) +
            (u.y * (-v.x * w.z + v.z * w.x)) +
            (u.z * (v.x * w.y - v.y * w.x));
    }

    // CrossProduct - iloczyn wektorowy - axb is defined only in 3d space
    template <typename T>
    TVector3D<T>CrossProduct(const TVector3D<T>& v1, const TVector3D<T>& v2)
    {
        return {
                v1.y * v2.z - v1.z * v2.y,
                -v1.x * v2.z + v1.z * v2.x,
                v1.x * v2.y - v1.y * v2.x
        };
    }

    template <typename T>
    std::ostream& operator <<(std::ostream& Stream, const TVector2D<T>& v)
    {
        Stream << "[" << v.x << ", " << v.y << "]";
        return Stream;
    }

    template <typename T>
    std::ostream& operator <<(std::ostream& Stream, const TVector3D<T>& v)
    {
        Stream << "[" << v.x << ", " << v.y << ", " << v.z << "]";
        return Stream;
    }

    template <typename T>
    std::ostream& operator <<(std::ostream& Stream, const TVector4D<T>& v)
    {
        Stream << "[" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "]";
        return Stream;
    }

    DECLSPEC TVector2D<float> Normalize(const TVector2D<float>& v);
    DECLSPEC TVector3D<float> Normalize(const TVector3D<float>& v);
    DECLSPEC TVector4D<float> Normalize(const TVector4D<float>& v);

    template <typename T>
    T ProjectVectorOntoDestination(T SourceVector, T DestinationVector)
    {
        return (DotProduct(DestinationVector, SourceVector) * DestinationVector) / DotPorduct(DestinationVector);
    }
    template <typename T>
    T ProjectVectorOntoNormalizedDestination(T SourceVector, T DestinationVector)
    {
        return DotProduct(DestinationVector, SourceVector) * DestinationVector;
    }

    // Points of the triangle must be sorted counter-clockwise to this function work properly
    DECLSPEC bool PointInsideTriangle(const TVector3D<float>& Point, const TVector3D<float>& TriA, const TVector3D<float>& TriB, const TVector3D<float>& TriC);
}
