#pragma once

#include "Tilc/Gui/GuiControl.h"
#include "Tilc/Utils/ExtString.h"
#include <vector>
#include <initializer_list>
#include <SDL3/SDL.h>

namespace Tilc {
    namespace Gui {
/*
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
            Tilc::TExtString GetTextExt(int selected = -1, int checked = -1, int imageIndex = -1, int data = 0);
            TGuiControlItem* GetItem(int item);
            Tilc::TExtString GetItemValue(int item);
            bool IsItemSelected(int item);
            bool IsItemChecked(int item);
            int GetItemImageIndex(int item);
            int GetItemData(int item);

            Tilc::TExtString GetSelectedItemValue();
            bool GetSelectedItemCheckedState();
            int GetSelectedItemImageIndex();
            int GetSelectedItemData();

            TGuiControlItemList GetSelectedItems();
            int GetSelectedIndex();
            inline size_t GetItemsCount()
            {
                if (m_Items)
                {
                    return m_Items->size();
                }
                return 0;
            };

            void SelectItem(int item);
            void SelectItem(Tilc::TExtString& item);
            void SetItemChecked(int item);
            void SetItemChecked(Tilc::TExtString& item);
            void SetItemData(int item, int data);
            void SetItemData(Tilc::TExtString& item, int data);
            void SetItemImageIndex(int item, int imageIndex);
            void SetItemImageIndex(Tilc::TExtString& item, int imageIndex);
            void SetItemAttributes(int item, bool selected, bool checked, int imageIndex, int data);
            void SetItemAttributes(Tilc::TExtString& item, bool selected, bool checked, int imageIndex, int data);

            // Funkcje obsługi zdarzeń
            virtual void Draw() override;
            virtual bool OnMouseMove(const SDL_Event& event) override;
            virtual bool OnMouseButtonDown(const SDL_Event& event) override;
            virtual bool OnMouseButtonUp(const SDL_Event& event) override;

            virtual bool OnKeyDown(const SDL_Event& event) override;
            virtual bool OnTextInput(const SDL_Event& event) override;

        protected:
            TGuiControlItemList m_Items;
            //CImageList* _imageList;
            bool m_IsCheckedList; //if checkbox near items are drawed and item.checked attribute is applied
            bool m_IsMultiselect;
            int m_SelectedItem; // used if this->_isMultiselect is false
            int m_TopItemIndex; // index item displayed at top
            int m_FullVisibleItems; // count of fully visible items
            int m_VisibleItems;     // count of all visible items (including the last one)
            int m_Padding;

            int GetInnerTopLeftX();
            int GetInnerTopLeftY();
            SDL_FPoint GetInnerSize();
            void DeleteItems();
        };
*/
    }
}
