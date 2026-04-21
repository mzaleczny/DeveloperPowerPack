#pragma once

#include "Tilc/Gui/ScrollBarVertical.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc {
    namespace Gui {

        class DECLSPEC TSliderVertical : public TScrollBarVertical {
        public:
            TSliderVertical(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, int thumbPosition = 1, int maxValue = 100, int minValue = 1, bool autosize = false); 

            virtual void Draw() override;
            //virtual bool Update(float DeltaTime) override;
            virtual void SetSize(float width, float height) override;
            virtual void SetSizeRelativeToParent(float parentWidth, float parentHeight) override;

            virtual void CalculateThumbLeftTopCorner(float& x, float& y, int position = INT_MIN) override;
            // Wylicza najbliższą pozycję (this->_position) dla podanych współrzędnych suwaka. Jeśli
            // współrzędna x jest równa MININT, to wyliczana jest pozycja dla bieżącego położenia
            // suwaka.
            virtual int CalculatePositionForThumbCoords(float x, float y) override;
            virtual bool OnMouseMove(const SDL_Event& event) override;
            virtual bool OnMouseButtonDown(const SDL_Event& event) override;
            virtual bool OnMouseButtonUp(const SDL_Event& event) override;

            virtual bool MouseOnThumb(int localX, int localY) override;
            virtual bool MouseOnBgUp(int localX, int localY) override;
            virtual bool MouseOnBgDown(int localX, int localY) override;
        };
    }
}
