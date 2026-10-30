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
		// if image name is "" then resource is created and added otherwise returned is image of the name if exists, if not exists then resource is created and returned
		void AddImage(const std::string_view ImageBlob, const std::string_view ImageName = "");
		void RemoveImage(const std::string_view ImageId);
		void RemoveAllImages();
		std::vector<Tilc::TExtString>& GetImages()
		{
			return m_Images;
		}
		Tilc::TExtString GetAllImagesCommaSeparated()
		{
			return Tilc::Implode(',', m_Images);
		}
		void UpdateHeight();
		void SetImagesSize(int ImageWidth, int ImageHeight)
		{
			m_ImageWidth = ImageWidth;
			m_ImageHeight = ImageHeight;
			UpdateHeight();
		}
		inline void SetAllowRemoveImage(bool Value)
		{
			m_AllowRemoveImage = Value;
		}

		std::function<int(TImageListView* Self, int ImageIndex, Tilc::TExtString& ImageName)> OnRemoveImage;
		virtual bool OnMouseButtonUp(const SDL_Event& event) override;

	protected:
		std::vector<Tilc::TExtString> m_Images;
		int m_ImageWidth{};
		int m_ImageHeight{};
		int m_ImagesPerRow{};
		int m_ImagesPerColumn{};
		bool m_AllowRemoveImage{};
	};
}
