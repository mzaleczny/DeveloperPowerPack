#include "Tilc/Gui/PanelMenu.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Resources/TextureManager.h"

Tilc::Gui::TPanelMenu::TPanelMenu(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& MainIconsSource, const Tilc::TExtString& SubIconsSource, bool FromFile)
    : Tilc::Gui::TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_PanelMenu)
{
    Tilc::Resources::TResourceTexture* ResourceTexture{};

    if (FromFile)
    {
        ResourceTexture = Tilc::GameObject->GetContext()->m_TextureManager->AddResourceFromFile(m_Name + "_Main", MainIconsSource, "SDL_Texture");
        if (ResourceTexture)
        {
            m_MainIconsTexture = ResourceTexture->AsSDLTexture();
        }
        ResourceTexture = Tilc::GameObject->GetContext()->m_TextureManager->AddResourceFromFile(m_Name + "_Sub", SubIconsSource, "SDL_Texture");
        if (ResourceTexture)
        {
            m_SubIconsTexture = ResourceTexture->AsSDLTexture();
        }
    }
}

Tilc::Gui::TPanelMenu::~TPanelMenu()
{
}

void Tilc::Gui::TPanelMenu::Draw()
{
    if (!m_Visible) return;
    m_NeedUpdate = ENeedUpdate::ENU_None;

    TTheme* m_Theme = Tilc::GameObject->GetContext()->m_Theme;
    Tilc::Gui::TFont* Font = m_Theme->DefaultFont;
    SDL_Texture* OldRenderTarget{};
    float x = 0;
    float y = 0;
    SDL_FRect SrcRc, DstRc;

    if (m_Canvas)
    {
        OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);
    }

    SDL_SetRenderDrawColor(Renderer, m_BgColor.r, m_BgColor.g, m_BgColor.b, m_BgColor.a);
    SDL_RenderFillRect(Renderer, &m_RealPosition);
    SDL_SetRenderDrawColor(Renderer, m_BorderColor.r, m_BorderColor.g, m_BorderColor.b, m_BorderColor.a);
    SDL_RenderRect(Renderer, &m_RealPosition);

    int Width{}, Height{};
    x = m_RealPosition.x + 20.0f;
    y = m_RealPosition.y + 1;
    for (size_t i = 0; i < m_Items.size(); ++i)
    {
        // Rysujemy ikonę
        SrcRc = Tilc::GetRect(m_Items[i].m_IconPosition.x, m_Items[i].m_IconPosition.y, m_Items[i].m_IconPosition.w, m_Items[i].m_IconPosition.h);
        DstRc = { x, y, static_cast<float>(m_Items[i].m_IconPosition.w), static_cast<float>(m_Items[i].m_IconPosition.h) };
        SDL_RenderTexture(Renderer, m_MainIconsTexture, &SrcRc, &DstRc);
        y += DstRc.h - 10.0f;


        // Rysujemy zaznaczenie
        if (m_Items[i].m_Selected)
        {
            SDL_SetRenderDrawColor(Renderer, m_SelectionColor.r, m_SelectionColor.g, m_SelectionColor.b, m_SelectionColor.a);
            SDL_RenderFillRect(Renderer, &m_Items[i].m_ItemRealPosition);
        }
        else
        {
            // Rysujemy hover-zaznaczenie
            if (m_Items[i].m_Hovered)
            {
                SDL_SetRenderDrawColor(Renderer, m_HoverColor.r, m_HoverColor.g, m_HoverColor.b, m_HoverColor.a);
                SDL_RenderFillRect(Renderer, &m_Items[i].m_ItemRealPosition);
            }
        }

        // Rysujemy tekst
        Font->SetColor(m_TextColor);
        Font->DrawString(Renderer, m_Items[i].m_Caption.c_str(), x, y);
        Font->GetTextSize(m_Items[i].m_Caption.c_str(), Width, Height);
        if (m_Items[i].m_ItemRealPosition.x < 0.1f)
        {
            m_Items[i].m_ItemRealPosition.x = m_RealPosition.x;
            m_Items[i].m_ItemRealPosition.y = y;
            m_Items[i].m_ItemRealPosition.w = m_RealPosition.w;
            m_Items[i].m_ItemRealPosition.h = Height;

            m_Items[i].m_ItemRealPosition.x += 1.0f;
            m_Items[i].m_ItemRealPosition.w -= 2.0f;
            m_Items[i].m_ItemRealPosition.y -= 2;
            if (m_Items[i].m_ItemRealPosition.y < 0.01f)
            {
                m_Items[i].m_ItemRealPosition.y = 0.0f;
            }
            m_Items[i].m_ItemRealPosition.h += 4;
        }
        y += 30.0f;

        for (size_t j = 0; j < m_Items[i].m_ChildItems.size(); ++j)
        {
            Tilc::Gui::TPanelMenuItem& SubItem = m_Items[i].m_ChildItems[j];

            // Rysujemy zaznaczenie
            if (SubItem.m_Selected)
            {
                SDL_SetRenderDrawColor(Renderer, m_SelectionColor.r, m_SelectionColor.g, m_SelectionColor.b, m_SelectionColor.a);
                SDL_RenderFillRect(Renderer, &SubItem.m_ItemRealPosition);
            }
            else
            {
                // Rysujemy hover-zaznaczenie
                if (SubItem.m_Hovered)
                {
                    SDL_SetRenderDrawColor(Renderer, m_HoverColor.r, m_HoverColor.g, m_HoverColor.b, m_HoverColor.a);
                    SDL_RenderFillRect(Renderer, &SubItem.m_ItemRealPosition);
                }
            }

            // Rysujemy ikonkę
            SrcRc = Tilc::GetRect(SubItem.m_IconPosition.x, SubItem.m_IconPosition.y, SubItem.m_IconPosition.w, SubItem.m_IconPosition.h);
            DstRc = { x, y, static_cast<float>(SubItem.m_IconPosition.w), static_cast<float>(SubItem.m_IconPosition.h) };
            SDL_RenderTexture(Renderer, m_SubIconsTexture, &SrcRc, &DstRc);

            // Rysujemy tekst
            Font->SetColor(m_TextColor);
            Font->GetTextSize(SubItem.m_Caption.c_str(), Width, Height);
            Font->DrawString(Renderer, SubItem.m_Caption.c_str(), x + SubItem.m_IconPosition.w + 8, y + 2.0f);
            if (SubItem.m_ItemRealPosition.w < 0.1f)
            {
                SubItem.m_ItemRealPosition.x = m_RealPosition.x;
                SubItem.m_ItemRealPosition.y = y;
                SubItem.m_ItemRealPosition.w = m_RealPosition.w;
                SubItem.m_ItemRealPosition.h = Height > DstRc.h ? Height : DstRc.h;

                SubItem.m_ItemRealPosition.x += 1.0f;
                SubItem.m_ItemRealPosition.w -= 2.0f;
                SubItem.m_ItemRealPosition.y -= 2;
                if (SubItem.m_ItemRealPosition.y < 0.01f)
                {
                    SubItem.m_ItemRealPosition.y = 0.0f;
                }
                SubItem.m_ItemRealPosition.h += 4;
            }
            y += DstRc.h + 8.0f;
        }

        y += 8.0f;
   }

    if (m_Canvas)
    {
        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
}

bool Tilc::Gui::TPanelMenu::OnMouseMove(const SDL_Event& event)
{
    if (!m_Visible) return false;
    if (OtherControlCapturedMouse())
    {
        return false;
    }

    TGuiControl::OnMouseMove(event);

    if (PointIn(event.button.x, event.button.y))
    {
        Tilc::Gui::TPanelMenuItem* HoveredItem = GetItemForPosition(event.button.x, event.button.y);
        if (HoveredItem)
        {
            HoverItem(HoveredItem);
        }
        Invalidate();
        return true;
    }

    return false;
}

bool Tilc::Gui::TPanelMenu::OnMouseButtonDown(const SDL_Event& event)
{
    if (!m_Visible) return false;
    if (OtherControlCapturedMouse())
    {
        return false;
    }

    TGuiControl::OnMouseButtonDown(event);

    if (PointIn(event.button.x, event.button.y))
    {
        m_SelectedItem = GetItemForPosition(event.button.x, event.button.y);
        // jeśli m_SelectedItem == nulptr, to wszystkie pozycje zostaną odznaczone
        SelectItemExclusive(m_SelectedItem);
        Invalidate();
        return true;
    }

    return false;
}

void Tilc::Gui::TPanelMenu::SetItems(const std::initializer_list<TPanelMenuItem>& Items)
{
    m_Items.reserve(Items.size());
    for (auto it = Items.begin(); it != Items.end(); ++it)
    {
        m_Items.emplace_back(*it);
    }
}

void Tilc::Gui::TPanelMenu::SelectItemExclusive(TPanelMenuItem* ItemToSelect)
{
    for (size_t i = 0; i < m_Items.size(); ++i)
    {
        if (&m_Items[i] == ItemToSelect)
        {
            ItemToSelect->m_Selected = true;
        }
        else
        {
            m_Items[i].m_Selected = false;
        }

        for (size_t j = 0; j < m_Items[i].m_ChildItems.size(); ++j)
        {
            if (&m_Items[i].m_ChildItems[j] == ItemToSelect)
            {
                ItemToSelect->m_Selected = true;
            }
            else
            {
                m_Items[i].m_ChildItems[j].m_Selected = false;
            }
        }
    }
}

Tilc::Gui::TPanelMenuItem* Tilc::Gui::TPanelMenu::GetItemForPosition(float X, float Y)
{
    SDL_FPoint pt{ X, Y };
    for (size_t i = 0; i < m_Items.size(); ++i)
    {
        if (SDL_PointInRectFloat(&pt, &m_Items[i].m_ItemRealPosition))
        {
            return &m_Items[i];
        }

        for (size_t j = 0; j < m_Items[i].m_ChildItems.size(); ++j)
        {
            if (SDL_PointInRectFloat(&pt, &m_Items[i].m_ChildItems[j].m_ItemRealPosition))
            {
                return &m_Items[i].m_ChildItems[j];
            }
        }
    }
    return nullptr;
}

void Tilc::Gui::TPanelMenu::HoverItem(TPanelMenuItem* ItemToSelect)
{
    for (size_t i = 0; i < m_Items.size(); ++i)
    {
        if (&m_Items[i] == ItemToSelect)
        {
            ItemToSelect->m_Hovered = true;
        }
        else
        {
            m_Items[i].m_Hovered = false;
        }

        for (size_t j = 0; j < m_Items[i].m_ChildItems.size(); ++j)
        {
            if (&m_Items[i].m_ChildItems[j] == ItemToSelect)
            {
                ItemToSelect->m_Hovered = true;
            }
            else
            {
                m_Items[i].m_ChildItems[j].m_Hovered = false;
            }
        }
    }
}
