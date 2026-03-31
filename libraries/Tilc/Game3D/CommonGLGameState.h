#pragma once
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include <vector>
#include <SDL3/SDL.h>
#include "Tilc/StateManager.h"
#include "Tilc/Graphics/OpenGL/Shader.h"

namespace Tilc
{

	struct TEventDetails;

	class DECLSPEC TCommonGLGameState : public TBaseState
	{
	public:
        TCommonGLGameState(Tilc::TStateManager* StateManager);
        virtual ~TCommonGLGameState();

        Tilc::Graphics::OpenGL::TPipeline* m_Pipeline = nullptr;
	};

}// namespace Tilc
