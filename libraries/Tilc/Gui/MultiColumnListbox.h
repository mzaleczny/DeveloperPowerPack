#pragma once

#include "Tilc/Gui/Listbox.h"
#include <vector>
#include <initializer_list>
#include <SDL3/SDL.h>

namespace Tilc {
    namespace Gui {
        class DECLSPEC TMultiColumnListbox : public TListbox
        {
        public:
            TMultiColumnListbox(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, std::vector<std::initializer_list<const char*>> items);
            virtual  ~TMultiColumnListbox();

            void SetItems(std::vector<std::initializer_list<const char*>> items, bool redraw = true);
            void SetColumnWidths(std::initializer_list<float> ColumnWidths);

            // Funkcje obsługi zdarzeń
            virtual void Draw() override;
            virtual bool OnMouseButtonDown(const SDL_Event& event) override;
            virtual void OnThumbChange(int oldPosition, int curPosition, TScrollBar* scrollbar) override;

        protected:
            std::vector<float> m_ColumnWidths;
            virtual void DeleteItems() override;
            virtual void SetScrollBars() override;
        };
    }
}
