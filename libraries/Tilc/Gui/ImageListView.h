#pragma once

#include "Tilc/DllGlobals.h"
#include <Tilc/Utils/ExtString.h>
#include <vector>
#include <Tilc/Gui/GuiControl.h>
#include <SDL3/SDL.h>

struct SDL_Renderer;

namespace Tilc::Gui
{
	class DECLSPEC TImageListView : public TGuiControl
	{
	public:
		TImageListView(TGuiControl* parent, Tilc::TExtString name, const SDL_FRect& position, int ImageWidth, int ImageHeight);
		~TImageListView();
		virtual void Draw() override;
		virtual bool Update(float DeltaTime) override;
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

		inline void SetImagePreview(SDL_Texture* ImagePreview)
		{
			m_PreviewImage = ImagePreview;
			m_PreviewImageRect = GetLastClickedImageRect();
			SetTickable(ImagePreview != nullptr);
			TogglePreview();
		}

		void TogglePreview();
		SDL_FRect GetLastClickedImageRect();

	protected:
		std::vector<Tilc::TExtString> m_Images;
		int m_ImageWidth{};
		int m_ImageHeight{};
		int m_ImagesPerRow{};
		int m_ImagesPerColumn{};
		int m_LastCickedImageIndex{};
		bool m_AllowRemoveImage{};
		SDL_Texture* m_PreviewImage{};
		SDL_FRect m_PreviewImageRect{};

		bool m_IsExpanded{false};
		bool m_IsPreviewAnimating{false};
		float m_PreviewAnimProgress{0.0f}; // 0.0 (thumbnail) to 1.0 (original)
		const float m_PreviewAnimDuration{0.4f}; // Duration in seconds

	private:
		SDL_FRect m_PreviewStartRect;
		SDL_FRect m_PreviewTargetRect;
		SDL_FRect m_PreviewCurrentRect;
	};
}
