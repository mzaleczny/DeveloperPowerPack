#include "Tilc/Gui/GuiControl.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/ScrollBarVertical.h"
#include "Tilc/Gui/ScrollBarHorizontal.h"
#include "Tilc/Gui/Menu.h"
#include "Tilc/Gui/TextField.h"
#include "Tilc/Gui/Button.h"
#include "Tilc/Game.h"
#include "Tilc/Game2D/Sprite/DirectionalAnimation.h"
#include "Tilc/Utils/StdObject.h"
#include <algorithm>

SDL_Renderer* Tilc::Gui::TGuiControl::Renderer{};
Tilc::Gui::TGuiControl* Tilc::Gui::TGuiControl::m_ControlThatCapturedMouse{};

Tilc::Gui::TGuiControlItem::TGuiControlItem(const Tilc::TExtString& value, bool selected, bool checked, int imageIndex, long long data)
    : m_Value(value), m_Selected(selected), m_Checked(checked), m_ImageIndex(imageIndex), m_Data(data), m_Size(0)
{
}


Tilc::Gui::TGuiControl::TGuiControl(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, bool editable)
{
    m_Name = name;
    //SDL_Log("Added control: %s", m_Name.c_str());
    m_Position = position;
    m_Parent = parent;
    CommonInit(editable);
    if (m_Parent)
    {
        // scrollbare dodajemy na poczatku, gdyz muszą miec priorytet podczas przetwarzania zdarzen
        if (dynamic_cast<TScrollBar*>(this))
        {
            m_Parent->m_Children.push_front(this);
        }
        else
        {
            m_Parent->m_Children.push_back(this);
        }
    }
}

void Tilc::Gui::TGuiControl::CommonInit(bool editable)
{
    m_State = 0;
    m_DestroyCanvas = false;
    m_Rewind = false;
    m_Visible = true;
    m_Enabled = true;
    m_TabStop = false;
    m_DisableTabkey = false;
    m_DoEnterAsTabkey = false;
    m_Autosize = false;
    m_NeedUpdate = ENeedUpdate::ENU_Everything;
    // zielony
    m_BackgroundColor = SDL_Color{ 0, 255, 0 };

    m_HScrollBar = nullptr;
    m_VScrollBar = nullptr;
    m_Editor = nullptr;
    
    m_LeftMouseButtonPressed = false;
    m_Editable = false;
    m_IsEditor = false;
    if (editable)
    {
        AddEditor();
        if (m_Editor)
        {
            m_Editable = true;
        }
    }
}

void Tilc::Gui::TGuiControl::DestroyCanvasIfNeedDestroy()
{
    if (m_DestroyCanvas && m_Canvas)
    {
        SDL_DestroyTexture(m_Canvas);
        m_Canvas = nullptr;
    }
}

Tilc::Gui::TGuiControl::~TGuiControl()
{
    DestroyChildren();
    DestroyCanvasIfNeedDestroy();
//    CStyledWindow* wnd = this->getParentWindow();
//    wnd->unregisterSprite(this);
}

void Tilc::Gui::TGuiControl::DestroyChildren()
{
    if (m_Children.empty())
    {
        return;
    }

    for (auto it = m_Children.begin(); it != m_Children.end(); ++it)
    {
        delete* it;
    }
    m_Children.clear();
}

void Tilc::Gui::TGuiControl::Play(bool forward)
{
    m_Animation->Play();
}

bool Tilc::Gui::TGuiControl::IsPlaying() const
{
    return m_Animation && m_Animation->IsPlaying();
}

void Tilc::Gui::TGuiControl::Stop()
{
    m_Animation->Pause();
}

void Tilc::Gui::TGuiControl::Loop()
{
    m_Animation->SetLooping(true);
}

bool Tilc::Gui::TGuiControl::IsLooping() const
{
    return m_Animation && m_Animation->IsLooping();
}

void Tilc::Gui::TGuiControl::SetCanvas(SDL_Texture* canvas)
{
    if (!canvas)
    {
        return;
    }

    if (m_DestroyCanvas && m_Canvas)
    {
        SDL_DestroyTexture(m_Canvas);
    }

    m_Canvas = canvas;
    m_Position.w = canvas->w;
    m_Position.h = canvas->h;
    m_NeedUpdate = ENeedUpdate::ENU_Everything;
}

void Tilc::Gui::TGuiControl::Hide()
{
    if (m_Visible)
    {
        m_Visible = false;
        if (m_Parent)
        {
            m_Parent->m_NeedUpdate = ENeedUpdate::ENU_Everything;
        }
    }
}

bool Tilc::Gui::TGuiControl::PointIn(float x, float y)
{
    SDL_FRect Position{ GetRealPosition() };
    if (
        x >= Position.x && x < Position.x + Position.w &&
        y >= Position.y && y < Position.y + Position.h
    )
    {
        return true;
    }
    return false;
}

bool Tilc::Gui::TGuiControl::Update(float DeltaTime)
{
    if (IsPlaying())
    {
        m_Animation->Update(DeltaTime);
    }

    bool result = false;
    for (auto it = m_Children.begin(); it != m_Children.end(); ++it)
    {
        if ((*it)->Update(DeltaTime))
        {
            (*it)->Invalidate();
            result = true;
        }
    }

    return result;
}

bool Tilc::Gui::TGuiControl::OnMouseMove(const SDL_Event& event)
{
    if (!m_Visible) return false;

    /*
    if (m_ControlThatCapturedMouse)
    {
        //SDL_Log("%p: %s, Captive: %p %s", this, m_Name.c_str(), m_ControlThatCapturedMouse, m_ControlThatCapturedMouse->m_Name.c_str());
    }
    else
    {
        //SDL_Log("%p: %s, Captive: %p", this, m_Name.c_str(), m_ControlThatCapturedMouse);
    }
    */
    if (m_ControlThatCapturedMouse && m_ControlThatCapturedMouse != this)
    {
        return m_ControlThatCapturedMouse->OnMouseMove(event);
    }

    TStyledWindow* wnd = GetParentWindow();
    if (PointIn(event.motion.x, event.motion.y))
    {
        if (!m_LeftMouseButtonPressed)
        {
            if (wnd && wnd->GetActiveControl() == this)
            {
                SetState(CONTROL_STATE_HOVER | CONTROL_STATE_FOCUSED | CONTROL_STATE_ACTIVE);
            }
            else
            {
                SetState(CONTROL_STATE_HOVER);
           }
        }
        else
        {
            if (wnd && wnd->GetActiveControl() == this)
            {
                SetState(CONTROL_STATE_PUSHED | CONTROL_STATE_FOCUSED | CONTROL_STATE_ACTIVE);
            }
            else
            {
                SetState(CONTROL_STATE_PUSHED);
            }
        }

        return true;
    }

    if ((m_State & CONTROL_STATE_HOVER) || (m_State & CONTROL_STATE_PUSHED))
    {
        if (wnd && wnd->GetActiveControl() == this)
        {
            SetState(CONTROL_STATE_NORMAL | CONTROL_STATE_FOCUSED | CONTROL_STATE_ACTIVE);
        }
        else
        {
            SetState(CONTROL_STATE_NORMAL);
        }
    }

    return false;
}


bool Tilc::Gui::TGuiControl::OnMouseButtonDown(const SDL_Event& event)
{
    if (!m_Visible || OtherControlCapturedMouse()) return false;

    if (PointIn(event.motion.x, event.motion.y))
    {
        m_LeftMouseButtonPressed = true;
        m_Dragging = true;
        //SDL_Log("m_Dragging set to True");
        m_DragStartX = event.motion.x;
        m_DragStartY = event.motion.y;

        Tilc::Gui::TStyledWindow* wnd = GetParentWindow();
        wnd->CaptureMouse(this);

        if (wnd->GetActiveControl() != this)
        {
            wnd->SetActiveControl(this);
            SetState(CONTROL_STATE_PUSHED | CONTROL_STATE_FOCUSED | CONTROL_STATE_ACTIVE);
        }
        else
        {
            SetState(CONTROL_STATE_PUSHED | CONTROL_STATE_FOCUSED);
        }
        return true;
    }

    return false;
}

bool Tilc::Gui::TGuiControl::OnMouseButtonUp(const SDL_Event& event)
{
    if (!m_Visible) return false;
    Tilc::Gui::TGuiControl* ControlThatCapturedMouse = GetParentWindow()->GetControlThatCapturedMouse();
    if (ControlThatCapturedMouse && ControlThatCapturedMouse != this)
    {
        return false;
    }

    bool MouseWasPressed = m_LeftMouseButtonPressed;
    m_LeftMouseButtonPressed = false;
    m_Dragging = false;
    //SDL_Log("m_Dragging set to False");
    Tilc::Gui::TStyledWindow* wnd = GetParentWindow();
    wnd->CaptureMouse(nullptr);

    if (PointIn(event.motion.x, event.motion.y))
    {
        if (wnd->GetActiveControl() == this)
        {
            SetState(CONTROL_STATE_HOVER | CONTROL_STATE_FOCUSED | CONTROL_STATE_ACTIVE);
        }
        else
        {
            SetState(CONTROL_STATE_HOVER | CONTROL_STATE_FOCUSED);
        }

        if (MouseWasPressed && OnClick)
        {
            OnClick(event.motion.x, event.motion.y, event.button.button);
        }
        return true;
    }

    if (wnd->GetActiveControl() == this)
    {
        SetState(CONTROL_STATE_NORMAL | CONTROL_STATE_FOCUSED | CONTROL_STATE_ACTIVE);
    }
    else
    {
        SetState(CONTROL_STATE_NORMAL);
    }

    return false;
}

Tilc::Gui::TStyledWindow* Tilc::Gui::TGuiControl::GetParentWindow()
{
    if (!m_ParentWindow)
    {
        Tilc::Gui::TGuiControl* parent = m_Parent;
        Tilc::Gui::TGuiControl* gc;
        while (parent)
        {
            gc = parent;
            if (gc->m_Parent && gc->m_Parent->m_Parent == nullptr)
            {
                m_ParentWindow = dynamic_cast<Tilc::Gui::TStyledWindow*>(gc->m_Parent);
                return m_ParentWindow;
            }
            if (!parent->m_Parent) break;
            parent = parent->m_Parent;
        }
        m_ParentWindow = dynamic_cast<Tilc::Gui::TStyledWindow*>(parent);
    }
	return m_ParentWindow;
}

Tilc::Gui::TGuiControl* Tilc::Gui::TGuiControl::GetChild(const Tilc::TExtString& name)
{
    if (m_Children.empty()) return nullptr;
    // szukamy sprite'a o podanej nazwie w liście wszystkich dzieci tej kontrolki
    auto Found = std::find_if(m_Children.begin(), m_Children.end(), [&name](const auto v) { return v->m_Name == name; });
    if (Found != m_Children.end()) return *Found;

    TGuiControl* Control{ nullptr };
    for (auto it = m_Children.begin(); it != m_Children.end(); ++it)
    {
        if (!(*it)->m_Children.empty())
        {
            Control = (*it)->GetChild(name);
            if (Control) return Control;
        }
    }
    return nullptr;
}

void Tilc::Gui::TGuiControl::GetCurrentMousePosition(float* x, float* y)
{
    float mx, my;
    SDL_GetMouseState(&mx, &my);
    GetPositionInWindow(x, y);
    *x = mx - (*x);
    *y = my - (*y);
}

void Tilc::Gui::TGuiControl::GetPositionInWindow(float* x, float* y)
{
    *x = m_Position.x;
    *y = m_Position.y;

    TGuiControl* p = m_Parent;
    while (p)
    {
        *x += p->m_Position.x;
        *y += p->m_Position.y;

        p = p->m_Parent;
    }
}

void Tilc::Gui::TGuiControl::RecreateCanvasForCurrentSize()
{
    if (m_Canvas)
    {
        SDL_DestroyTexture(m_Canvas);
        m_Canvas = SDL_CreateTexture(GetRenderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, m_Position.w, m_Position.h);
        m_DestroyCanvas = true;
    }
}

bool Tilc::Gui::TGuiControl::OtherControlCapturedMouse()
{
    Tilc::Gui::TStyledWindow* ParentWindow = GetParentWindow();
    if (ParentWindow)
    {
        Tilc::Gui::TGuiControl* ControlThatCapturedMouse = ParentWindow->GetControlThatCapturedMouse();
        if (ControlThatCapturedMouse && ControlThatCapturedMouse != this)
        {
            return true;
        }
    }
    return false;
}

void Tilc::Gui::TGuiControl::ResetToDefaultState()
{
    ResetControl();
    size_t OrigSize = m_Children.size();
    std::for_each(m_Children.begin(), m_Children.end(), [](Tilc::Gui::TGuiControl* c) { c->ResetControl(); });
}

void Tilc::Gui::TGuiControl::SetState(int state, bool redraw)
{
    if (m_State != state)
    {
        m_State = state;
        if (redraw)
        {
            Invalidate();
        }
    }
}

void Tilc::Gui::TGuiControl::SetDetailedState(int state, bool redraw)
{
    if (m_DetailedState != state)
    {
        m_DetailedState = state;
        if (redraw)
        {
            Invalidate();
        }
    }
}

void Tilc::Gui::TGuiControl::AddState(int state, bool redraw)
{
    int old_state = m_State;
    m_State |= state;
    if (redraw && old_state != m_State)
    {
        m_NeedUpdate = ENeedUpdate::ENU_Everything;
        if (GetParentWindow())
        {
            GetParentWindow()->Invalidate();
        }
    }
}

void Tilc::Gui::TGuiControl::RemoveState(int state, bool redraw)
{
    int old_state = m_State;
    m_State &= ~state;
    if (redraw && old_state != m_State)
    {
        m_NeedUpdate = ENeedUpdate::ENU_Everything;
    }
}

bool Tilc::Gui::TGuiControl::SetTextForSprite(Tilc::TExtString value, Tilc::TExtString spriteName, Tilc::TExtString wndName, bool redraw)
{
    Tilc::Gui::TGuiControl* wnd{ GetParentWindow() };

    if (!wndName.empty())
    {
        Tilc::Gui::TGuiControl* Control = wnd->GetChild(wndName);
        if (Control)
        {
            wnd = Control;
        }
    }

    // jeśli nie znaleziono okna, to zwracamy FALSE
    if (!wnd)
    {
        return false;
    }

    Tilc::Gui::TGuiControl* Control = wnd->GetChild(spriteName);
    if (!Control)
    {
        return false;
    }

    Control->SetText(value, redraw);

    return true;
}

void Tilc::Gui::TGuiControl::AddScrollbars(bool vertical, bool horizontal, int vmin, int vmax, int hmin, int hmax)
{
    int size;
    
    if (vertical)
    {
        size = 0;
        if (!horizontal)
        {
            size = m_Position.h;
        }
        AddVerticalScrollbar(vmin, vmax, size);
    }
    
    if (horizontal)
    {
        size = 0;
        if (!vertical)
        {
            size = m_Position.w;
        }
        AddHorizontalScrollbar(hmin, hmax, size);
    }
}

void Tilc::Gui::TGuiControl::AddVerticalScrollbar(int min, int max, int size)
{
    if (Tilc::GameObject)
    {
        Tilc::Gui::TTheme* theme = Tilc::GameObject->GetContext()->m_Theme;
        int vscr_w = theme->scrollbar_vertical_arrow_up_rc.w;
        int hscr_h = theme->scrollbar_horizontal_arrow_right_rc.h;
        float top = 0;
        float DesiredHeight = m_Position.h;
        if (const Tilc::Gui::TStyledWindow* wnd = dynamic_cast<Tilc::Gui::TStyledWindow*>(this))
        {
            top = theme->wnd_caption_inactive_middle_rc.h;
            DesiredHeight -= top + theme->wnd_frame_bottom_rc.h;
        }
        SDL_FRect ChildPosition{ static_cast<float>(m_Position.w - vscr_w), top, static_cast<float>(vscr_w), DesiredHeight };
        Tilc::Gui::TScrollBar* scrbar = new Tilc::Gui::TScrollBarVertical(this, m_Name + "_VSB", ChildPosition, min, max, min, true);
        if (scrbar)
        {
            if (!size)
            {
                size = m_Position.h - hscr_h;
            }
            scrbar->SetSizeRelativeToParent(m_Position.w, size);
            scrbar->RegisterPositionNotificationForControl(this);
            m_VScrollBar = scrbar;
        }
    }
}

bool Tilc::Gui::TGuiControl::RemoveVerticalScrollbar()
{
    if (m_VScrollBar)
    {
        return RemoveChild(m_VScrollBar) > 0;
    }
    return false;
}

void Tilc::Gui::TGuiControl::AddHorizontalScrollbar(int min, int max, int size)
{
    if (Tilc::GameObject)
    {
        Tilc::Gui::TTheme* theme = Tilc::GameObject->GetContext()->m_Theme;
        int vscr_w = theme->scrollbar_vertical_arrow_up_rc.w;
        int hscr_h = theme->scrollbar_horizontal_arrow_right_rc.h;
        float left = 0;
        float DesiredWidth = m_Position.w;
        if (const Tilc::Gui::TStyledWindow* wnd = dynamic_cast<Tilc::Gui::TStyledWindow*>(this))
        {
            left = theme->wnd_frame_left_rc.w;
            DesiredWidth -= theme->wnd_frame_left_rc.w + theme->wnd_frame_right_rc.w;
        }
        SDL_FRect ChildPosition{ left, m_Position.h - hscr_h, DesiredWidth, static_cast<float>(hscr_h) };
        Tilc::Gui::TScrollBar* scrbar = new Tilc::Gui::TScrollBarHorizontal(this, m_Name + "_HSB", ChildPosition, min, max, min, true);
        if (scrbar)
        {
            if (!size)
            {
                size = m_Position.w - vscr_w;
            }
            scrbar->SetSizeRelativeToParent(size, m_Position.h);
            scrbar->RegisterPositionNotificationForControl(this);
            m_HScrollBar = scrbar;
        }
    }
}

bool Tilc::Gui::TGuiControl::RemoveHorizontalScrollbar()
{
    if (m_HScrollBar)
    {
        return RemoveChild(m_HScrollBar) > 0;
    }
    return false;
}

void Tilc::Gui::TGuiControl::AddEditor()
{
    Tilc::Gui::TTextField* Editor = new Tilc::Gui::TTextField(this, m_Name + "_EDITOR", { 0, 0, 200, 25 });
    if (Editor)
    {
        //editor->setSizeRelativeToParent(this->width - vscr_w, this->height);
        //scrbar->registerPositionNotificationForSprite(this);
        Editor->m_IsEditor = true;
        Editor->DisableTabkey();
        Editor->Hide();
        m_Editor = Editor;
        AddChild(Editor);
        m_Editable = true;
    }
}

bool Tilc::Gui::TGuiControl::RemoveEditor()
{
    if (m_Editor)
    {
        return RemoveChild(m_Editor);
    }
    return false;
}

void Tilc::Gui::TGuiControl::SetEditable(bool value)
{
    m_Editable = true;
    if (value && m_Editor)
    {
        return;
    }
    if (!value)
    {
        RemoveEditor();
        m_Editable = false;
    }
    AddEditor();
}

void Tilc::Gui::TGuiControl::ResetEditor()
{
    if (!m_Editable || !m_Editor)
    {
        return;
    }
    Tilc::Gui::TTextField* tf = dynamic_cast<Tilc::Gui::TTextField*>(m_Editor);
    if (tf)
    {
        tf->SetText("");
    }
}

void Tilc::Gui::TGuiControl::SetEditorValue(Tilc::TStdObject* value)
{
    if (m_Editor)
    {
        Tilc::Gui::TTextField* tf = dynamic_cast<Tilc::Gui::TTextField*>(m_Editor);
        if (tf)
        {
            Tilc::TExtString v = value->getAsString("text");
            m_Editor->SetText(v);
        }
    }
}

void Tilc::Gui::TGuiControl::Edit()
{
    if (!m_Editable || !m_Editor)
    {
        return;
    }
    ResetEditor();
    Tilc::TStdObject value = GetValue();
    SetEditorValue(&value);
    OnEditorShow();
    m_Editor->Show();

    TStyledWindow* wnd = GetParentWindow();
    if (wnd)
    {
        wnd->SetActiveControl(m_Editor);
    }
    m_Editor->Invalidate(); // forces this control to redraw also
}

void Tilc::Gui::TGuiControl::EndEdit(bool acceptChanges)
{
    if (!m_Editable || !m_Editor)
    {
        return;
    }

    TStyledWindow* ParentWnd = GetParentWindow();
    if (!ParentWnd || (ParentWnd->GetActiveControl() != m_Editor))
    {
        return;
    }

    if (acceptChanges)
    {
        Tilc::Gui::TTextField* tf = dynamic_cast<Tilc::Gui::TTextField*>(m_Editor);
        if (tf)
        {
            Tilc::TStdObject value;
            value.set("text", tf->GetText());
            OnApplyEditorChanges(&value);
        }
    }
    
    m_Editor->Hide();
    ParentWnd->SetActiveControl(this);
    Invalidate();
}


size_t Tilc::Gui::TGuiControl::RemoveChild(Tilc::Gui::TGuiControl* child)
{
    size_t OrigSize = m_Children.size();
    m_Children.erase(std::remove(m_Children.begin(), m_Children.end(), child), m_Children.end());
    if (reinterpret_cast<void*>(m_VScrollBar) == reinterpret_cast<void*>(child))
    {
        m_VScrollBar = nullptr;
    }
    else if (reinterpret_cast<void*>(m_HScrollBar) == reinterpret_cast<void*>(child))
    {
        m_HScrollBar = nullptr;
    }
    else if (reinterpret_cast<void*>(m_Editor) == reinterpret_cast<void*>(child))
    {
        m_Editor = nullptr;
    }
    return OrigSize - m_Children.size();
}

void Tilc::Gui::TGuiControl::DrawChildren()
{
    // ================================================================
    // Rysowanie dzieci
    // ================================================================
    for (auto it = m_Children.begin(); it != m_Children.end(); ++it)
    {
        (*it)->Draw();
    }
    // ================================================================
    // Koniec rysowania dzieci
    // ================================================================
}

void Tilc::Gui::TGuiControl::InvalidateAllChildren()
{
    // ================================================================
    // updating canvases of child-sprites
    // ================================================================
    for (auto it = m_Children.begin(); it != m_Children.end(); ++it)
    {
        if ((*it)->IsVisible())
        {
            (*it)->Invalidate();
            if (!(*it)->m_Children.empty())
            {
                (*it)->InvalidateAllChildren();
            }
        }
    }
    // ================================================================
    // end of updating canvases of child-sprites
    // ================================================================
}

bool Tilc::Gui::TGuiControl::DoChildEvent(const SDL_Event& event)
{
    Tilc::Gui::TMenu* Menu{};
    bool result = false;
    SDL_FPoint pt = SDL_FPoint(event.motion.x, event.motion.y);
    SDL_FRect Rect;
    // Resetujemy tą kontrolkę i wszystkie jej dzieci do stanu domyślnego, żeby np. po zjechaniu myszki z nich starciły stan HOVER. Bez tego nie zawsze tak się działo
    if (event.type == SDL_EVENT_MOUSE_MOTION)
    {
        ResetToDefaultState();
    }
    // Po puszczeniu przycisku resetujemy stan suwaków, żeby przestały odbierać stan przeciąganie jeśli taki miały ustwiony
    else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        if (m_HScrollBar)
        {
            m_HScrollBar->SetDetailedState(0);
            m_HScrollBar->ResetControl();
        }
        if (m_VScrollBar)
        {
            m_VScrollBar->SetDetailedState(0);
            m_VScrollBar->ResetControl();
        }
    }

    // Jeśli ta kontrolka ma scrollbary i myszka jest nad którymś z nich, to zdarzenie kierujemy do scrollbarów a nie do kontrolek, które mogą być pod nimi. Po to żeby
    // scrollbary zawsze dziaały poprawnie
    if (m_HScrollBar)
    {
        Rect = m_HScrollBar->GetRealPosition();
        if (SDL_PointInRectFloat(&pt, &Rect))
        {
            return m_HScrollBar->DoChildEvent(event);
        }
    }
    if (m_VScrollBar)
    {
        Rect = m_VScrollBar->GetRealPosition();
        if (SDL_PointInRectFloat(&pt, &Rect))
        {
            return m_VScrollBar->DoChildEvent(event);
        }
        else
        {
            static long long c{};
            SDL_Log("NOT In RECT %ld: pt=(%5.2f, %5.2f), Rect=(%5.2f, %5.2f, %5.2f, %5.2f)", ++c, pt.x, pt.y, Rect.x, Rect.y, Rect.w, Rect.h);
        }
    }

    // First we traverse childs list, to handle ecvent by innermost child first and then pop upwards
    for (auto it = m_Children.begin(); it != m_Children.end(); ++it)
    {
        SDL_FRect Position{ (*it)->GetRealPosition() };
        if (SDL_PointInRectFloat(&pt, &Position))
        {
            result = (*it)->DoChildEvent(event);
            if (result)
            {
                // make sure to uncapture window on mouse up
                if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
                {
                    (*it)->CaptureMouse(nullptr);
                }
                return true;
            }
        }
    }

    switch (event.type)
    {
    case SDL_EVENT_KEY_DOWN:
        if (OnKeyDown(event)) return true;
        break;
    case SDL_EVENT_KEY_UP:
        if (OnKeyUp(event)) return true;
        break;
    case SDL_EVENT_MOUSE_MOTION:
        if (OnMouseMove(event)) return true;
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (OnMouseButtonDown(event)) return true;
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        // make sure to uncapture window on mouse up
        if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
        {
            CaptureMouse(nullptr);
        }
        if (OnMouseButtonUp(event)) return true;
        break;
    }
    
    return false;
}

void Tilc::Gui::TGuiControl::Invalidate(ENeedUpdate WhatNeedUpdate)
{
    m_NeedUpdate = WhatNeedUpdate;
    // if there is parent window then invalidate it also
    if (GetParentWindow()) GetParentWindow()->Invalidate();
}


bool Tilc::Gui::TGuiControl::OnEnterPressed()
{
    if (m_IsEditor && m_Parent)
    {
        m_Parent->OnChildNotification(this, SDL_SCANCODE_RETURN);
        return true;
    }
    
    if (m_Editable)
    {
        if (m_Editor && !m_Editor->IsVisible())
        {
            Edit();
            return true;
        }
    }
    return false;
}

bool Tilc::Gui::TGuiControl::OnEscPressed()
{
    if (m_IsEditor && m_Parent)
    {
        m_Parent->OnChildNotification(this, SDL_SCANCODE_ESCAPE);
        return true;
    }
    
    return false;
}

int Tilc::Gui::TGuiControl::OnChildNotification(Tilc::Gui::TGuiControl* child, uintptr_t data)
{
    if (m_Editor && child == m_Editor && m_Editable && m_Editor->IsVisible())
    {
        if (data == SDL_SCANCODE_RETURN || data == SDL_SCANCODE_ESCAPE)
        {
            EndEdit(data == SDL_SCANCODE_RETURN);
        }
    }
    
    return 1;
}

void Tilc::Gui::TGuiControl::Draw(float x, float y,
    SDL_FRect* frame_top_left_rc, SDL_FRect* frame_top_rc, SDL_FRect* frame_top_right_rc,
    SDL_FRect* frame_bottom_left_rc, SDL_FRect* frame_bottom_rc, SDL_FRect* frame_bottom_right_rc,
    SDL_FRect* frame_left_rc, SDL_FRect* frame_right_rc, SDL_FRect* bg_rc)
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    TWindow* w = Tilc::GameObject->GetContext()->m_Window;
    SDL_Texture* TextureMap = t->GuiTextureMap1;

    float curX = x;
    float curY = y;
    float innerX = x;
    float innerY = y;
    float innerW{}, innerH{};
    SDL_FRect rc, DestRect;

    // Top left corner
    if (frame_top_left_rc->w > 0)
    {
        RenderTexture(TextureMap, frame_top_left_rc, curX, curY);
        curX += frame_top_left_rc->w;
        innerX = curX;
    }

    // Top frame
    if (frame_top_left_rc->w > 0)
    {
        innerW = m_Position.w - curX;
        if (frame_top_right_rc->w > 0)
        {
            innerW -= frame_top_right_rc->w;
        }
        RenderTexture(TextureMap, frame_top_rc, curX, curY, innerW, frame_top_rc->h);
        curX += innerW;
    }

    // Top right corner
    if (frame_top_right_rc->w > 0)
    {
        RenderTexture(TextureMap, frame_top_right_rc, curX, curY);
        curX = x;
        curY += frame_top_right_rc->h;
        innerY = curY;
    }

    // Left frame
    if (frame_left_rc->w > 0)
    {
        innerH = m_Position.h - curY;
        if (frame_bottom_left_rc->w > 0)
        {
            innerH -= frame_bottom_left_rc->h;
        }
        RenderTexture(TextureMap, frame_left_rc, curX, curY, frame_left_rc->w, innerH);
    }

    // Right frame
    if (frame_right_rc->w > 0)
    {
        curX = innerX + innerW;
        curY = innerY;
        RenderTexture(TextureMap, frame_right_rc, curX, curY, frame_right_rc->w, innerH);
    }
    
    // Bottom left corner
    if (frame_bottom_left_rc->w > 0)
    {
        curX = x;
        curY = innerY + innerH;
        RenderTexture(TextureMap, frame_bottom_left_rc, curX, curY);
    }
    
    // Bottom frame
    if (frame_bottom_rc->w > 0)
    {
        curX = innerX;
        curY = innerY + innerH;
        RenderTexture(TextureMap, frame_bottom_rc, curX, curY, innerW, frame_bottom_rc->h);
    }
    
    // Bottom right corner
    if (frame_bottom_right_rc->w > 0)
    {
        curX = innerX + innerW;
        curY = innerY + innerH;
        RenderTexture(TextureMap, frame_bottom_right_rc, curX, curY);
    }
    
    // Background
    if (bg_rc->w > 0)
    {
        RenderTexture(TextureMap, bg_rc, innerX, innerY, innerW, innerH);
    }
}

void Tilc::Gui::TGuiControl::SetOffsetX(float Offset)
{
    // jesli nie jest to suwak okna głównego (kontrolki, które nie ma rodzica), to aktualizujemy offset tej kontrolki uwzględniając offset rodzica
    if (dynamic_cast<Tilc::Gui::TScrollBar*>(this) && m_Parent && m_Parent->m_Parent == nullptr)
    {
        return;
    }

    if (m_Parent)
    {
        m_OffsetX = m_Parent->m_OffsetX + Offset;
    }
    else
    {
        m_OffsetX = Offset;
    }
    m_NeedUpdate = ENeedUpdate::ENU_Everything;

    for (auto it = m_Children.begin(); it != m_Children.end(); ++it)
    {
        // wywolanie ponizej ustawi przesuniecie wszystkich kontrolek dzieci taki sam jak kontrolki rodzica
        (*it)->SetOffsetX(0);
    }
}

void Tilc::Gui::TGuiControl::SetOffsetY(float Offset)
{
    // jesli nie jest to suwak okna głównego (kontrolki, które nie ma rodzica), to aktualizujemy offset tej kontrolki uwzględniając offset rodzica
    if (dynamic_cast<Tilc::Gui::TScrollBar*>(this) && m_Parent && m_Parent->m_Parent == nullptr)
    {
        return;
    }

    if (m_Parent)
    {
        m_OffsetY = m_Parent->m_OffsetY + Offset;
    }
    // lub tylko przesunięcie
    else
    {
        m_OffsetY = Offset;
    }
    m_NeedUpdate = ENeedUpdate::ENU_Everything;

    for (auto it = m_Children.begin(); it != m_Children.end(); ++it)
    {
        // wywolanie ponizej ustawi przesuniecie wszystkich kontrolek dzieci taki sam jak kontrolki rodzica
        (*it)->SetOffsetY(0);
    }
}

SDL_FRect Tilc::Gui::TGuiControl::GetRealPosition()
{
    SDL_FRect Position = m_Position;
    Position.x -= m_OffsetX;
    Position.y -= m_OffsetY;
    return Position;
}
