#pragma once

#include "Tilc/Gui/ScrollBar.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc {
    namespace Gui {

        class DECLSPEC TScrollBarHorizontal : public TScrollBar {
        public:
            TScrollBarHorizontal(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, EControlType ControlType = EControlType::ECT_ScrollBar, int thumbPosition = 1, int maxValue = 100, int minValue = 1, bool autosize = false, bool IsStandardSizedScrollBar = true);

            virtual void Draw() override;
            //virtual bool Update(float DeltaTime) override;

            virtual void SetSize(float width, float height) override;
            virtual void SetSizeRelativeToParent(float parentWidth, float parentHeight) override;
            virtual void SetThumbSize() override;
            virtual void CalculateThumbLeftTopCorner(float& x, float& y, int position = INT_MIN) override;
            // Wylicza najbliższą pozycję (this->_position) dla podanych współrzędnych suwaka. Jeśli
            // współrzędna x jest równa MININT, to wyliczana jest pozycja dla bieżącego położenia
            // suwaka.
            virtual int CalculatePositionForThumbCoords(float x, float y) override;
            virtual bool OnMouseMove(const SDL_Event& event) override;
            virtual bool OnMouseButtonDown(const SDL_Event& event) override;
            virtual bool OnMouseButtonUp(const SDL_Event& event) override;

            virtual bool MouseOnThumb(int localX, int localY);
            virtual bool MouseOnArrowLeft(int localX, int localY);
            virtual bool MouseOnArrowRight(int localX, int localY);
            virtual bool MouseOnBgLeft(int localX, int localY);
            virtual bool MouseOnBgRight(int localX, int localY);
        protected:
            SDL_FRect scrollbar_horizontal_arrow_left_rc;
            SDL_FRect scrollbar_horizontal_arrow_left_disabled_rc;
            SDL_FRect scrollbar_horizontal_arrow_left_hover_rc;
            SDL_FRect scrollbar_horizontal_arrow_left_pushed_rc;

            SDL_FRect scrollbar_horizontal_arrow_right_rc;
            SDL_FRect scrollbar_horizontal_arrow_right_disabled_rc;
            SDL_FRect scrollbar_horizontal_arrow_right_hover_rc;
            SDL_FRect scrollbar_horizontal_arrow_right_pushed_rc;

            SDL_FRect scrollbar_horizontal_thumb_left_rc;
            SDL_FRect scrollbar_horizontal_thumb_middle_rc;
            SDL_FRect scrollbar_horizontal_thumb_right_rc;
            SDL_FRect scrollbar_horizontal_thumb_bg_rc;

            SDL_FRect scrollbar_horizontal_bg_rc;
            SDL_FRect scrollbar_horizontal_bg_disabled_rc;
            SDL_FRect scrollbar_horizontal_bg_pushed_rc;
        };
    }
}
