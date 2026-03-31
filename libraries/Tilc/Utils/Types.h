#pragma once

#include <stdint.h>
#include "Tilc/DllGlobals.h"

namespace Tilc
{
	using float16 = uint16_t;
	struct DECLSPEC TVertex16
	{
		float16 m_Pos[3];
		float16 m_Color[3];
		//uint16_t m_Padding;
	};

	DECLSPEC float16 f32tof16(float f32);
	DECLSPEC TVertex16 f32tof16Vertex(float* ArrayF32);
    DECLSPEC float f16tof32(float16 f16);
}
