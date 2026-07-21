#include "Tilc/Gui/Listbox.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/ScrollBarVertical.h"
#include "Tilc/Gui/ScrollBarHorizontal.h"
#include "Tilc/Utils/StdObject.h"
#include <algorithm>
#include <ranges>

Tilc::Gui::TListbox::TListbox(Tilc::Gui::TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, std::initializer_list<const char*> items)
    : Tilc::Gui::TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_Listbox)
{
    m_TopItemIndex = -1;
    m_TabStop = true;
    m_Padding = 2;

    Tilc::Gui::TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    Tilc::Gui::TFont* font = t->DefaultFont;
    if (m_SpaceWidth == 0)
    {
        font->GetTextSize("Testing A string qjy", m_SpaceWidth, m_SpaceHeight);
        m_MeasuredTextSize.x = m_SpaceWidth;
        m_MeasuredTextSize.y = m_SpaceHeight;
    }

    SetItems(items);
}

Tilc::Gui::TListbox::~TListbox()
{
    DeleteItems();
}

void Tilc::Gui::TListbox::DeleteItems()
{
    std::ranges::for_each(m_Items, [](TGuiControlItem* Item) { delete Item; });
    m_Items.clear();
}

void Tilc::Gui::TListbox::SetScrollBars()
{
    float AllItemsHeight = m_Items.size() * m_MeasuredTextSize.y;
    bool addVScr = m_RealPosition.h < AllItemsHeight;
    bool addHScr = false;//allowedTextAreaWidth < maxItemWidth;

    if (addVScr)
    {
        if (!m_VScrollBar)
        {
            AddVerticalScrollBar(0, m_Items.size() - m_VisibleItems, 0, false);
        }
        else
        {
            m_VScrollBar->SetMinMaxValues(0, m_Items.size() - m_VisibleItems);
        }
        m_VScrollBar->Show();
    }
    else
    {
        if (m_VScrollBar)
        {
            m_VScrollBar->Hide();
        }
    }


    if (addHScr)
    {
        if (!m_HScrollBar)
        {
    //        AddHorizontalScrollBar(0, maxItemWidth - allowedTextAreaWidth, 0, false);
        }
        else
        {
      //      m_HScrollBar->SetMinMaxValues(0, m_Items.size() - m_VisibleItems);
        }
        m_HScrollBar->Show();
    }
    else
    {
        if (m_HScrollBar)
        {
            m_HScrollBar->Hide();
        }
    }

    SetMaxAvailableSizeOfScrollBars();
}

void Tilc::Gui::TListbox::SetItems(std::initializer_list<const char*> items, bool redraw)
{
    DeleteItems();
    Tilc::TStringVector Lst;
    Lst.reserve(items.size());
    for (auto it = items.begin(); it != items.end(); ++it)
    {
        Lst.push_back(*it);
    }
    if (Lst.size() > 0)
    {
        SetItems(Lst, redraw);
    }
    else
    {
        SetScrollBars();
    }
}

void Tilc::Gui::TListbox::SetItems(Tilc::TExtString& items, bool redraw)
{
    DeleteItems();
    Tilc::TStringVector Lst;
    items.Explode('\n', Lst);
    if (Lst.size() > 0)
    {
        SetItems(Lst, redraw);
    }
    else
    {
        SetScrollBars();
    }
}

void Tilc::Gui::TListbox::SetItems(const Tilc::TStringVector& items, bool redraw)
{
    DeleteItems();
    Tilc::Gui::TTheme *t = GetTheme();
    
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
    for (size_t i = 0; i < items.size(); ++i)
    {
        item = new Tilc::Gui::TGuiControlItem(items[i]);
        if (item)
        {
            SDL_FPoint size = {0, 0};
            int Width, Height;
            if (t && t->DefaultFont)
            {
                t->DefaultFont->GetTextSize(item->m_Value.c_str(), Width, Height);
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
            m_Items.push_back(item);
        }
    }
    m_TopItemIndex = 0;

    SetScrollBars();

    if (redraw)
    {
        Invalidate();
    }
}

Tilc::TExtString Tilc::Gui::TListbox::GetText()
{
    Tilc::TExtString result;

    for (size_t i = 0; i < m_Items.size(); ++i)
    {
        if (m_Items[i])
        {
            result += m_Items[i]->m_Value + "\n";
        }
    }

    return result;
}

Tilc::TExtString Tilc::Gui::TListbox::GetTextExt(int selected, int checked, int imageIndex, long long data)
{
    Tilc::TExtString result;

    if (selected == -1 && checked == -1 && imageIndex == -1 && data == 0)
    {
        return "";
    }

    Tilc::Gui::TGuiControlItem* item;
    bool includeItem;

    for (size_t i = 0; i < m_Items.size(); ++i)
    {
        item = m_Items[i];
        if (item)
        {
            includeItem = true;

            if (includeItem && selected > 0 && (bool)selected != item->m_Selected)
            {
                includeItem = false;
            }
            if (includeItem && checked > 0 && (bool)checked != item->m_Checked)
            {
                includeItem = false;
            }
            if (includeItem && imageIndex >= 0 && imageIndex != item->m_ImageIndex)
            {
                includeItem = false;
            }
            if (includeItem && data >= 0 && data != item->m_Data)
            {
                includeItem = false;
            }

            if (includeItem)
            {
                result += item->m_Value + "\n";
            }
        }
    }

    return result;
}

Tilc::TExtString Tilc::Gui::TListbox::GetItemValue(int item)
{
    if (m_Items[item])
    {
        return m_Items[item]->m_Value;
    }
    return "";
}

bool Tilc::Gui::TListbox::IsItemSelected(int item)
{
    if (m_IsMultiselect)
    {
        if (m_Items[item])
        {
            return m_Items[item]->m_Selected;
        }
    }
    else
    {
        return m_SelectedItem == item;
    }

    return false;
}

bool Tilc::Gui::TListbox::IsItemChecked(int item)
{
    if (m_Items[item])
    {
        return m_Items[item]->m_Checked;
    }
    return false;
}

int Tilc::Gui::TListbox::GetItemImageIndex(int item)
{
    if (m_Items[item])
    {
        return m_Items[item]->m_ImageIndex;
    }
    return -1;
}

long long Tilc::Gui::TListbox::GetItemData(int item)
{
    if (m_Items[item])
    {
        return m_Items[item]->m_Data;
    }
    return 0;
}

Tilc::Gui::TGuiControlItemList Tilc::Gui::TListbox::GetSelectedItems()
{
    Tilc::Gui::TGuiControlItemList lst;
    lst.reserve(50);

    std::ranges::for_each(m_Items, [&lst](TGuiControlItem* Item) {
        if (Item->m_Selected)
        {
            lst.push_back(Item);
        }
    });

    return lst;
}

int Tilc::Gui::TListbox::GetSelectedIndex()
{
    if (!m_IsMultiselect)
    {
        return m_SelectedItem;
    }

    return -1;
}

Tilc::TExtString Tilc::Gui::TListbox::GetSelectedItemValue()
{
    if (m_IsMultiselect)
    {
        return GetTextExt(1);
    }
    return GetItemValue(m_SelectedItem);
}

bool Tilc::Gui::TListbox::GetSelectedItemCheckedState()
{
    if (!m_IsMultiselect)
    {
        if (m_SelectedItem >= 0)
        {
            return IsItemChecked(m_SelectedItem);
        }
    }

    return false;
}

int Tilc::Gui::TListbox::GetSelectedItemImageIndex()
{
    if (!m_IsMultiselect)
    {
        if (m_SelectedItem >= 0)
        {
            return GetItemImageIndex(m_SelectedItem);
        }
    }

    return -1;
}

int Tilc::Gui::TListbox::GetSelectedItemData()
{
    if (!m_IsMultiselect)
    {
        if (m_SelectedItem >= 0)
        {
            return GetItemData(m_SelectedItem);
        }
    }

    return 0;
}

void Tilc::Gui::TListbox::SelectItem(int item)
{
    if (m_IsMultiselect)
    {
        Tilc::Gui::TGuiControlItem* gcItem = GetItem(item);
        if (gcItem)
        {
            gcItem->m_Selected = true;
        }
    }
    else
    {
        m_SelectedItem = item;
    }

    Invalidate();
}

void Tilc::Gui::TListbox::SelectItem(const Tilc::TExtString& item)
{
    for (auto it = m_Items.begin(); it != m_Items.end(); ++it)
    {
        if ((*it)->m_Value == item)
        {
            (*it)->m_Selected = true;
            if (!m_IsMultiselect)
            {
                break;
            }
        }
    }
    Invalidate();
 }

void Tilc::Gui::TListbox::SetItemChecked(int item)
{
    m_Items[item]->m_Checked = true;

    Invalidate();
}

void Tilc::Gui::TListbox::SetItemChecked(const Tilc::TExtString& item)
{
    for (auto it = m_Items.begin(); it != m_Items.end(); ++it)
    {
        if ((*it)->m_Value == item)
        {
            (*it)->m_Checked = true;
        }
    }
    Invalidate();
}

void Tilc::Gui::TListbox::SetItemData(int item, long long data)
{
    m_Items[item]->m_Data = data;
    Invalidate();
}

void Tilc::Gui::TListbox::SetItemData(const Tilc::TExtString& item, long long data)
{
    for (auto it = m_Items.begin(); it != m_Items.end(); ++it)
    {
        if ((*it)->m_Value == item)
        {
            (*it)->m_Data = data;
        }
    }
    Invalidate();
}

void Tilc::Gui::TListbox::SetItemImageIndex(int item, int imageIndex)
{
    m_Items[item]->m_ImageIndex = imageIndex;
    Invalidate();
}

void Tilc::Gui::TListbox::SetItemImageIndex(const Tilc::TExtString& item, int imageIndex)
{
    for (auto it = m_Items.begin(); it != m_Items.end(); ++it)
    {
        if ((*it)->m_Value == item)
        {
            (*it)->m_ImageIndex = imageIndex;
        }
    }
    Invalidate();
}

void Tilc::Gui::TListbox::SetItemAttributes(int item, bool selected, bool checked, int imageIndex, long long data)
{
    Tilc::Gui::TGuiControlItem* gcItem = GetItem(item);
    if (gcItem)
    {
        gcItem->m_Selected = selected;
        gcItem->m_Checked = checked;
        gcItem->m_Data = data;
        gcItem->m_ImageIndex = imageIndex;
    }
    Invalidate();
}

void Tilc::Gui::TListbox::SetItemAttributes(const Tilc::TExtString& item, bool selected, bool checked, int imageIndex, long long data)
{
    Tilc::Gui::TGuiControlItem* gcItem;
    for (auto it = m_Items.begin(); it != m_Items.end(); ++it)
    {
        gcItem = *it;
        if (gcItem->m_Value == item)
        {
            gcItem->m_Selected = selected;
            gcItem->m_Checked = checked;
            gcItem->m_ImageIndex = imageIndex;
            gcItem->m_Data = data;
        }
    }
    Invalidate();
}


int Tilc::Gui::TListbox::GetInnerTopLeftX()
{
	Tilc::Gui::TTheme* t = GetTheme();
    return m_Padding + t->listbox_frame_top_left_rc.w;
}

int Tilc::Gui::TListbox::GetInnerTopLeftY()
{
    Tilc::Gui::TTheme* t = GetTheme();
    return m_Padding + t->listbox_frame_top_left_rc.h;
}

SDL_FPoint Tilc::Gui::TListbox::GetInnerSize()
{
    Tilc::Gui::TTheme* t = GetTheme();
    SDL_FPoint size{};
    size.x = m_Position.w;
    size.y = m_Position.h;
    if (t)
    {
        size.x -= m_Padding;
        size.x -= t->listbox_frame_top_left_rc.w;
        size.x -= t->listbox_frame_top_right_rc.w;
        if (m_VScrollBar && m_VScrollBar->IsVisible())
        {
            size.x -= m_VScrollBar->m_RealPosition.w;
        }

        size.y -= m_Padding;
        size.y -= t->listbox_frame_top_left_rc.h;
        size.y -= t->listbox_frame_bottom_left_rc.h;
        if (m_HScrollBar && m_HScrollBar->IsVisible())
        {
            size.y -= m_HScrollBar->m_RealPosition.h;
        }
    }
    return size;
}

void Tilc::Gui::TListbox::Draw()
{
    if (!m_Visible) return;
    Tilc::Gui::TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    SDL_Texture* OldRenderTarget{ nullptr };
    Tilc::Gui::TFont* font = t->DefaultFont;

    if (m_Canvas)
    {
        OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);
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
    // Draw TListbox
    // ================================================================
    DrawCommonComplex(
        GetRealPosition(),
        frame_tl, frame_t, frame_tr, frame_l, frame_r, frame_bl, frame_b, frame_br,
        frame_tl, frame_t, frame_tr, frame_l, frame_r, frame_bl, frame_b, frame_br,
        frame_tl, frame_t, frame_tr, frame_l, frame_r, frame_bl, frame_b, frame_br,
        frame_tl, frame_t, frame_tr, frame_l, frame_r, frame_bl, frame_b, frame_br,
        frame_tl, frame_t, frame_tr, frame_l, frame_r, frame_bl, frame_b, frame_br,
        frame_tl, frame_t, frame_tr, frame_l, frame_r, frame_bl, frame_b, frame_br,
        frame_tl, frame_t, frame_tr, frame_l, frame_r, frame_bl, frame_b, frame_br
    );

    rc = GetRealPosition();
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
            SDL_FPoint innerSize = GetInnerSize();
            SDL_FPoint size{ static_cast<float>(m_SpaceWidth), static_cast<float>(m_SpaceHeight) };
            x = GetInnerTopLeftX() + m_RealPosition.x;
            y = GetInnerTopLeftY() + m_RealPosition.y;
            SDL_FRect itemRect {static_cast<float>(x), static_cast<float>(y), static_cast<float>(innerSize.x), static_cast<float>(size.y)};
            for (int i = m_TopItemIndex; i < m_Items.size(); ++i)
            {
                if (itemRect.y >= m_RealPosition.y + innerSize.y)
                {
                    break;
                }
                if (itemRect.y + itemRect.h  >= m_RealPosition.y + innerSize.y)
                {
                    itemRect.h = m_RealPosition.y + innerSize.y - itemRect.y;
                }
                item = m_Items[i];
                if (item)
                {
                    if (m_IsMultiselect)
                    {
                        if (item->m_Selected)
                        {
                            //RenderTiledTexture(t->GuiTextureMap1, &t->listbox_bg_selected_rc, &itemRect);
                            SDL_SetRenderDrawColor(Renderer, t->listbox_bg_selected.r, t->listbox_bg_selected.g, t->listbox_bg_selected.b, t->listbox_bg_selected.a);
                            SDL_RenderFillRect(Renderer, &itemRect);
                        }
                    }
                    else
                    {
                        if (m_SelectedItem == i)
                        {
                            //RenderTiledTexture(t->GuiTextureMap1, &t->listbox_bg_selected_rc, &itemRect);
                            SDL_SetRenderDrawColor(Renderer, t->listbox_bg_selected.r, t->listbox_bg_selected.g, t->listbox_bg_selected.b, t->listbox_bg_selected.a);
                            SDL_RenderFillRect(Renderer, &itemRect);
                        }
                    }
                    font->DrawString(Renderer, item->m_Value.c_str(), &itemRect);
                    itemRect.y += size.y;
                }
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

bool Tilc::Gui::TListbox::OnMouseButtonDown(const SDL_Event& event)
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
        int ClickedItem = (event.button.y - m_RealPosition.y - t->listbox_frame_top_rc.h) / m_MeasuredTextSize.y;
        SelectItem(ClickedItem);

        return true;
    }

    return false;
}

void Tilc::Gui::TListbox::OnThumbChange(int oldPosition, int curPosition, TScrollBar* scrollbar)
{
    m_TopItemIndex = curPosition;
}
