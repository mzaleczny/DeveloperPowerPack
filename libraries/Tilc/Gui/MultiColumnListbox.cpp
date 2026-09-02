#include "Tilc/Gui/MultiColumnListbox.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/ScrollBarVertical.h"
#include "Tilc/Gui/ScrollBarHorizontal.h"
#include "Tilc/Utils/StdObject.h"
#include <algorithm>
#include <ranges>

Tilc::Gui::TMultiColumnListbox::TMultiColumnListbox(Tilc::Gui::TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const std::vector<std::initializer_list<const char*>>& items)
    : Tilc::Gui::TListbox(parent, name, position, Tilc::Gui::EControlType::ECT_MultiColumnListbox)
{
    SetItems(items);
}

Tilc::Gui::TMultiColumnListbox::~TMultiColumnListbox()
{
    DeleteItems();
}

void Tilc::Gui::TMultiColumnListbox::SetScrollBars()
{
    TListbox::SetScrollBars();

    SetMaxAvailableSizeOfScrollBars();
}

void Tilc::Gui::TMultiColumnListbox::DeleteItems()
{
    std::ranges::for_each(m_Items, [](TGuiControlItem* Item) { delete Item; });
    m_Items.clear();
    m_ColumnWidths.clear();
}

void Tilc::Gui::TMultiColumnListbox::SetItems(const std::vector<std::initializer_list<const char*>>& Items, bool redraw)
{
    DeleteItems();
    Tilc::Gui::TTheme* t = GetTheme();

    SDL_FPoint size = GetInnerSize();
    float allowedTextAreaWidth = size.x;
    float allowedTextAreaHeight = size.y;

    Tilc::Gui::TGuiControlItem* item;
    float maxItemWidth = 0;
    float maxItemHeight = 0;
    float summaryItemHeight = 0;
    float visibleItems = 0;

    m_FullVisibleItems = 0;
    m_VisibleItems = 0;
    for (size_t i = 0; i < Items.size(); ++i)
    {
        item = new Tilc::Gui::TGuiControlItem(*Items[i].begin());
        if (item)
        {
            SDL_FPoint size = { 0, 0 };
            int Width, Height;
            if (t && t->DefaultFont)
            {
                t->DefaultFont->GetTextSize(item->m_Value.c_str(), Width, Height);
                // overwrite text height as m_MeasuredTextSize.y can be 1 pixel greater ten current Height, so we must unifying this
                Height = m_MeasuredTextSize.y;
                if (Width > maxItemWidth)
                {
                    maxItemWidth += Width;
                }
                if (Height > maxItemHeight)
                {
                    maxItemHeight += Height;
                }
                if (summaryItemHeight <= allowedTextAreaHeight)
                {
                    m_FullVisibleItems += 1;
                }
                summaryItemHeight += Height;
                if (summaryItemHeight <= allowedTextAreaHeight)
                {
                    m_VisibleItems += 1;
                }
            }
            item->m_Size = { static_cast<float>(Width), static_cast<float>(Height) };

            item->m_Columns.reserve(Items[i].size());
            int Column = 0;
            for (auto it = Items[i].begin(); it != Items[i].end(); ++it, ++Column)
            {
                if (it == Items[i].begin())
                {
                    continue;
                }
                item->m_Columns.push_back(*it);
            }
            m_Items.push_back(item);
        }
    }
    m_TopItemIndex = 0;

    if (Items.size() > 0)
    {
        m_ColumnWidths.reserve(Items[0].size());
        for (size_t j = 0; j < Items[0].size(); ++j)
        {
            m_ColumnWidths.push_back(GetInnerSize().x / Items[0].size());
        }
    }

    SetScrollBars();

    if (redraw)
    {
        Invalidate();
    }
}

void Tilc::Gui::TMultiColumnListbox::SetItems(const std::vector<std::vector<Tilc::TExtString>>& Items, bool redraw)
{
    DeleteItems();
    Tilc::Gui::TTheme* t = GetTheme();

    SDL_FPoint size = GetInnerSize();
    float allowedTextAreaWidth = size.x;
    float allowedTextAreaHeight = size.y;

    Tilc::Gui::TGuiControlItem* item;
    float maxItemWidth = 0;
    float maxItemHeight = 0;
    float summaryItemHeight = 0;
    float visibleItems = 0;

    m_FullVisibleItems = 0;
    m_VisibleItems = 0;
    for (size_t i = 0; i < Items.size(); ++i)
    {
        item = new Tilc::Gui::TGuiControlItem(*Items[i].begin());
        if (item)
        {
            SDL_FPoint size = { 0, 0 };
            int Width, Height;
            if (t && t->DefaultFont)
            {
                t->DefaultFont->GetTextSize(item->m_Value.c_str(), Width, Height);
                // overwrite text height as m_MeasuredTextSize.y can be 1 pixel greater ten current Height, so we must unifying this
                Height = m_MeasuredTextSize.y;
                if (Width > maxItemWidth)
                {
                    maxItemWidth += Width;
                }
                if (Height > maxItemHeight)
                {
                    maxItemHeight += Height;
                }
                if (summaryItemHeight <= allowedTextAreaHeight)
                {
                    m_FullVisibleItems += 1;
                }
                summaryItemHeight += Height;
                if (summaryItemHeight <= allowedTextAreaHeight)
                {
                    m_VisibleItems += 1;
                }
            }
            item->m_Size = { static_cast<float>(Width), static_cast<float>(Height) };

            item->m_Columns.reserve(Items[i].size());
            int Column = 0;
            for (auto it = Items[i].begin(); it != Items[i].end(); ++it, ++Column)
            {
                if (it == Items[i].begin())
                {
                    continue;
                }
                item->m_Columns.push_back(*it);
            }
            m_Items.push_back(item);
        }
    }
    m_TopItemIndex = 0;

    if (Items.size() > 0)
    {
        m_ColumnWidths.reserve(Items[0].size());
        for (size_t j = 0; j < Items[0].size(); ++j)
        {
            m_ColumnWidths.push_back(GetInnerSize().x / Items[0].size());
        }
    }

    SetScrollBars();

    if (redraw)
    {
        Invalidate();
    }
}

void Tilc::Gui::TMultiColumnListbox::SetColumnWidths(std::initializer_list<float> ColumnWidths)
{
    m_ColumnWidths.clear();
    std::ranges::for_each(ColumnWidths, [this](float ItemWidth) { m_ColumnWidths.push_back(ItemWidth); });
}

void Tilc::Gui::TMultiColumnListbox::Draw()
{
    if (!m_Visible) return;
    Draw(m_Canvas, &m_RealPosition);
}

void Tilc::Gui::TMultiColumnListbox::Draw(SDL_Texture* Canvas, SDL_FRect* Position)
{
    if (!m_Visible) return;
    Tilc::Gui::TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    SDL_Texture* OldRenderTarget{ nullptr };
    Tilc::Gui::TFont* font = t->DefaultFont;

    if (Canvas)
    {
        OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, Canvas);
    }

    int x = 0;
    int y = 0;

    SDL_FRect bg = t->listbox_bg_rc;
    SDL_FRect frame_tl = t->listbox_frame_top_left_rc;
    SDL_FRect frame_t = t->listbox_frame_top_rc;
    SDL_FRect frame_tr = t->listbox_frame_top_right_rc;

    SDL_FRect frame_bl = t->listbox_frame_bottom_left_rc;
    SDL_FRect frame_b = t->listbox_frame_bottom_rc;
    SDL_FRect frame_br = t->listbox_frame_bottom_right_rc;

    SDL_FRect frame_l = t->listbox_frame_left_rc;
    SDL_FRect frame_r = t->listbox_frame_right_rc;

    SDL_FRect rc;
    // ================================================================
    // Draw TMultiColumnListbox
    // ================================================================
    DrawCommonComplex(
        *Position,
        frame_tl, frame_t, frame_tr, frame_l, frame_r, frame_bl, frame_b, frame_br,
        frame_tl, frame_t, frame_tr, frame_l, frame_r, frame_bl, frame_b, frame_br,
        frame_tl, frame_t, frame_tr, frame_l, frame_r, frame_bl, frame_b, frame_br,
        frame_tl, frame_t, frame_tr, frame_l, frame_r, frame_bl, frame_b, frame_br,
        frame_tl, frame_t, frame_tr, frame_l, frame_r, frame_bl, frame_b, frame_br,
        frame_tl, frame_t, frame_tr, frame_l, frame_r, frame_bl, frame_b, frame_br,
        frame_tl, frame_t, frame_tr, frame_l, frame_r, frame_bl, frame_b, frame_br
    );

    rc = *Position;
    rc.x += t->listbox_frame_left_rc.w;
    rc.y += t->listbox_frame_top_rc.h;
    rc.w -= t->listbox_frame_left_rc.w + t->listbox_frame_right_rc.w;
    rc.h -= t->listbox_frame_top_rc.h + t->listbox_frame_bottom_rc.h;
    // Line below eats much much much memory, I do not know why
    //RenderTiledTexture(t->GuiTextureMap1, &t->listbox_bg_rc, &rc);
    SDL_SetRenderDrawColor(Renderer, t->listbox_bg.r, t->listbox_bg.g, t->listbox_bg.b, t->listbox_bg.a);
    SDL_RenderFillRect(Renderer, &rc);
    // ================================================================
    // ================================================================

    // ================================================================
    // Draw items
    // ================================================================
    if (m_Items.size() > 0)
    {
        Tilc::Gui::TGuiControlItem* item;
        if (font)
        {
            SDL_FPoint innerSize = GetInnerSize(Position);
            SDL_FPoint size{ static_cast<float>(m_SpaceWidth), static_cast<float>(m_SpaceHeight) };
            x = GetInnerTopLeftX() + Position->x;
            y = GetInnerTopLeftY() + Position->y;
            SDL_FRect itemRect {static_cast<float>(x), static_cast<float>(y), static_cast<float>(innerSize.x), static_cast<float>(size.y)};
            SDL_FRect SubItemRect;
            SDL_Rect OrigClipRect, ControlClipRect;
            font->SetColor(m_TextColor);
            for (int i = m_TopItemIndex; i < m_Items.size(); ++i)
            {
                int Left = itemRect.x;
                int TotalWidth;
                if (itemRect.y + size.y > Position->y + m_Padding + innerSize.y)
                {
                    break;
                }
                if (itemRect.y + itemRect.h >= Position->y + m_Padding + innerSize.y)
                {
                    itemRect.h = Position->y + m_Padding + innerSize.y - itemRect.y;
                }
                if (i - m_TopItemIndex + 1 == m_VisibleItems)
                {
                    ControlClipRect = FRectToRectFloor(&itemRect);
                    SDL_SetRenderClipRect(Renderer, &ControlClipRect);
                }

                if (m_IsMultiselect)
                {
                    if (item->m_Selected)
                    {
                        //RenderTiledTexture(t->GuiTextureMap1, &t->listbox_bg_selected_rc, &itemRect);
                        itemRect.x -= m_Padding;
                        itemRect.w += m_Padding;
                        SDL_SetRenderDrawColor(Renderer, t->listbox_bg_selected.r, t->listbox_bg_selected.g, t->listbox_bg_selected.b, t->listbox_bg_selected.a);
                        SDL_RenderFillRect(Renderer, &itemRect);
                        itemRect.w -= m_Padding;
                        itemRect.x += m_Padding;
                    }
                }
                else
                {
                    if (m_SelectedItem == i)
                    {
                        //RenderTiledTexture(t->GuiTextureMap1, &t->listbox_bg_selected_rc, &itemRect);
                        itemRect.x -= m_Padding;
                        itemRect.w += m_Padding;
                        SDL_SetRenderDrawColor(Renderer, t->listbox_bg_selected.r, t->listbox_bg_selected.g, t->listbox_bg_selected.b, t->listbox_bg_selected.a);
                        SDL_RenderFillRect(Renderer, &itemRect);
                        itemRect.w -= m_Padding;
                        itemRect.x += m_Padding;
                    }
                }

                item = m_Items[i];
                if (item)
                {
                    TotalWidth = 0;
                    for (int j = 0; j < m_ColumnWidths.size(); TotalWidth += m_ColumnWidths[j], ++j)
                    {
                        SubItemRect = itemRect;
                        SubItemRect.x += TotalWidth;
                        SubItemRect.w = m_ColumnWidths[j];
                        if (SubItemRect.x + SubItemRect.w > itemRect.w)
                        {
                            SubItemRect.w = itemRect.w - SubItemRect.x;
                        }

                        if (j == 0)
                        {
                            font->DrawString(Renderer, item->m_Value.c_str(), &SubItemRect);
                        }
                        else
                        {
                            font->DrawString(Renderer, item->m_Columns[j - 1].c_str(), &SubItemRect);
                        }
                    }
                }
                itemRect.y += size.y;
                if (i - m_TopItemIndex + 1 == m_VisibleItems)
                {
                    OrigClipRect = FRectToRectFloor(&GetParentWindow()->m_Position);
                    SDL_SetRenderClipRect(Renderer, &OrigClipRect);
                }
            }

            SDL_SetRenderDrawColor(Renderer, 0x33, 0x66, 0x99, 0xff);
            float OffsetX = 0;
            // Nie renderujemy ostatniego obramowania - pionowej kreski, żeby ostatnie pole miało całą dostepną szerokość
            for (size_t i = 0; i < m_ColumnWidths.size() - 1; ++i)
            {
                SDL_FRect rc = m_RealPosition;
                OffsetX += m_ColumnWidths[i];
                rc.x += OffsetX;               
                rc.w = 1.0f;
                SDL_RenderRect(Renderer, &rc);
            }
        }
    }
    // ================================================================
    // ================================================================

    DrawChildren(false);    

    if (m_Canvas)
    {
        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
    m_NeedUpdate = ENeedUpdate::ENU_None;
}

bool Tilc::Gui::TMultiColumnListbox::OnMouseButtonDown(const SDL_Event& event)
{
    if (!m_Visible) return false;
    if (OtherControlCapturedMouse())
    {
        return false;
    }

    TGuiControl::OnMouseButtonDown(event);

    if (PointIn(event.button.x, event.button.y))
    {
        CaptureMouse(this);

        Tilc::Gui::TTheme* t = GetTheme();
        int ClickedItem = (event.button.y - m_RealPosition.y - t->listbox_frame_top_rc.h);
        if (m_VScrollBar)
        {
            ClickedItem += m_TopItemIndex * m_MeasuredTextSize.y;
        }
        ClickedItem /= m_MeasuredTextSize.y;
        SelectItem(ClickedItem);

        return true;
    }

    return false;
}

void Tilc::Gui::TMultiColumnListbox::OnThumbChange(int oldPosition, int curPosition, TScrollBar* scrollbar)
{
    m_TopItemIndex = curPosition;
}
