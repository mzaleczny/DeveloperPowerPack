#pragma once

#include "Tilc/DllGlobals.h"
#include <Tilc/Utils/ExtString.h>
#include <vector>
#include <Tilc/Gui/GuiControl.h>

struct SDL_Renderer;

namespace Tilc::Gui
{
	class DECLSPEC TImageListView : public TGuiControl
	{
	public:
		TImageListView(TGuiControl* parent, Tilc::TExtString name, const SDL_FRect& position, int ImageWidth, int ImageHeight);
		~TImageListView();
		virtual void Draw() override;
		void AddImage(const std::string_view ImageBlob);
		void RemoveImage(const std::string_view ImageId);
		void RemoveAllImages();
		void UpdateHeight();
		void SetImagesSize(int ImageWidth, int ImageHeight)
		{
			m_ImageWidth = ImageWidth;
			m_ImageHeight = ImageHeight;
			UpdateHeight();
		}
	protected:
		std::vector<Tilc::TExtString> m_Images;
		int m_ImageWidth{};
		int m_ImageHeight{};
		int m_ImagesPerRow{};
		int m_ImagesPerColumn{};
	};
}
