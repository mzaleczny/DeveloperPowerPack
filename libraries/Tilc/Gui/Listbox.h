#pragma once

#include "Tilc/Gui/GuiControl.h"
#include "Tilc/Utils/ExtString.h"
#include <vector>
#include <initializer_list>
#include <SDL3/SDL.h>

namespace Tilc {
    namespace Gui {
        class DECLSPEC TListbox : public TGuiControl
        {
        public:
            TListbox(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, std::initializer_list<const char*> items);
            virtual  ~TListbox();

            void SetItems(std::initializer_list<const char*> items, bool redraw = true);
            void SetItems(Tilc::TExtString& items, bool redraw = true);
            void SetItems(const Tilc::TStringVector& items, bool redraw = true);

            // zwraca przechowywany w kontrolce tekst
            virtual Tilc::TExtString GetText() override;
            Tilc::TExtString GetTextExt(int selected = -1, int checked = -1, int imageIndex = -1, long long data = 0);
            inline TGuiControlItem* GetItem(int item) {
                return m_Items[item];
            }
            Tilc::TExtString GetItemValue(int item);
            bool IsItemSelected(int item);
            bool IsItemChecked(int item);
            int GetItemImageIndex(int item);
            long long GetItemData(int item);

            Tilc::TExtString GetSelectedItemValue();
            bool GetSelectedItemCheckedState();
            int GetSelectedItemImageIndex();
            int GetSelectedItemData();

            TGuiControlItemList GetSelectedItems();
            int GetSelectedIndex();
            inline size_t GetItemsCount()
            {
                return m_Items.size();
            };

            void SelectItem(int item);
            void SelectItem(const Tilc::TExtString& item);
            void SetItemChecked(int item);
            void SetItemChecked(const Tilc::TExtString& item);
            void SetItemData(int item, long long data);
            void SetItemData(const Tilc::TExtString& item, long long data);
            void SetItemImageIndex(int item, int imageIndex);
            void SetItemImageIndex(const Tilc::TExtString& item, int imageIndex);
            void SetItemAttributes(int item, bool selected, bool checked, int imageIndex, long long data);
            void SetItemAttributes(const Tilc::TExtString& item, bool selected, bool checked, int imageIndex, long long data);

            // Funkcje obsługi zdarzeń
            virtual void Draw() override;
            virtual bool OnMouseButtonDown(const SDL_Event& event) override;

        protected:
            TGuiControlItemList m_Items;
            SDL_FPoint m_MeasuredTextSize;
            //CImageList* _imageList;
            bool m_IsCheckedList{}; //if checkbox near items are drawed and item.checked attribute is applied
            bool m_IsMultiselect{};
            int m_SelectedItem{}; // used if this->_isMultiselect is false
            int m_TopItemIndex{}; // index item displayed at top
            int m_FullVisibleItems{}; // count of fully visible items
            int m_VisibleItems{};     // count of all visible items (including the last one)
            int m_Padding{};
            int m_SpaceWidth{};
            int m_SpaceHeight{};

            int GetInnerTopLeftX();
            int GetInnerTopLeftY();
            SDL_FPoint GetInnerSize();
            void DeleteItems();
        };
    }
}
