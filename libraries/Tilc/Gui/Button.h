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

        // Umieszcza focus na tej kontrolce a dokładnie:
        //      Jeśli aktywną kontrolką okna Parent nie jest ta kontrolka, to jest wywoływane metoda
        //      ParentWnd->setActiveControl(this) tego okna.
        //      Przełącza stan kontrolki w tryb CBUTTON_STATE_FOCUSED.
        virtual void Focus() override;
        // Usuwa focus z tej kontrolki a dokładnie:
        //      NULL-uje wskaźnik aktywnej kontrolki w ParentWindow
        //      Przełącza kontrolkę w stan CBUTTON_STATE_HOVER lub CBUTTON_STATE_NORMAL
        //      w zależności od aktualnego położenia myszy.
        virtual void LooseFocus() override;

        // Funkcje obsługi zdarzeń
        virtual void Draw() override;

    protected:
        TExtString m_Text;

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
