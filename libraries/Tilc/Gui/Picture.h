#pragma once

#include "Tilc/Gui/GuiControl.h"
#include "Tilc/Utils/ExtString.h"


namespace Tilc {
    namespace Gui
    {
        class DECLSPEC TPicture : public TGuiControl
        {
        public:
            // LineHeight = 0 menas default LineHeight
            TPicture(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, SDL_Texture* Texture);
            virtual ~TPicture();

            virtual void Draw() override;
            inline  void SetPicture(SDL_Texture* Texture)
            {
                // m_Bg is not released texture in class TGuiControl
                m_Picture = Texture;
            }
        protected:
            SDL_Texture* m_Picture{};
        };
    }
}
