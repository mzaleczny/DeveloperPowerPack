#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <initializer_list>
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/Vector.h"

namespace Tilc
{
    template <typename T>
    class TMatrix3
    {
    public:
        union
        {
            T Elems[9];
            struct
            {
                // first row
                T e11;
                T e12;
                T e13;

                // second row
                T e21;
                T e22;
                T e23;

                // third row
                T e31;
                T e32;
                T e33;
            };
        };

        TMatrix3(bool UnitMatrix = false)
        {
            std::memset(&Elems, 0, sizeof(T) * 9);
            if (UnitMatrix)
            {
                e11 = 1;
                e22 = 1;
                e33 = 1;
            }
        };

        TMatrix3(T DiagonalElem)
        {
            std::memset(&Elems, 0, sizeof(T) * 9);
            e11 = DiagonalElem;
            e22 = DiagonalElem;
            e33 = DiagonalElem;
        };

        TMatrix3(std::initializer_list<T> List)
        {
            const T* it{ List.begin() };
            int i = 0;
            for (int i = 0; i < List.size() && i < 9; ++i, ++it)
            {
                Elems[i] = *it;
            }
        }

        TMatrix3<T>& operator=(const TMatrix3<T>& Mat)
        {
            std::memcpy(&Elems, &Mat.Elems, sizeof(T) * 9);
            return *this;
        }

        bool operator==(const TMatrix3<T> Mat)
        {
            return std::memcmp(&Elems, &Mat.Elems, sizeof(T) * 9) == 0;
        }

        TMatrix3<T> operator+(const TMatrix3<T>& Mat)
        {
            return {
                e11 + Mat.e11, e12 + Mat.e12, e13 + Mat.e13,
                e21 + Mat.e21, e22 + Mat.e22, e23 + Mat.e23,
                e31 + Mat.e31, e32 + Mat.e32, e33 + Mat.e33
            };
        }

        TMatrix3<T> operator*(const TMatrix3<T>& Mat)
        {
            return {
                e11 * Mat.e11 + e12 * Mat.e21 + e13 * Mat.e31,
                e11 * Mat.e12 + e12 * Mat.e22 + e13 * Mat.e32,
                e11 * Mat.e13 + e12 * Mat.e23 + e13 * Mat.e33,

                e21 * Mat.e11 + e22 * Mat.e21 + e23 * Mat.e31,
                e21 * Mat.e12 + e22 * Mat.e22 + e23 * Mat.e32,
                e21 * Mat.e13 + e22 * Mat.e23 + e23 * Mat.e33,

                e31 * Mat.e11 + e32 * Mat.e21 + e33 * Mat.e31,
                e31 * Mat.e12 + e32 * Mat.e22 + e33 * Mat.e32,
                e31 * Mat.e13 + e32 * Mat.e23 + e33 * Mat.e33
            };
        }

        TMatrix3<T> operator*=(const TMatrix3<T>& Mat)
        {
            e11 = e11 * Mat.e11 + e12 * Mat.e21 + e13 * Mat.e31;
            e12 = e11 * Mat.e12 + e12 * Mat.e22 + e13 * Mat.e32;
            e13 = e11 * Mat.e13 + e12 * Mat.e23 + e13 * Mat.e33;

            e21 = e21 * Mat.e11 + e22 * Mat.e21 + e23 * Mat.e31;
            e22 = e21 * Mat.e12 + e22 * Mat.e22 + e23 * Mat.e32;
            e23 = e21 * Mat.e13 + e22 * Mat.e23 + e23 * Mat.e33;

            e31 = e31 * Mat.e11 + e32 * Mat.e21 + e33 * Mat.e31;
            e32 = e31 * Mat.e12 + e32 * Mat.e22 + e33 * Mat.e32;
            e33 = e31 * Mat.e13 + e32 * Mat.e23 + e33 * Mat.e33;
        }

        // returns idx row
        T* operator[](int idx)
        {
            return &Elems[idx * 3];
        }

        T Det()
        {
            return Elems[0] * Elems[4] * Elems[8] +
                Elems[1] * Elems[5] * Elems[6] +
                Elems[2] * Elems[3] * Elems[7] -
                Elems[2] * Elems[4] * Elems[6] -
                Elems[1] * Elems[3] * Elems[8] -
                Elems[0] * Elems[5] * Elems[7];
        }
    };

    template <typename T>
    class TMatrix4
    {
    public:
        union
        {
            T Elems[16];
            struct
            {
                // first row
                T e11;
                T e12;
                T e13;
                T e14;

                // second row
                T e21;
                T e22;
                T e23;
                T e24;

                // third row
                T e31;
                T e32;
                T e33;
                T e34;

                // fourth row
                T e41;
                T e42;
                T e43;
                T e44;
            };
        };

        TMatrix4(bool UnitMatrix = false)
        {
            std::memset(&Elems, 0, sizeof(T) * 16);
            if (UnitMatrix)
            {
                e11 = 1;
                e22 = 1;
                e33 = 1;
                e44 = 1;
            }
        };

        TMatrix4(T DiagonalElem)
        {
            std::memset(&Elems, 0, sizeof(T) * 16);
            e11 = DiagonalElem;
            e22 = DiagonalElem;
            e33 = DiagonalElem;
            e44 = DiagonalElem;
        };

        TMatrix4(std::initializer_list<T> List)
        {
            const T* it { List.begin() };
            int i = 0;
            for (int i = 0; i < List.size() && i < 16; ++i, ++it)
            {
                Elems[i] = *it;
            }
        }

        TMatrix4<T>& operator=(const TMatrix4<T>& Mat)
        {
            std::memcpy(&Elems, &Mat.Elems, sizeof(T) * 16);
            return *this;
        }

        bool operator==(const TMatrix4<T> Mat)
        {
            return std::memcmp(&Elems, &Mat.Elems, sizeof(T) * 16) == 0;
        }

        TMatrix4<T> operator+(const TMatrix4<T>& Mat)
        {
            return {
                e11 + Mat.e11, e12 + Mat.e12, e13 + Mat.e13, e14 + Mat.e14,
                e21 + Mat.e21, e22 + Mat.e22, e23 + Mat.e23, e24 + Mat.e24,
                e31 + Mat.e31, e32 + Mat.e32, e33 + Mat.e33, e34 + Mat.e34,
                e41 + Mat.e41, e42 + Mat.e42, e43 + Mat.e43, e44 + Mat.e44,
            };
        }

        TMatrix4<T> operator*(const TMatrix4<T>& Mat)
        {
            return {
                e11 * Mat.e11 + e12 * Mat.e21 + e13 * Mat.e31 + e14 * Mat.e41,
                e11 * Mat.e12 + e12 * Mat.e22 + e13 * Mat.e32 + e14 * Mat.e42,
                e11 * Mat.e13 + e12 * Mat.e23 + e13 * Mat.e33 + e14 * Mat.e43,
                e11 * Mat.e14 + e12 * Mat.e24 + e13 * Mat.e34 + e14 * Mat.e44,

                e21 * Mat.e11 + e22 * Mat.e21 + e23 * Mat.e31 + e24 * Mat.e41,
                e21 * Mat.e12 + e22 * Mat.e22 + e23 * Mat.e32 + e24 * Mat.e42,
                e21 * Mat.e13 + e22 * Mat.e23 + e23 * Mat.e33 + e24 * Mat.e43,
                e21 * Mat.e14 + e22 * Mat.e24 + e23 * Mat.e34 + e24 * Mat.e44,

                e31 * Mat.e11 + e32 * Mat.e21 + e33 * Mat.e31 + e34 * Mat.e41,
                e31 * Mat.e12 + e32 * Mat.e22 + e33 * Mat.e32 + e34 * Mat.e42,
                e31 * Mat.e13 + e32 * Mat.e23 + e33 * Mat.e33 + e34 * Mat.e43,
                e31 * Mat.e14 + e32 * Mat.e24 + e33 * Mat.e34 + e34 * Mat.e44,

                e41 * Mat.e11 + e42 * Mat.e21 + e43 * Mat.e31 + e44 * Mat.e41,
                e41 * Mat.e12 + e42 * Mat.e22 + e43 * Mat.e32 + e44 * Mat.e42,
                e41 * Mat.e13 + e42 * Mat.e23 + e43 * Mat.e33 + e44 * Mat.e43,
                e41 * Mat.e14 + e42 * Mat.e24 + e43 * Mat.e34 + e44 * Mat.e44
            };
        }

        TMatrix4<T>& operator*=(const TMatrix4<T>& Mat)
        {
            e11 = e11 * Mat.e11 + e12 * Mat.e21 + e13 * Mat.e31 + e14 * Mat.e41;
            e12 = e11 * Mat.e12 + e12 * Mat.e22 + e13 * Mat.e32 + e14 * Mat.e42;
            e13 = e11 * Mat.e13 + e12 * Mat.e23 + e13 * Mat.e33 + e14 * Mat.e43;
            e14 = e11 * Mat.e14 + e12 * Mat.e24 + e13 * Mat.e34 + e14 * Mat.e44;

            e21 = e21 * Mat.e11 + e22 * Mat.e21 + e23 * Mat.e31 + e24 * Mat.e41;
            e22 = e21 * Mat.e12 + e22 * Mat.e22 + e23 * Mat.e32 + e24 * Mat.e42;
            e23 = e21 * Mat.e13 + e22 * Mat.e23 + e23 * Mat.e33 + e24 * Mat.e43;
            e24 = e21 * Mat.e14 + e22 * Mat.e24 + e23 * Mat.e34 + e24 * Mat.e44;

            e31 = e31 * Mat.e11 + e32 * Mat.e21 + e33 * Mat.e31 + e34 * Mat.e41;
            e32 = e31 * Mat.e12 + e32 * Mat.e22 + e33 * Mat.e32 + e34 * Mat.e42;
            e33 = e31 * Mat.e13 + e32 * Mat.e23 + e33 * Mat.e33 + e34 * Mat.e43;
            e34 = e31 * Mat.e14 + e32 * Mat.e24 + e33 * Mat.e34 + e34 * Mat.e44;

            e41 = e41 * Mat.e11 + e42 * Mat.e21 + e43 * Mat.e31 + e44 * Mat.e41;
            e42 = e41 * Mat.e12 + e42 * Mat.e22 + e43 * Mat.e32 + e44 * Mat.e42;
            e43 = e41 * Mat.e13 + e42 * Mat.e23 + e43 * Mat.e33 + e44 * Mat.e43;
            e44 = e41 * Mat.e14 + e42 * Mat.e24 + e43 * Mat.e34 + e44 * Mat.e44;
            return *this;
        }

        // returns idx row
        T* operator[](int idx)
        {
            return &Elems[idx * 4];
        }

        TMatrix3<T> ToMat3()
        {
            return {
                this->a11, this->a12, this->a13,
                this->a21, this->a22, this->a23,
                this->a31, this->a32, this->a33
            };
        }

        T Det()
        {
            return Elems[0] * Elems[4] * Elems[8] +
                Elems[1] * Elems[5] * Elems[6] +
                Elems[2] * Elems[3] * Elems[7] -
                Elems[2] * Elems[4] * Elems[6] -
                Elems[1] * Elems[3] * Elems[8] -
                Elems[0] * Elems[5] * Elems[7];
        }

        TVector4D<T> GetCol(int i) const
        {
            return TVector4D<T>(Elems[i], Elems[i+4], Elems[i+8], Elems[i+12]);
        }

        void SetCol(int i, const TVector4D<T>& v)
        {
            Elems[i] = v.x;
            Elems[i + 4] = v.y;
            Elems[i + 8] = v.z;
            Elems[i + 12] = v.w;
        }

        TVector4D<T> GetRow(int i) const
        {
            return TVector4D<T>(Elems[i*4], Elems[i*4+1], Elems[i*4+2], Elems[i*4+3]);
        }

        void SetRow(int i, const TVector4D<T>& v)
        {
            Elems[i*4] = v.x;
            Elems[i*4 + 1] = v.y;
            Elems[i*4 + 2] = v.z;
            Elems[i*4 + 3] = v.w;
        }
    };

    template <typename T>
    std::ostream& operator <<(std::ostream& Stream, TMatrix3<T> v)
    {
        Stream  << std::setw(10) << std::setprecision(4) << v.Elems[0]
                << std::setw(10) << std::setprecision(4) << v.Elems[1]
                << std::setw(10) << std::setprecision(4) << v.Elems[2] << std::endl;

        Stream  << std::setw(10) << std::setprecision(4) << v.Elems[3]
                << std::setw(10) << std::setprecision(4) << v.Elems[4]
                << std::setw(10) << std::setprecision(4) << v.Elems[5] << std::endl;

        Stream  << std::setw(10) << std::setprecision(4) << v.Elems[6]
                << std::setw(10) << std::setprecision(4) << v.Elems[7]
                << std::setw(10) << std::setprecision(4) << v.Elems[8] << std::endl;

        return Stream;
    }

    template <typename T>
    std::ostream& operator <<(std::ostream& Stream, TMatrix4<T> v)
    {
        Stream  << std::setw(10) << std::setprecision(4) << v.Elems[0]
                << std::setw(10) << std::setprecision(4) << v.Elems[1]
                << std::setw(10) << std::setprecision(4) << v.Elems[2]
                << std::setw(10) << std::setprecision(4) << v.Elems[3] << std::endl;

        Stream  << std::setw(10) << std::setprecision(4) << v.Elems[4]
                << std::setw(10) << std::setprecision(4) << v.Elems[5]
                << std::setw(10) << std::setprecision(4) << v.Elems[6]
                << std::setw(10) << std::setprecision(4) << v.Elems[7] << std::endl;

        Stream  << std::setw(10) << std::setprecision(4) << v.Elems[8]
                << std::setw(10) << std::setprecision(4) << v.Elems[9]
                << std::setw(10) << std::setprecision(4) << v.Elems[10]
                << std::setw(10) << std::setprecision(4) << v.Elems[11] << std::endl;

        Stream  << std::setw(10) << std::setprecision(4) << v.Elems[12]
                << std::setw(10) << std::setprecision(4) << v.Elems[13]
                << std::setw(10) << std::setprecision(4) << v.Elems[14]
                << std::setw(10) << std::setprecision(4) << v.Elems[15] << std::endl;

        return Stream;
    }

    // Translation
    DECLSPEC TMatrix4<float> Translate(const TMatrix4<float>& Source, const TVector3D<float>& v);
    // Rotation Angle is in Radians
    DECLSPEC TMatrix4<float> RotateAroundXAxis(const TMatrix4<float>& Source, float Angle);
    DECLSPEC TMatrix4<float> RotateAroundYAxis(const TMatrix4<float>& Source, float Angle);
    DECLSPEC TMatrix4<float> RotateAroundZAxis(const TMatrix4<float>& Source, float Angle);
    DECLSPEC TMatrix4<float> RotateAroundVector(const TMatrix4<float>& Source, float Angle, const TVector3D<float>& v);
    // Scale
    DECLSPEC TMatrix4<float> Scale(const TMatrix4<float>& Source, const TVector3D<float>& v);

    DECLSPEC TMatrix4<float> LookAtRH(const TVector3D<float>& Eye, const TVector3D<float>& Center, const TVector3D<float>& Up);
    DECLSPEC TMatrix4<float> LookAtLH(const TVector3D<float>& Eye, const TVector3D<float>& Center, const TVector3D<float>& Up);
    DECLSPEC TMatrix4<float> LookAt(const TVector3D<float>& Eye, const TVector3D<float>& Center, const TVector3D<float>& Up);

    DECLSPEC TMatrix4<float> Ortho(unsigned char force_depth_zero_to_one, float left, float right, float bottom, float top, float near, float far);
    DECLSPEC TMatrix4<float> Frustum(float left, float right, float bottom, float top, float near, float far);
    // fovy - in degrees
    DECLSPEC TMatrix4<float> Perspective(float fovy, float aspect, float near, float far);

}
