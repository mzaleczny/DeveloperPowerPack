#include "Tilc/Gui/Listbox.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/ScrollbarVertical.h"
#include "Tilc/Gui/ScrollbarHorizontal.h"
#include "Tilc/Utils/StdObject.h"
#include <algorithm>
#include <ranges>

/*
Tilc::Gui::TListbox::TListbox(Tilc::Gui::TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, std::initializer_list<const char*> items)
    : Tilc::Gui::TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_Listbox)
{
    m_TopItemIndex = -1;
    m_TabStop = true;
    m_Padding = 2;
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
}

void Tilc::Gui::TListbox::SetItems(Tilc::TExtString& items, bool redraw = true)
{
    DeleteItems();
    Tilc::TStringVector Lst;
    items.Explode('\n', Lst);
    if (Lst.size() > 0)
    {
        SetItems(Lst, redraw);
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
    RemoveVerticalScrollbar();
    RemoveHorizontalScrollbar();
    bool addVScr = allowedTextAreaHeight < summaryItemHeight;
    bool addHScr = allowedTextAreaWidth < maxItemWidth;
    AddScrollBars(addVScr, addHScr, 0, m_Items.size() - m_VisibleItems, 0, maxItemWidth - allowedTextAreaWidth);

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

Tilc::TExtString Tilc::Gui::TListbox::GetTextExt(int selected, int checked, int imageIndex, int data)
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

CGuiControlItem* Tilc::Gui::TListbox::getItem(int item) {
    CGuiControlItem* result = NULL;

    if (this->_items) {
        result = (CGuiControlItem*)this->_items->getAt(item);
    }

    return result;
}

Tilc::TExtString Tilc::Gui::TListbox::getItemValue(int item) {
    CGuiControlItem* result = this->getItem(item);
    if (result) {
        return result->value;
    }
    return COMMON_EMPTY_STRING;
}

bool Tilc::Gui::TListbox::isItemSelected(int item) {
    if (this->_isMultiselect) {
        CGuiControlItem* result = this->getItem(item);
        if (result) {
            return result->selected;
        }
    } else {
        return this->_selectedItem == item;
    }

    return false;
}

bool Tilc::Gui::TListbox::isItemChecked(int item) {
    CGuiControlItem* result = this->getItem(item);
    if (result) {
        return result->checked;
    }
    return false;
}

int Tilc::Gui::TListbox::getItemImageIndex(int item) {
    CGuiControlItem* result = this->getItem(item);
    if (result) {
        return result->imageIndex;
    }
    return 0;
}

int Tilc::Gui::TListbox::getItemData(int item) {
    CGuiControlItem* result = this->getItem(item);
    if (result) {
        return result->data;
    }
    return 0;
}

CObjectList* Tilc::Gui::TListbox::getSelectedItems() {
    // create object list that does not deletes its items in destructor
    CObjectList* lst = new CObjectList(50, false);
    if (!lst) {
        return NULL;
    }

    if (this->_items) {
        CGuiControlItem* item;
        for (int i = 0; i < this->_items->size(); i++) {
            item = (CGuiControlItem*)this->_items->getAt(i);
            if (item && item->selected) {
                lst->append(item);
            }
        }
    }

    return lst;
}

int Tilc::Gui::TListbox::getSelectedIndex() {
    if (!this->_isMultiselect) {
        return this->_selectedItem;
    }

    return -1;
}

Tilc::TExtString Tilc::Gui::TListbox::getSelectedItemValue() {
    if (this->_isMultiselect) {
        return this->getTextExt(1);
    }
    return this->getItemValue(this->_selectedItem);
}

bool Tilc::Gui::TListbox::getSelectedItemCheckedState() {
    if (!this->_isMultiselect) {
        if (this->_selectedItem >= 0) {
            return this->isItemChecked(this->_selectedItem);
        }
    }

    return false;
}

int Tilc::Gui::TListbox::getSelectedItemImageIndex() {
    if (!this->_isMultiselect) {
        if (this->_selectedItem >= 0) {
            return this->getItemImageIndex(this->_selectedItem);
        }
    }

    return -1;
}

int Tilc::Gui::TListbox::getSelectedItemData() {
    if (!this->_isMultiselect) {
        if (this->_selectedItem >= 0) {
            return this->getItemData(this->_selectedItem);
        }
    }

    return 0;
}

void Tilc::Gui::TListbox::selectItem(int item) {
    if (this->_isMultiselect) {
        CGuiControlItem* gcItem = this->getItem(item);
        if (gcItem) {
            gcItem->selected = true;
        }
    } else {
        this->_selectedItem = item;
    }

    this->redraw();
}

void Tilc::Gui::TListbox::selectItem(Tilc::TExtString item) {
    if (this->_items) {
        CGuiControlItem* gcItem;
        for (int i = 0; i < this->_items->size(); i++) {
            gcItem = (CGuiControlItem*)this->_items->getAt(i);
            if (gcItem && gcItem->value == item) {
                gcItem->selected = true;
                if (!this->_isMultiselect) {
                    break;
                }
            }
        }
    }

    this->redraw();
}

void Tilc::Gui::TListbox::setItemChecked(int item) {
    CGuiControlItem* gcItem = this->getItem(item);
    if (gcItem) {
        gcItem->checked = true;
    }

    this->redraw();
}

void Tilc::Gui::TListbox::setItemChecked(Tilc::TExtString item) {
    if (this->_items) {
        CGuiControlItem* gcItem;
        for (int i = 0; i < this->_items->size(); i++) {
            gcItem = (CGuiControlItem*)this->_items->getAt(i);
            if (gcItem && gcItem->value == item) {
                gcItem->checked = true;
            }
        }
    }

    this->redraw();
}

void Tilc::Gui::TListbox::setItemData(int item, int data) {
    CGuiControlItem* gcItem = this->getItem(item);
    if (gcItem) {
        gcItem->data = data;
    }

    this->redraw();
}

void Tilc::Gui::TListbox::setItemData(Tilc::TExtString item, int data) {
    if (this->_items) {
        CGuiControlItem* gcItem;
        for (int i = 0; i < this->_items->size(); i++) {
            gcItem = (CGuiControlItem*)this->_items->getAt(i);
            if (gcItem && gcItem->value == item) {
                gcItem->data = data;
            }
        }
    }

    this->redraw();
}

void Tilc::Gui::TListbox::setItemImageIndex(int item, int imageIndex) {
    CGuiControlItem* gcItem = this->getItem(item);
    if (gcItem) {
        gcItem->imageIndex = imageIndex;
    }

    this->redraw();
}

void Tilc::Gui::TListbox::setItemImageIndex(Tilc::TExtString item, int imageIndex) {
    if (this->_items) {
        CGuiControlItem* gcItem;
        for (int i = 0; i < this->_items->size(); i++) {
            gcItem = (CGuiControlItem*)this->_items->getAt(i);
            if (gcItem && gcItem->value == item) {
                gcItem->imageIndex = imageIndex;
            }
        }
    }

    this->redraw();
}

void Tilc::Gui::TListbox::setItemAttributes(int item, bool selected, bool checked, int imageIndex, int data) {
    CGuiControlItem* gcItem = this->getItem(item);
    if (gcItem) {
        gcItem->selected = selected;
        gcItem->checked = checked;
        gcItem->data = data;
        gcItem->imageIndex = imageIndex;
    }

    this->redraw();
}

void Tilc::Gui::TListbox::setItemAttributes(Tilc::TExtString item, bool selected, bool checked, int imageIndex, int data) {
    if (this->_items) {
        CGuiControlItem* gcItem;
        for (int i = 0; i < this->_items->size(); i++) {
            gcItem = (CGuiControlItem*)this->_items->getAt(i);
            if (gcItem && gcItem->value == item) {
                gcItem->selected = selected;
                gcItem->checked = checked;
                gcItem->imageIndex = imageIndex;
                gcItem->data = data;
            }
        }
    }

    this->redraw();
}


CFont* Tilc::Gui::TListbox::getFont() {
    CFont* font = this->_theme->commonCheckboxControlFont;
    if (this->_font) {
        font = this->_font;
    }
    return font;
}

int Tilc::Gui::TListbox::_getInnerTopLeftX() {
	CTheme *t = this->_theme;
    int x = this->_padding;
    if (t && t->TListbox_frame_top_left) {
        x += t->TListbox_frame_top_left->width();
    }
    return x;
}

int Tilc::Gui::TListbox::_getInnerTopLeftY() {
	CTheme *t = this->_theme;
    int y = this->_padding;
    if (t && t->TListbox_frame_top_left) {
        y += t->TListbox_frame_top_left->height();
    }
    return y;
}

SIZE Tilc::Gui::TListbox::_getInnerSize() {
    SIZE size;
    size.cx = this->width;
    size.cy = this->height;
	CTheme *t = this->_theme;
    if (t) {
        if (t && t->TListbox_frame_top_left) {
            size.cx -= t->TListbox_frame_top_left->width();
            size.cy -= t->TListbox_frame_top_left->height();
        }
        if (t && t->TListbox_frame_top_right) {
            size.cx -= t->TListbox_frame_top_right->width();
        }
        if (t && t->TListbox_frame_bottom_left) {
            size.cy -= t->TListbox_frame_bottom_left->height();
        }
    }
    return size;
}

void Tilc::Gui::TListbox::_updateCanvas() {
    this->_needUpdate = false;
    if (!this->canvas) {
		return;
    }

	int x = 0;
    int y = 0;
    HDC hdc = GetDC(0);
	CTheme *t = this->_theme;
    CBitmap* bg = t->TListbox_bg_fullscreen;
    CBitmap* frame_tl = t->TListbox_frame_top_left;
    CBitmap* frame_t = t->TListbox_frame_top;
    CBitmap* frame_tr = t->TListbox_frame_top_right;

    CBitmap* frame_bl = t->TListbox_frame_bottom_left;
    CBitmap* frame_b = t->TListbox_frame_bottom;
    CBitmap* frame_br = t->TListbox_frame_bottom_right;

    CBitmap* frame_l = t->TListbox_frame_left;
    CBitmap* frame_r = t->TListbox_frame_right;
    CFont* font = this->getFont();

	this->canvas->beginPaint(hdc);
    // ================================================================
    // Draw TListbox
    // ================================================================
    this->draw(x, y, frame_tl, frame_t, frame_tr, frame_bl, frame_b, frame_br, frame_l, frame_r, bg);
    // ================================================================
    // ================================================================

    // ================================================================
    // Draw focus if applied
    // ================================================================
    // ================================================================
    // ================================================================

    // ================================================================
    // Draw items
    // ================================================================
    if (this->_items && this->_items->size() > 0) {
        CGuiControlItem* item;
        CFont* font = this->getFont();
        if (font) {
            SIZE innerSize = this->_getInnerSize();
            SIZE size = font->measureString(COMMON_MEASURE_STRING, this->canvas->getDC());
            x = this->_getInnerTopLeftX();
            y = this->_getInnerTopLeftY();
            RECT itemRect = {x, y, x + innerSize.cx, y + size.cy};
            for (int i = 0; i < this->_items->size(); i++) {
                if (itemRect.top >= this->height - this->_padding) {
                    break;
                }
                if (itemRect.bottom >= this->height - this->_padding) {
                    itemRect.bottom = this->height - this->_padding - 1;
                }
                item = (CGuiControlItem*)this->_items->getAt(i);
                if (item) {
                    this->canvas->drawText(font, item->value, itemRect, true, DT_LEFT | DT_NOPREFIX);
                    itemRect.top += size.cy;
                    itemRect.bottom += size.cy;
                }
            }
        }
    }
    // ================================================================
    // ================================================================

    this->canvas->endPaint();

	ReleaseDC(0, hdc);
}

void Tilc::Gui::TListbox::onDraw(CBitmap *dest) {
	CSprite::onDraw(dest);
}

void Tilc::Gui::TListbox::focus() {
    CStyledWindow* swnd = this->getParentWindow();
    if (swnd && swnd->getActiveControl() != this) {
        swnd->setActiveControl(this);
        return;
    }
    this->addState(CONTROL_STATE_FOCUSED);
}

void Tilc::Gui::TListbox::looseFocus() {
    CStyledWindow* wnd = this->getParentWindow();
    int x = MININT, y = MININT;
    wnd->setOnlyActiveControlPointer(NULL);
    this->getCurrentMousePosition(x, y);

    if (x != MININT && y != MININT) {
        if (this->pointIn(x, y)) {
            this->addState(CONTROL_STATE_HOVER);
        } else {
            this->removeState(CONTROL_STATE_HOVER);
        }
    } else {
        this->removeState(CONTROL_STATE_HOVER);
    }
}

bool Tilc::Gui::TListbox::onMouseMove(int x, int y) {
    if (!this->_visible) return false;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return false;
    }

    if (this->pointIn(x, y)) {
        this->addState(CONTROL_STATE_HOVER);
        return true;
    } else {
        this->removeState(CONTROL_STATE_HOVER);
    }

    return false;
}

bool Tilc::Gui::TListbox::onMouseDown(int x, int y) {
    if (!this->_visible) return false;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return false;
    }

    if (this->pointIn(x, y)) {
        this->_lMouseButtonPressed = true;
        CStyledWindow* wnd = this->getParentWindow();
        wnd->captureMouse(this);

        if (wnd->getActiveControl() != this) {
            wnd->setActiveControl(this);
        }

        return true;
    }

    return false;
}

bool Tilc::Gui::TListbox::onMouseUp(int x, int y) {
    if (!this->_visible) return false;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return false;
    }

    bool mouseWasPressed = this->_lMouseButtonPressed;
    this->_lMouseButtonPressed = false;
    CStyledWindow* wnd = this->getParentWindow();
    wnd->captureMouse(NULL);

    if (this->pointIn(x, y)) {
        if (wnd->getActiveControl() == this) {
            this->setState(CONTROL_STATE_FOCUSED);
        } else {
            this->setState(CONTROL_STATE_HOVER);
        }

        if (mouseWasPressed) {
            this->onClick();
        }
        return true;
    }


    return false;
}

void Tilc::Gui::TListbox::onClick() {
    __super::onClick();
}

bool Tilc::Gui::TListbox::_commonKeyProcessing(bool vkAlt, bool vkShift, bool vkControl,
            bool vkLAlt, bool vkRAlt,
            bool vkLShift, bool vkRShift,
            bool vkLControl, bool vkRControl,
            bool systemKey,
            Uint virtualCode, Uint scanCode, WCHAR ch, bool& redraw) {
    redraw = false;

    CKeyboard* kbd = this->getKbd();

    return true;
}

bool Tilc::Gui::TListbox::onKeyDown(bool vkAlt, bool vkShift, bool vkControl,
        bool vkLAlt, bool vkRAlt,
        bool vkLShift, bool vkRShift,
        bool vkLControl, bool vkRControl,
        bool systemKey,
        Uint virtualCode, Uint scanCode, WCHAR ch) {

    bool redraw = false;

    // wciśnięte pojedyńczo klawisze systemowe ignorujemy
    if (!systemKey) {
        bool process = !this->_commonKeyProcessing(vkAlt, vkShift, vkControl, vkLAlt, vkRAlt, vkLShift, vkRShift,
                                    vkLControl, vkRControl, systemKey, virtualCode, scanCode, ch, redraw);
        if (process) {
            CKeyboard* kbd = this->getKbd();
            bool processed = false;
            if (kbd) {
                if (virtualCode == VK_SPACE) {
                }
            }
        }
    }

    if (redraw) {
        this->redraw();
    }

    return true;
}
*/
