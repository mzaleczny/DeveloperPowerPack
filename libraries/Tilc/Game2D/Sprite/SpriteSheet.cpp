#include "Tilc/Game2D/Sprite/DirectionalAnimation.h"
#include "Tilc/Game2D/Sprite/SpriteSheet.h"
#include "Tilc/Resources/TextureManager.h"
#include "Tilc/Utils/FileUtils.h"
#include "Tilc/Game.h"
#include <filesystem>
#include <iostream>

Tilc::Game2D::Sprite::TSpriteSheet::TSpriteSheet()
	: m_SpriteScale{ 1.0f, 1.0f }, m_Direction(EDirection::Right), m_AnimationCurrent(nullptr)
{
	if (Tilc::GameObject)
	{
		m_TextureManager = Tilc::GameObject->GetContext()->m_TextureManager;
	}
}

bool Tilc::Game2D::Sprite::TSpriteSheet::LoadSheet(const std::string& FileName)
{
	Tilc::TFile File((std::filesystem::current_path().string() + "/" + FileName).c_str());
	if (File.m_File.is_open())
	{
		ReleaseSheet(); // Release current sheet resources.
		std::string Line;
		int LineNumber = 0;
		while (std::getline(File.m_File, Line))
		{
			++LineNumber;
			if (Line.length() > 0 && Line[0] == '#')
			{
				continue;
			}
			if (Line.length() > 1 && Line[0] == '/' && Line[1] == '/')
			{
				continue;
			}
			std::stringstream KeyStream(Line);
			std::string Type;

			KeyStream >> Type;
			if (Type == "Texture")
			{
				if (m_TextureId != "")
				{
					std::cerr << "ERROR! Duplicate texture entries in: " << FileName << ", Line: " << LineNumber << std::endl;
					continue;
				}
				std::string TextureId;
				KeyStream >> TextureId;
				if (!m_TextureManager->RequireResource(TextureId))
				{
					std::cerr << "Error! Could not set up the texture: " << TextureId << " in: " << FileName << ", Line: " << LineNumber << std::endl;
					continue;
				}
				m_TextureId = TextureId;
				SpriteSheetSurface = (SDL_Surface*)m_TextureManager->GetResource(m_TextureId)->m_Resource;
			}

			else if (Type == "Size")
			{
				KeyStream >> m_SpriteDestRect.w >> m_SpriteDestRect.h;
			}
			else if (Type == "Scale")
			{
				KeyStream >> m_SpriteScale.x >> m_SpriteScale.y;
			}
			else if (Type == "AnimationType")
			{
				KeyStream >> m_AnimType;
			}
			else if (Type == "Animation")
			{
				std::string Name;
				KeyStream >> Name;
				if (m_Animations.find(Name) != m_Animations.end())
				{
					std::cerr << "ERROR! Duplicate animation(" << Name << ") in: " << FileName << ", Line : " << LineNumber << std::endl;
					continue;
				}
				TBaseAnimation* Animation = nullptr;
				if (m_AnimType == "Directional")
				{
					Animation = new TDirectionalAnimation();
				}
				else
				{
					std::cerr << "ERROR! Unknown animation type: " << m_AnimType << " in: " << FileName << ", Line : " << LineNumber << std::endl;
					continue;
				}
				KeyStream >> *Animation;
				Animation->SetSpriteSheet(this);
				Animation->SetName(Name);
				Animation->Reset();
				m_Animations.emplace(Name, Animation);
				if (!m_AnimationCurrent)
				{
					m_AnimationCurrent = Animation;
					m_AnimationCurrent->Play();
				}
			}
		}
		return true;
	}
	std::cout << "Spritesheet file " << FileName << " not exists. Skipping..." << std::endl;
	return false;
}

void Tilc::Game2D::Sprite::TSpriteSheet::ReleaseSheet()
{
	m_TextureManager->ReleaseResource(m_TextureId);
	m_AnimationCurrent = nullptr;
	while (m_Animations.begin() != m_Animations.end())
	{
		delete m_Animations.begin()->second;
		m_Animations.erase(m_Animations.begin());
	}
}

void Tilc::Game2D::Sprite::TSpriteSheet::SetDirection(EDirection Direction)
{
	if (Direction == m_Direction)
	{
		return;
	}
	m_Direction = Direction;
	m_AnimationCurrent->CropSprite();
}

bool Tilc::Game2D::Sprite::TSpriteSheet::SetAnimation(const std::string& Name, const bool& Play, const bool& Loop)
{
	auto Itr = m_Animations.find(Name);
	if (Itr == m_Animations.end())
	{
		return false;
	}

	if (Itr->second == m_AnimationCurrent)
	{
		return false;
	}

	if (m_AnimationCurrent)
	{
		m_AnimationCurrent->Stop();
	}
	m_AnimationCurrent = Itr->second;
	m_AnimationCurrent->SetLooping(Loop);
	if (Play)
	{
		m_AnimationCurrent->Play();
	}
	m_AnimationCurrent->CropSprite();

	return true;
}

void Tilc::Game2D::Sprite::TSpriteSheet::Update(const float& DeltaTime)
{
	m_AnimationCurrent->Update(DeltaTime);
}
