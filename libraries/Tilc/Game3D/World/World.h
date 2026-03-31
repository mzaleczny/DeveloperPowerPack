#pragma once

#include <string>
#include <vector>
#include "SDL3/SDL.h"
#include "Tilc/DllGlobals.h"
#include "Tilc/Game2D/World/MapLayer.h"
#include "Tilc/Utils/Vector.h"
#include "Tilc/Graphics/Light.h"
#include <unordered_map>
#include <vector>

namespace Tilc
{
	class TWindow;
	class TBaseState;
	class TSharedContext;

	namespace Graphics
	{
		class TCamera;
	}
}

namespace Tilc::Game3D
{
	class DECLSPEC TWorld3D
	{
	public:
		TWorld3D();
		~TWorld3D();

		std::vector<Tilc::Graphics::TCamera*> m_Cameras;
		Tilc::Graphics::TLights m_Lights;

	protected:
		Tilc::TWindow* m_Window = nullptr;
		Tilc::TBaseState* m_CurrentState;
	};
}
