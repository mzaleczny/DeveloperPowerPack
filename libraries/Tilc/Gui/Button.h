#pragma once

#include "Tilc/Gui/GuiControl.h"
#include "Tilc/Utils/ExtString.h"
#include <list>
#include <functional>

namespace Tilc::Gui
{
    class DECLSPEC TButton : public TGuiControl
    {
    public:
        TButton(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const TExtString& text);

        // ustawia w kontrolce podany tekst oraz przed wyświetleniem wycina z niego wszystkie znaki
        // o kodzie < 32
        virtual void SetText(TExtString text, bool redraw = true);
        virtual void SetImagesTextureMap(SDL_Texture* ImagesTextureMap, int ImagesW, int ImagesH,
            int NormalStateImageIndex = -1,
            int HoverStateImageIndex = -1,
            int PushedStateImageIndex = -1,
            int FocusedStateImageIndex = -1,
            int DisabledStateImageIndex = -1);
        virtual void RemoveImage();

        // czyści tekst kontrolki (atrybut this->_text) ze znaków o kodzie < 32.
        virtual void CleanupText();

        // Funkcje obsługi zdarzeń
        virtual void Draw() override;

    protected:
        // Przechowuje wskaźnik na teksturę obrazków z której pobierane są obrazki, tgo wskaźnika nie zwalniamy
        SDL_Texture* m_ImagesTextureMap{};
        SDL_Rect m_NormalStateImageRect{};
        SDL_Rect m_HoverStateImageRect{};
        SDL_Rect m_PushedStateImageRect{};
        SDL_Rect m_FocusedStateImageRect{};
        SDL_Rect m_DisabledStateImageRect{};

        void CommonInit(const TExtString& text);

        // Poniższa funkcja zwraca TRUE, jeśli przetworzyła klawisz i FALSE jeśli go zignorowała.
        virtual bool CommonKeyProcessing(const SDL_Event& Event);
    };
}
