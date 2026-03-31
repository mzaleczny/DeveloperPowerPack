#include "Tilc/Utils/Matrix.h"
#define _USE_MATH_DEFINES
#include <math.h>

Tilc::TMatrix4<float> Tilc::Translate(const Tilc::TMatrix4<float>& Source, const Tilc::TVector3D<float>& v)
{
    Tilc::TMatrix4<float> Result(Source);
    Result.SetCol(3, Source.GetCol(0) * v.x + Source.GetCol(1) * v.y + Source.GetCol(2) * v.z + Source.GetCol(3));
    return Result;
}

Tilc::TMatrix4<float> Tilc::RotateAroundXAxis(const Tilc::TMatrix4<float>& Source, float Angle)
{
    float c = std::cos(Angle);
    float s = std::sin(Angle);

    Tilc::TMatrix4<float> Rotate = {
        1.0f,   0.0f,   0.0f,   0.0f,
        0.0f,   c,      -s,     0.0f,
        0.0f,   s,      c,      0.0f,
        0.0f,   0.0f,   0.0f,   1.0f
    };

    Tilc::TMatrix4<float> Result;
    Result.SetCol(0, Source.GetCol(0) * Rotate.Elems[0] + Source.GetCol(1) * Rotate.Elems[4] + Source.GetCol(2) * Rotate.Elems[8]);
    Result.SetCol(1, Source.GetCol(0) * Rotate.Elems[1] + Source.GetCol(1) * Rotate.Elems[5] + Source.GetCol(2) * Rotate.Elems[9]);
    Result.SetCol(2, Source.GetCol(0) * Rotate.Elems[2] + Source.GetCol(1) * Rotate.Elems[6] + Source.GetCol(2) * Rotate.Elems[10]);
    Result.SetCol(3, Source.GetCol(3));
    return Result;
}

Tilc::TMatrix4<float> Tilc::RotateAroundYAxis(const Tilc::TMatrix4<float>& Source, float Angle)
{
    float c = std::cos(Angle);
    float s = std::sin(Angle);

    Tilc::TMatrix4<float> Rotate = {
        c,      0.0f,   s,      0.0f,
        0.0f,   1.0f,   0.0f,   0.0f,
        -s,     0.0f,   c,      0.0f,
        0.0f,   0.0f,   0.0f,   1.0f
    };

    Tilc::TMatrix4<float> Result;
    Result.SetCol(0, Source.GetCol(0) * Rotate.Elems[0] + Source.GetCol(1) * Rotate.Elems[4] + Source.GetCol(2) * Rotate.Elems[8]);
    Result.SetCol(1, Source.GetCol(0) * Rotate.Elems[1] + Source.GetCol(1) * Rotate.Elems[5] + Source.GetCol(2) * Rotate.Elems[9]);
    Result.SetCol(2, Source.GetCol(0) * Rotate.Elems[2] + Source.GetCol(1) * Rotate.Elems[6] + Source.GetCol(2) * Rotate.Elems[10]);
    Result.SetCol(3, Source.GetCol(3));
    return Result;
}

Tilc::TMatrix4<float> Tilc::RotateAroundZAxis(const Tilc::TMatrix4<float>& Source, float Angle)
{
    float c = std::cos(Angle);
    float s = std::sin(Angle);

    Tilc::TMatrix4<float> Rotate = {
        c,      -s,     0.0f,   0.0f,
        s,      c,      0.0f,   0.0f,
        0.0f,   0.0f,   1.0f,   0.0f,
        0.0f,   0.0f,   0.0f,   1.0f
    };

    Tilc::TMatrix4<float> Result;
    Result.SetCol(0, Source.GetCol(0) * Rotate.Elems[0] + Source.GetCol(1) * Rotate.Elems[4] + Source.GetCol(2) * Rotate.Elems[8]);
    Result.SetCol(1, Source.GetCol(0) * Rotate.Elems[1] + Source.GetCol(1) * Rotate.Elems[5] + Source.GetCol(2) * Rotate.Elems[9]);
    Result.SetCol(2, Source.GetCol(0) * Rotate.Elems[2] + Source.GetCol(1) * Rotate.Elems[6] + Source.GetCol(2) * Rotate.Elems[10]);
    Result.SetCol(3, Source.GetCol(3));
    return Result;
}

Tilc::TMatrix4<float> Tilc::RotateAroundVector(const Tilc::TMatrix4<float>& Source, float Angle, const Tilc::TVector3D<float>& v)
{
    float c = cos(Angle);
    float s = sin(Angle);
    Tilc::TVector3D<float> axis{ v };
    axis.Normalize();
    Tilc::TVector3D<float> temp{ axis * (1 - c) };

    Tilc::TMatrix4<float> Rotate = {
        c + temp.x * axis.x,
        0.0f + temp.y * axis.x - s * axis.z,
        0.0f + temp.z * axis.x + s * axis.y,
        0.0f,

        0.0f + temp.x * axis.y + s * axis.z,
        c + temp.y * axis.y,
        0.0f + temp.z * axis.y - s * axis.x,
        0.0f,

        0.0f + temp.x * axis.z - s * axis.y,
        0.0f + temp.y * axis.z + s * axis.x,
        c + temp.z * axis.z,
        0.0f,

        0.0f,
        0.0f,
        0.0f,
        1.0f
    };

    Tilc::TMatrix4<float> Result;
    Result.SetCol(0, Source.GetCol(0) * Rotate.Elems[0] + Source.GetCol(1) * Rotate.Elems[4] + Source.GetCol(2) * Rotate.Elems[8]);
    Result.SetCol(1, Source.GetCol(0) * Rotate.Elems[1] + Source.GetCol(1) * Rotate.Elems[5] + Source.GetCol(2) * Rotate.Elems[9]);
    Result.SetCol(2, Source.GetCol(0) * Rotate.Elems[2] + Source.GetCol(1) * Rotate.Elems[6] + Source.GetCol(2) * Rotate.Elems[10]);
    Result.SetCol(3, Source.GetCol(3));
    return Result;
}

Tilc::TMatrix4<float> Tilc::Scale(const TMatrix4<float>& Source, const TVector3D<float>& v)
{
    Tilc::TMatrix4<float> Result(Source);
    Result.e11 = Result.e11 * v.x;
    Result.e22 = Result.e22 * v.y;
    Result.e33 = Result.e33 * v.z;
    return Result;
}

Tilc::TMatrix4<float> Tilc::LookAtRH(const Tilc::TVector3D<float>& Eye, const Tilc::TVector3D<float>& Center, const Tilc::TVector3D<float>& Up)
{
    const Tilc::TVector3D<float> f(Tilc::Normalize(Center - Eye));
    const Tilc::TVector3D<float> s(Tilc::Normalize(Tilc::CrossProduct(f, Up)));
    const Tilc::TVector3D<float> u(Tilc::CrossProduct(s, f));

    Tilc::TMatrix4<float> Result(1.0f);
    Result.Elems[0] = s.x;
    Result.Elems[1] = s.y;
    Result.Elems[2] = s.z;
    Result.Elems[4] = u.x;
    Result.Elems[5] = u.y;
    Result.Elems[6] = u.z;
    Result.Elems[8] = -f.x;
    Result.Elems[9] = -f.y;
    Result.Elems[10] = -f.z;
    Result.Elems[3] = -Tilc::DotProduct(s, Eye);
    Result.Elems[7] = -Tilc::DotProduct(u, Eye);
    Result.Elems[11] = Tilc::DotProduct(f, Eye);
    return Result;
}

Tilc::TMatrix4<float> Tilc::LookAtLH(const Tilc::TVector3D<float>& Eye, const Tilc::TVector3D<float>& Center, const Tilc::TVector3D<float>& Up)
{
    const Tilc::TVector3D<float> f(Tilc::Normalize(Center - Eye));
    const Tilc::TVector3D<float> s(Tilc::Normalize(Tilc::CrossProduct(Up, f)));
    const Tilc::TVector3D<float> u(Tilc::CrossProduct(f, s));

    Tilc::TMatrix4<float> Result(1.0f);
    Result.Elems[0] = s.x;
    Result.Elems[1] = s.y;
    Result.Elems[2] = s.z;
    Result.Elems[4] = u.x;
    Result.Elems[5] = u.y;
    Result.Elems[6] = u.z;
    Result.Elems[8] = f.x;
    Result.Elems[9] = f.y;
    Result.Elems[10] = f.z;
    Result.Elems[3] = -Tilc::DotProduct(s, Eye);
    Result.Elems[7] = -Tilc::DotProduct(u, Eye);
    Result.Elems[11] = -Tilc::DotProduct(f, Eye);
    return Result;
}

Tilc::TMatrix4<float> Tilc::LookAt(const Tilc::TVector3D<float>& Eye, const Tilc::TVector3D<float>& Center, const Tilc::TVector3D<float>& Up)
{
    return LookAtRH(Eye, Center, Up);
}

Tilc::TMatrix4<float> Tilc::Ortho(unsigned char force_depth_zero_to_one, float left, float right, float bottom, float top, float near, float far)
{
    Tilc::TMatrix4<float> Result;
    if (!force_depth_zero_to_one)
    {
        // for opengl where depth is in range [-1,1]
        Result.Elems[0] = 2.0f / (right - left);
        Result.Elems[1] = 0.0f;
        Result.Elems[2] = 0.0f;
        Result.Elems[3] = -(right + left) / (right - left);

        Result.Elems[4] = 0.0f;
        Result.Elems[5] = 2.0f / (top - bottom);
        Result.Elems[6] = 0.0f;
        Result.Elems[7] = -(top + bottom) / (top - bottom);

        Result.Elems[8] = 0.0f;
        Result.Elems[9] = 0.0f;
        Result.Elems[10] = -2.0f / (far - near);
        Result.Elems[11] = -(far + near) / (far - near);

        Result.Elems[12] = 0.0f;
        Result.Elems[13] = 0.0f;
        Result.Elems[14] = 0.0f;
        Result.Elems[15] = 1.0f;
    }
    else
    {
        // for vulkan where depth is in range [0,1]
        Result.Elems[0] = 2.0f / (right - left);
        Result.Elems[1] = 0.0f;
        Result.Elems[2] = 0.0f;
        Result.Elems[3] = -(right + left) / (right - left);

        Result.Elems[4] = 0.0f;
        Result.Elems[5] = 2.0f / (top - bottom);
        Result.Elems[6] = 0.0f;
        Result.Elems[7] = -(top + bottom) / (top - bottom);

        Result.Elems[8] = 0.0f;
        Result.Elems[9] = 0.0f;
        Result.Elems[10] = 1.0f / (near - far);
        Result.Elems[11] = -near / (far - near);

        Result.Elems[12] = 0.0f;
        Result.Elems[13] = 0.0f;
        Result.Elems[14] = 0.0f;
        Result.Elems[15] = 1.0f;
    }

    return Result;
}

Tilc::TMatrix4<float> Tilc::Frustum(float left, float right, float bottom, float top, float near, float far)
{
    Tilc::TMatrix4<float> Result;

    Result.Elems[0] = 2.0f * near / (right - left);
    Result.Elems[1] = 0.0f;
    Result.Elems[2] = (right + left) / (right - left);
    Result.Elems[3] = 0.0f;

    Result.Elems[4] = 0.0f;
    Result.Elems[5] = 2.0f * near / (top - bottom);
    Result.Elems[6] = (top + bottom) / (top - bottom);
    Result.Elems[7] = 0.0f;

    Result.Elems[8] = 0.0f;
    Result.Elems[9] = 0.0f;
    Result.Elems[10] = -(far + near) / (far - near);
    Result.Elems[11] = -2.0f * far * near / (far - near);

    Result.Elems[12] = 0.0f;
    Result.Elems[13] = 0.0f;
    Result.Elems[14] = -1.0f;
    Result.Elems[15] = 0.0f;

    return Result;
}

Tilc::TMatrix4<float> Tilc::Perspective(float fovy, float aspect, float near, float far)
{
    Tilc::TMatrix4<float> Result;

    float radians = fovy * M_PI / 180.0f;
    float tan_half_fovy = std::tan(radians / 2.0f);

    Result.Elems[0] = 1.0f / (tan_half_fovy * aspect);
    Result.Elems[1] = 0.0f;
    Result.Elems[2] = 0.0f;
    Result.Elems[3] = 0.0f;

    Result.Elems[4] = 0.0f;
    Result.Elems[5] = 1.0f / tan_half_fovy;
    Result.Elems[6] = 0.0f;
    Result.Elems[7] = 0.0f;

    Result.Elems[8] = 0.0f;
    Result.Elems[9] = 0.0f;
    Result.Elems[10] = -(far + near) / (far - near);
    Result.Elems[11] = -2.0f * far * near / (far - near);;

    Result.Elems[12] = 0.0f;
    Result.Elems[13] = 0.0f;
    Result.Elems[14] = -1.0f;
    Result.Elems[15] = 0.0f;

    return Result;
}
