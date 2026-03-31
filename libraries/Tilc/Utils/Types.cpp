#include "Tilc/Utils/Types.h"
#include <string>
#include <math.h>

Tilc::float16 Tilc::f32tof16(float f32)
{
    if (f32 == 0.0f)
    {
        return 0u;
    }

    uint32_t BitsF32;
    memcpy(&BitsF32, &f32, sizeof(BitsF32));

    uint16_t BitsF16 = (BitsF32 >> (23 - 10)) & 0x3ff;
    uint16_t Exp = (BitsF32 >> 23) & 0xff;
    Exp -= (127u - 15u);
    Exp &= 0x1f;
    BitsF16 |= (Exp << 10);
    BitsF16 |= ((BitsF32 >> 31) << 15);

    return BitsF16;
}

Tilc::TVertex16 Tilc::f32tof16Vertex(float* ArrayF32)
{
    Tilc::TVertex16 Result;
    
    Result.m_Pos[0] = f32tof16(ArrayF32[0]);
    Result.m_Pos[1] = f32tof16(ArrayF32[1]);
    Result.m_Pos[2] = f32tof16(ArrayF32[2]);

    Result.m_Color[0] = f32tof16(ArrayF32[3]);
    Result.m_Color[1] = f32tof16(ArrayF32[4]);
    Result.m_Color[2] = f32tof16(ArrayF32[5]);

    return Result;
}

float Tilc::f16tof32(float16 f16)
{
    float f32 = 0.0f;

    unsigned int Sign = (f16 & 0x8000) >> 15;
    unsigned int Exponent = ((f16 & 0x7c00) >> 10);
    float Fraction = (f16 & 0x03ff) / 1024.0f;

    if (Exponent == 0x1f)
    {
        if (Fraction == 0)
        {
            int PositiveInf = 0x7f800000;
            int NegativeInf = 0xff800000;
            return (Sign == 0) ? *(float*)&PositiveInf : *(float*)&NegativeInf;
        }
        else
        {
            return NAN; // generate NaN
        }
    }

    if (Exponent == 0)
    {
        if (Fraction == 0)
        {
            if (Sign == 1)
                return -0.0f;
            return 0.0f;
        }
        else
        {
            f32 = Fraction * pow(2.0, -14.0);
            if (Sign == 1)
                f32 *= -1.0f;
            return f32;
        }
    }

    f32 = (Fraction + 1.0f) * pow(2.0, ((int)Exponent - 15));
    if (Sign == 1)
    {
        f32 *= -1.0f;
    }
    return f32;
}
