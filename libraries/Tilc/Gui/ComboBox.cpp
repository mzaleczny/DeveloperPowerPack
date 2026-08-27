#include "Tilc/Gui/ComboBox.h"
#include "Tilc/Gui/Listbox.h"
#include "Tilc/Gui/MultiColumnListbox.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/ScrollBar.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Theme.h"

int Tilc::Gui::DefaultComboDropDownClicked(float x, float y, Uint8 MouseButton, Tilc::Gui::TGuiControl* DropDown)
{
    Tilc::Gui::TListbox* DropDownList = reinterpret_cast<Tilc::Gui::TListbox*>(DropDown);
    Tilc::Gui::TComboBox* Combo = reinterpret_cast<Tilc::Gui::TComboBox*>(DropDown->GetParent());
    bool ApplyAndClose = true;

    if (DropDownList->m_VScrollBar && DropDownList->m_VScrollBar->IsVisible())
    {
        if (Tilc::Gui::TGuiControl::GetLastClickedControl() != DropDownList)
        {
            ApplyAndClose = false;
        }
    }

    if (ApplyAndClose)
    {
        Combo->SetText(DropDownList->GetSelectedItemValue());
        // Zwijamy
        Combo->m_DropDownVisible = false;
        Combo->m_RollDirection = 0;
        Combo->m_CurrentDropDownHeight = 0.0f;

        Combo->RemoveDropDownFromPrivilegedList();
    }
    return 0;
}


Tilc::Gui::TComboBox::TComboBox(Tilc::Gui::TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& text, const std::initializer_list<const char*>& Items, bool tabStop)
    : Tilc::Gui::TTextField(parent, name, position, Tilc::Gui::EControlType::ECT_ComboBox, text, tabStop)
{
    m_PaddingRight = 25.0f;

    m_ChevronRect = m_Position;
    m_ChevronRect.x = m_ChevronRect.w - m_PaddingRight;
    m_ChevronRect.y = 0;

    SetItems(Items);

    SetTickable(true);
}

Tilc::Gui::TComboBox::TComboBox(Tilc::Gui::TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& text, const std::vector<std::initializer_list<const char*>>& Items, bool tabStop)
    : Tilc::Gui::TTextField(parent, name, position, Tilc::Gui::EControlType::ECT_ComboBox, text, tabStop)
{
    m_PaddingRight = 25.0f;

    m_ChevronRect = m_Position;
    m_ChevronRect.x = m_ChevronRect.w - m_PaddingRight;
    m_ChevronRect.y = 0;

    SetItems(Items);

    SetTickable(true);
}

Tilc::Gui::TComboBox::~TComboBox()
{
}

void Tilc::Gui::TComboBox::SetItems(const std::initializer_list<const char*>& Items)
{
    if (!m_DropDownItems)
    {
        SDL_FRect Position = m_Position;
        Position.x = 0;
        Position.y = m_Position.h;
        Position.h = 0;
        m_DropDownItems = new TListbox(this, m_Name + "_DropdownList", Position);
    }
    if (m_DropDownItems)
    {
        m_DropDownItems->SetItems(Items);
        m_DropDownItems->OnClick = &Tilc::Gui::DefaultComboDropDownClicked;
    }
}

void Tilc::Gui::TComboBox::SetItems(const std::vector<std::initializer_list<const char*>>& Items, bool redraw)
{
    if (!m_DropDownItems)
    {
        SDL_FRect Position = m_Position;
        Position.x = 0;
        Position.y = m_Position.h;
        Position.h = 0;
        m_DropDownItems = new TMultiColumnListbox(this, m_Name + "_DropdownList", Position, {});
    }
    if (m_DropDownItems)
    {
        Tilc::Gui::TMultiColumnListbox* mlb = reinterpret_cast<Tilc::Gui::TMultiColumnListbox*>(m_DropDownItems);
        mlb->SetItems(Items);
        mlb->OnClick = &Tilc::Gui::DefaultComboDropDownClicked;
    }
}

void Tilc::Gui::TComboBox::Draw()
{
    if (!m_Visible) return;

    TTextField::Draw();

    m_NeedUpdate = ENeedUpdate::ENU_None;

    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    Tilc::Gui::TFont* Font = t->DefaultFont;
    SDL_Texture* OldRenderTarget{};
    float x = 0;
    float y = 0;
    SDL_Texture* TextureMap = t->GuiTextureMap1;
    SDL_FRect Rect = m_Position;

    if (m_Canvas)
    {
        OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);
    }

    RenderTexture(TextureMap, &t->combobox_chevron_down_rc, m_ChevronRect.x, m_ChevronRect.y);


    SDL_FRect bg = t->combobox_bg_rc;
    SDL_FRect frame_tl = t->combobox_frame_top_left_rc;
    SDL_FRect frame_t = t->combobox_frame_top_rc;
    SDL_FRect frame_tr = t->combobox_frame_top_right_rc;

    SDL_FRect frame_bl = t->combobox_frame_bottom_left_rc;
    SDL_FRect frame_b = t->combobox_frame_bottom_rc;
    SDL_FRect frame_br = t->combobox_frame_bottom_right_rc;

    SDL_FRect frame_l = t->combobox_frame_left_rc;
    SDL_FRect frame_r = t->combobox_frame_right_rc;
    // ================================================================
    // Draw DropDownList - jest rysowany z poziou listy uprzywilejowanych kontrolek: Tilc::Gui::TGuiControl::m_HighPrivilegedControls
    // ================================================================
    /*
    if (m_DropDownVisible && m_DropDownItems)
    {
        m_DropDownRect = m_RealPosition;
        m_DropDownRect.y += m_DropDownRect.h;
        m_DropDownRect.h = m_CurrentDropDownHeight;
        if (Tilc::Gui::TGuiControl::m_HighPrivilegedDrawCalls.find(this) == Tilc::Gui::TGuiControl::m_HighPrivilegedDrawCalls.end())
        {
            Tilc::Gui::TGuiControl::m_HighPrivilegedDrawCalls[this] =
                [this]() {
                    m_DropDownItems->Draw(nullptr, &m_DropDownRect);
                };
        }
    }
    */
    if (m_Canvas)
    {
        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
}

bool Tilc::Gui::TComboBox::Update(float DeltaTime)
{
    if (m_DropDownVisible && m_RollDirection != 0)
    {
        // tutaj rozwijamy
        if (m_RollDirection == 1 && m_CurrentDropDownHeight < m_DestinationDropDownHeight)
        {
            m_CurrentDropDownHeight += m_RollDirection * m_RollSpeed * DeltaTime;
            if (m_CurrentDropDownHeight > m_DestinationDropDownHeight)
            {
                // rozwinęliśmy, więc zablokuj dalsze rozwijanie
                m_CurrentDropDownHeight = m_DestinationDropDownHeight;
                m_RollDirection = 0;
                // Ustawiamy rozmiar scrollbarów
                m_DropDownItems->SetSize(m_RealPosition.w, m_CurrentDropDownHeight);
                // i ustawiamy ich widoczność
                m_DropDownItems->SetScrollBars();
            }
            else if (m_DropDownItems)
            {
                // Ustawiamy rozmiar scrollbarów
                m_DropDownItems->SetSize(m_RealPosition.w, m_CurrentDropDownHeight);
                // i ustawiamy ich widoczność
                m_DropDownItems->SetScrollBars();
            }
            Invalidate();
        }
        // tutaj zwijamy
        else if (m_RollDirection == -1 && m_CurrentDropDownHeight > 0.0f)
        {
            m_CurrentDropDownHeight += m_RollDirection * m_RollSpeed * DeltaTime;
            if (m_CurrentDropDownHeight < 0.0f)
            {
                // rozwinęliśmy, więc zablokuj dalsze zwijanie
                m_CurrentDropDownHeight = 0.0f;
                m_RollDirection = 0;
                m_DropDownVisible = false;
                RemoveDropDownFromPrivilegedList();
            }
            else if (m_DropDownItems)
            {
                // Ustawiamy rozmiar scrollbarów
                m_DropDownItems->SetSize(m_RealPosition.w, m_CurrentDropDownHeight);
                // i ustawiamy ich widoczność
                m_DropDownItems->SetScrollBars();
            }
            Invalidate();
        }
    }
    return false;
}

bool Tilc::Gui::TComboBox::OnMouseMove(const SDL_Event& event)
{
    if (!m_Visible) return false;
    if (OtherControlCapturedMouse())
    {
        return false;
    }

    TGuiControl::OnMouseMove(event);

    return true;
}

bool Tilc::Gui::TComboBox::OnMouseButtonDown(const SDL_Event& event)
{
    if (!m_Visible) return false;
    if (OtherControlCapturedMouse())
    {
        return false;
    }

    TGuiControl::OnMouseButtonDown(event);

    if (PointIn(event.button.x, event.button.y))
    {
        if (event.button.x >= m_RealPosition.x + m_ChevronRect.x)
        {
            if (m_RollDirection == -1)
            {
                // Rozwijamy
                m_DropDownVisible = true;
                m_RollDirection = 1;
                AddDropDownToPrivilegedList();
            }
            else if (m_RollDirection == 1)
            {
                // Zwijamy
                m_DropDownVisible = true;
                m_RollDirection = -1;
            }
            else
            {
                if (!m_DropDownVisible)
                {
                    // Rozwijamy
                    m_DropDownVisible = true;
                    m_RollDirection = 1;
                    AddDropDownToPrivilegedList();
                }
                else
                {
                    // Zwijamy
                    m_DropDownVisible = true;
                    m_RollDirection = -1;
                }
            }
        }
        CaptureMouse(this);
        Invalidate();
        return true;
    }

    return false;
}

void Tilc::Gui::TComboBox::AddDropDownToPrivilegedList()
{
    std::list<Tilc::Gui::TGuiControl*>& lst = Tilc::Gui::TGuiControl::m_HighPrivilegedControls;
    if (std::find(lst.begin(), lst.end(), m_DropDownItems) == lst.end())
    {
        lst.push_back(m_DropDownItems);
    }
}

void Tilc::Gui::TComboBox::RemoveDropDownFromPrivilegedList()
{
    std::list<Tilc::Gui::TGuiControl*>& lst = Tilc::Gui::TGuiControl::m_HighPrivilegedControlsToRemove;
    if (std::find(lst.begin(), lst.end(), m_DropDownItems) == lst.end())
    {
        lst.push_back(m_DropDownItems);
    }
}
