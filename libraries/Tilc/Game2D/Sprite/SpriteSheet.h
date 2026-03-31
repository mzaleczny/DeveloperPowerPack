#pragma once

#include "Tilc/DllGlobals.h"
#include "SDL3/SDL.h"
#include <string>
#include <unordered_map>
#include "Tilc/Utils/Vector.h"
#include "Tilc/Game2D/Sprite/CommonTypes.h"

namespace Tilc::Resources
{
	class TTextureManager;
}

namespace Tilc::Game2D::Sprite
{
	class TBaseAnimation;
	class TTextureManager;
	using TAnimations = std::unordered_map<std::string, TBaseAnimation*>;


	class DECLSPEC TSpriteSheet
	{
	public:
		TSpriteSheet();
		~TSpriteSheet()
		{
			ReleaseSheet();
		}
		bool LoadSheet(const std::string& FileName);
		void ReleaseSheet();
		const SDL_Rect& GetSpriteSrcRect() const
		{
			return m_SpriteSrcRect;
		}
		void SetSpriteSrcRect(const SDL_Rect& Rect)
		{
			m_SpriteSrcRect = Rect;
		}
		const SDL_Rect& GetSpriteDestRect() const
		{
			return m_SpriteDestRect;
		}
		void SetSpriteDestRect(const SDL_Rect& Rect)
		{
			m_SpriteDestRect = Rect;
		}
		void SetDirection(EDirection Direction);
		EDirection GetDirection() const
		{
			return m_Direction;
		}
		TBaseAnimation* GetCurrentAnimation()
		{
			return m_AnimationCurrent;
		}
		bool SetAnimation(const std::string& Name, const bool& Play = false, const bool& Loop = false);
		void Update(const float& DeltaTime);
		inline void Draw(SDL_Surface* Destination)
		{
			SDL_BlitSurface(SpriteSheetSurface, &m_SpriteSrcRect, Destination, &m_SpriteDestRect);
			//SDL_RenderTexture(Destination, SpriteSheetSurface, &m_SpriteSrcRect, &m_SpriteDestRect);
		}
	private:
		std::string m_TextureId;
		SDL_Rect m_SpriteSrcRect;
		SDL_Rect m_SpriteDestRect;
		SDL_FPoint m_SpriteScale;
		// Texture below is handled by resource manager so it is not deleted in this class
		SDL_Surface* SpriteSheetSurface;
		EDirection m_Direction;
		std::string m_AnimType;
		TAnimations m_Animations;
		TBaseAnimation* m_AnimationCurrent = nullptr;
		Tilc::Resources::TTextureManager* m_TextureManager = nullptr;
	};
}
