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
		bool LoadSheet(const std::string& FileName, bool UseTexture = true);
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
		const SDL_FRect& GetSpriteSrcFRect() const
		{
			return m_SpriteSrcFRect;
		}
		void SetSpriteSrcFRect(const SDL_FRect& Rect)
		{
			m_SpriteSrcFRect = Rect;
		}
		const SDL_FRect& GetSpriteDestFRect() const
		{
			return m_SpriteDestFRect;
		}
		void SetSpriteDestFRect(const SDL_FRect& Rect)
		{
			m_SpriteDestFRect = Rect;
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
		}
		inline void Render(SDL_Renderer* Renderer)
		{
			SDL_RenderTexture(Renderer, SpriteSheetTexture, &m_SpriteSrcFRect, &m_SpriteDestFRect);
		}
	private:
		std::string m_TextureId;
		SDL_Rect m_SpriteSrcRect;
		SDL_FRect m_SpriteSrcFRect;
		SDL_Rect m_SpriteDestRect;
		SDL_FRect m_SpriteDestFRect;
		SDL_FPoint m_SpriteScale;
		// Texture below is handled by resource manager so it is not deleted in this class
		SDL_Surface* SpriteSheetSurface{};
		SDL_Texture* SpriteSheetTexture{};
		EDirection m_Direction;
		std::string m_AnimType;
		TAnimations m_Animations;
		TBaseAnimation* m_AnimationCurrent = nullptr;
		Tilc::Resources::TTextureManager* m_TextureManager = nullptr;
	};
}
