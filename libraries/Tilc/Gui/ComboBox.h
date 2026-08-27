#pragma once

#include "Tilc/Gui/TextField.h"
#include "Tilc/Utils/ExtString.h"
#include <initializer_list>
#include <vector>

namespace Tilc {
    namespace Gui {
        class TListbox;

        class DECLSPEC TComboBox : public TTextField
        {
        public:
            // LineHeight = 0 menas default LineHeight
            TComboBox(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const TExtString& text = "", const std::initializer_list<const char*>& Items = {}, bool tabStop = true);
            TComboBox(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const TExtString& text = "", const std::vector<std::initializer_list<const char*>>& Items = {}, bool tabStop = true);
            virtual ~TComboBox();

            // Adds TListbox with items
            void SetItems(const std::initializer_list<const char*>& Items);
            // Adds TMultilineListbox with multi-items
            void SetItems(const std::vector<std::initializer_list<const char*>>& Items, bool redraw = true);
            virtual void Draw() override;
            virtual bool Update(float DeltaTime) override;

            // Funkcje obsługi zdarzeń
            virtual bool OnMouseMove(const SDL_Event& event) override;
            virtual bool OnMouseButtonDown(const SDL_Event& event) override;

            friend int DefaultComboDropDownClicked(float x, float y, Uint8 MouseButton, Tilc::Gui::TGuiControl* DropDown);
        protected:
            float m_CurrentDropDownHeight{};
            float m_DestinationDropDownHeight{154.0f};
            int m_RollDirection{ 0 };
            float m_RollSpeed{ 750.0f };
            bool m_DropDownVisible{};
            bool m_AllowCloseDropDownList{ true };
            TListbox* m_DropDownItems{};
            SDL_FRect m_ChevronRect;
            SDL_FRect m_DropDownRect;
            void AddDropDownToPrivilegedList();
            void RemoveDropDownFromPrivilegedList();
        };
    }
}
