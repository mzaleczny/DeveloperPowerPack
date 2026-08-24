#pragma once

#include "Tilc/Gui/TextField.h"
#include "Tilc/Utils/ExtString.h"
#include <initializer_list>
#include <vector>

namespace Tilc {
    namespace Gui {
        class DECLSPEC TComboBox : public TTextField
        {
        public:
            // LineHeight = 0 menas default LineHeight
            TComboBox(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const TExtString& text = "", const std::initializer_list<const char*>& Items = {}, bool tabStop = true);
            virtual ~TComboBox();

            void SetItems(const std::initializer_list<const char*>& Items);
            virtual void Draw() override;
            virtual bool Update(float DeltaTime) override;

            // Funkcje obsługi zdarzeń
            virtual bool OnMouseMove(const SDL_Event& event) override;
            virtual bool OnMouseButtonDown(const SDL_Event& event) override;
        protected:
            float m_CurrentDropDownHeight{};
            float m_DestinationDropDownHeight{100.0f};
            int m_RollDirection{ 1 };
            float m_RollSpeed{ 1.0f };
            bool m_DropDownVisible{};
            std::vector<Tilc::TExtString> m_Items;
            SDL_FRect m_ChevronRect;
        };
    }
}
