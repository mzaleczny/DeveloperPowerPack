#pragma once
#include <string>
#include "SDL3/SDL.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "Tilc/DllGlobals.h"
#include "Tilc/Globals.h"

namespace Tilc {

    class TBaseState;

	class DECLSPEC TLevel
	{
	public:
		TLevel(TBaseState* ParentState);
		virtual ~TLevel();

		virtual int OnKeyDown(SDL_Event& Event) { return 1; }
		virtual int OnKeyUp(SDL_Event& Event) { return 1; }
		virtual int OnMouseMotion(SDL_Event& Event) { return 1; }
		virtual int OnMouseButtonDown(SDL_Event& Event) { return 1; }
		virtual int OnMouseButtonUp(SDL_Event& Event) { return 1; }
        virtual void Update(float DeltaTime) {};
        virtual void Render() {};

	protected:
		TBaseState* m_ParentState = nullptr;
	};

}
