#pragma once

#include "Tilc/Gui/ScrollBar.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc {
    namespace Gui {

        class DECLSPEC TScrollBarVertical : public TScrollBar {
        public:
            TScrollBarVertical(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, EControlType ControlType = EControlType::ECT_ScrollBar, int thumbPosition = 1, int maxValue = 100, int minValue = 1, bool autosize = false, bool IsStandardSizedScrollBar = true);

            virtual void Draw() override;
            //virtual bool Update(float DeltaTime) override;

            virtual void SetSize(float width, float height) override;
            virtual void SetSizeRelativeToParent(float parentWidth, float parentHeight) override;
            virtual void SetThumbSize() override;
            virtual void CalculateThumbLeftTopCorner(float& x, float& y, int position = INT_MIN) override;
            // Wylicza najbliższą pozycję (this->_position) dla podanych współrzędnych suwaka. Jeśli
            // współrzędna y jest równa INT_MIN, to wyliczana jest pozycja dla bieżącego położenia
            // suwaka.
            virtual int CalculatePositionForThumbCoords(float x, float y) override;
            virtual bool OnMouseMove(const SDL_Event& event) override;
            virtual bool OnMouseButtonDown(const SDL_Event& event) override;
            virtual bool OnMouseButtonUp(const SDL_Event& event) override;

            virtual bool MouseOnThumb(int localX, int localY);
            virtual bool MouseOnArrowUp(int localX, int localY);
            virtual bool MouseOnArrowDown(int localX, int localY);
            virtual bool MouseOnBgUp(int localX, int localY);
            virtual bool MouseOnBgDown(int localX, int localY);
        protected:
            SDL_FRect scrollbar_vertical_arrow_up_rc;
            SDL_FRect scrollbar_vertical_arrow_up_disabled_rc;
            SDL_FRect scrollbar_vertical_arrow_up_hover_rc;
            SDL_FRect scrollbar_vertical_arrow_up_pushed_rc;

            SDL_FRect scrollbar_vertical_arrow_down_rc;
            SDL_FRect scrollbar_vertical_arrow_down_disabled_rc;
            SDL_FRect scrollbar_vertical_arrow_down_hover_rc;
            SDL_FRect scrollbar_vertical_arrow_down_pushed_rc;

            SDL_FRect scrollbar_vertical_thumb_top_rc;
            SDL_FRect scrollbar_vertical_thumb_middle_rc;
            SDL_FRect scrollbar_vertical_thumb_bottom_rc;
            SDL_FRect scrollbar_vertical_thumb_bg_rc;

            SDL_FRect scrollbar_vertical_bg_rc;
            SDL_FRect scrollbar_vertical_bg_disabled_rc;
            SDL_FRect scrollbar_vertical_bg_pushed_rc;
        };

    }
}
