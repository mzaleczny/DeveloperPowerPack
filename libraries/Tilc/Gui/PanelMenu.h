#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Gui/GuiControl.h"
#include "Tilc/Utils/ExtString.h"

#include <initializer_list>

namespace Tilc
{
    namespace Gui
    {
        struct TPanelMenuItem;
        using TPanelMenuItems = std::vector<TPanelMenuItem>;
        struct DECLSPEC TPanelMenuItem
        {
            SDL_Rect m_IconPosition{};
            SDL_FRect m_ItemRealPosition{};
            Tilc::TExtString m_Caption;
            bool m_Selected{};
            bool m_Hovered{};
            TPanelMenuItems m_ChildItems;
        };

        class DECLSPEC TPanelMenu : public TGuiControl
        {
        public:
            // LineHeight = 0 menas default LineHeight
            TPanelMenu(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& MainIconsSource, const Tilc::TExtString& SubIconsSource, bool FromFile = true);
            virtual ~TPanelMenu();

            virtual void Draw() override;
            virtual bool OnMouseMove(const SDL_Event& event) override;
            virtual bool OnMouseButtonDown(const SDL_Event& event) override;

            void SetItems(const std::initializer_list<TPanelMenuItem>& Items);
            void SelectItemExclusive(TPanelMenuItem* ItemToSelect);
            TPanelMenuItem* GetSelectedItem() const { return m_SelectedItem; }

        protected:
            SDL_Color m_BgColor{ 0xff, 0xff, 0xff, 0xff };
            SDL_Color m_BorderColor{ 0x00, 0x00, 0x00, 0xff };
            SDL_Color m_SelectionColor{ 0xc0, 0xc0, 0xc0, 0xff };
            SDL_Color m_HoverColor{ 0xd9, 0xd9, 0xd9, 0xff };
            SDL_Color m_TextColor{ 0x00, 0x00, 0x00, 0xff };
            // Ponizszych dwóch teksturyne zwalniamy, bo są one zarządzane przez Tilc::Resources::TResourceTexture i przez niego zwalniane
            SDL_Texture* m_MainIconsTexture{};
            SDL_Texture* m_SubIconsTexture{};
            TPanelMenuItems m_Items;
            TPanelMenuItem* m_SelectedItem{};
            TPanelMenuItem* GetItemForPosition(float X, float Y);
            void HoverItem(TPanelMenuItem* ItemToSelect);
        };
    }
}
