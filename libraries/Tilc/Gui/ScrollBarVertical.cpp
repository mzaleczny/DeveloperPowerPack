#include "Tilc/Gui/ScrollBarVertical.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/StyledWindow.h"

Tilc::Gui::TScrollBarVertical::TScrollBarVertical(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, int thumbPosition, int maxValue, int minValue, bool autosize)
    : Tilc::Gui::TScrollBar(parent, name, position, thumbPosition, maxValue, minValue, autosize)
{
    // w konstruktorze ustawiamy minimalną wielkość rozmiaru suwaka
    SetThumbSize();
}

void Tilc::Gui::TScrollBarVertical::SetSize(float width, float height)
{
    if (!m_Autosize)
    {
        Tilc::Gui::TGuiControl::SetSize(Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_thumb_middle_rc.w, height);
        SetThumbSize();
    }
}

void Tilc::Gui::TScrollBarVertical::SetSizeRelativeToParent(float parentWidth, float parentHeight)
{
    if (m_Autosize)
    {
        float DesiredHeight = parentHeight;
        if (const Tilc::Gui::TStyledWindow* wnd = dynamic_cast<Tilc::Gui::TStyledWindow*>(m_Parent))
        {
            DesiredHeight -= Tilc::GameObject->GetContext()->m_Theme->wnd_caption_middle_rc.h;
        }
        Tilc::Gui::TGuiControl::SetSize(Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_arrow_up_rc.w, DesiredHeight);
        SetThumbSize();
    }
}

void Tilc::Gui::TScrollBarVertical::SetThumbSize()
{
    // ustawiamy wielkość suwaka
    int delta = m_MaxValue - m_MinValue;
    int thumb_size = m_Position.h / delta;
    if (thumb_size < m_Position.h / 5)
    {
        thumb_size = m_Position.h / 5;
    }
    if (thumb_size > m_Position.h - Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_arrow_up_rc.h - Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_arrow_down_rc.h)
    {
        thumb_size = m_Position.h - Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_arrow_up_rc.h - Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_arrow_down_rc.h;
    }
    if (thumb_size < Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_thumb_top_rc.h + Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_thumb_middle_rc.h + Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_thumb_bottom_rc.h)
    {
        thumb_size = Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_thumb_top_rc.h + Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_thumb_middle_rc.h + Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_thumb_bottom_rc.h;
    }

    m_ThumbSize = thumb_size;
}

void Tilc::Gui::TScrollBarVertical::CalculateThumbLeftTopCorner(float& x, float& y, int position)
{
    SDL_FRect Position = GetRealPosition();
    if (position == INT_MIN)
    {
        position = m_ThumbPosition;
    }

    float middle_height = Position.h - Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_arrow_up_rc.h - Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_arrow_down_rc.h;
    // musimy obliczyć położenie suwaka
    float y_thumb = Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_arrow_up_rc.h + (static_cast<float>(position - m_MinValue) / static_cast<float>(m_MaxValue - m_MinValue)) * (middle_height - m_ThumbSize);

    x = 0;
    y = y_thumb;
}

int Tilc::Gui::TScrollBarVertical::CalculatePositionForThumbCoords(float x, float y)
{
    SDL_FRect Position = GetRealPosition();
    if (y == INT_MIN)
    {
        CalculateThumbLeftTopCorner(x, y);
    }

    float middle_height = Position.h - Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_arrow_up_rc.h - Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_arrow_down_rc.h;
    y -= Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_arrow_down_rc.h;

    float scale = (float)y / (middle_height - m_ThumbSize);
    int position = m_MinValue + static_cast<int>(scale * (m_MaxValue - m_MinValue));
    if (position < m_MinValue)
    {
        position = m_MinValue;
    }
    if (position > m_MaxValue)
    {
        position = m_MaxValue;
    }

    return position;
}

void Tilc::Gui::TScrollBarVertical::Draw()
{
    m_NeedUpdate = ENeedUpdate::ENU_None;

    TTheme* m_Theme = Tilc::GameObject->GetContext()->m_Theme;
    SDL_Texture* OldRenderTarget{};
	float x = 0;
    float y = 0;
    SDL_Texture* TextureMap = m_Theme->GuiTextureMap1;
    SDL_FRect rcAbove = m_Theme->scrollbar_vertical_bg_rc;
    SDL_FRect rcBottom = m_Theme->scrollbar_vertical_bg_rc;
    if (m_DetailedState == CSCROLLBAR_STATE_LARGE_SCROLLING_UP)
    {
        rcAbove = m_Theme->scrollbar_vertical_bg_pushed_rc;
        rcBottom = m_Theme->scrollbar_vertical_bg_rc;
    }
    if (m_DetailedState == CSCROLLBAR_STATE_LARGE_SCROLLING_DOWN)
    {
        rcAbove = m_Theme->scrollbar_vertical_bg_rc;
        rcBottom = m_Theme->scrollbar_vertical_bg_pushed_rc;
    }

    if (m_Canvas)
    {
        OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);
    }
    // ================================================================
    // Rysujemy górną strzałkę scrollbara
    // ================================================================
    SDL_FRect scrollbar_vertical_arrow_up_rc = m_Theme->scrollbar_vertical_arrow_up_rc;
    if (m_DetailedState == CSCROLLBAR_STATE_SMALL_SCROLLING_UP)
    {
        scrollbar_vertical_arrow_up_rc = m_Theme->scrollbar_vertical_arrow_up_pushed_rc;
    }
    else if (m_DetailedState == CSCROLLBAR_STATE_HOVER_UP_BUTTON)
    {
        scrollbar_vertical_arrow_up_rc = m_Theme->scrollbar_vertical_arrow_up_hover_rc;
    }
    RenderTexture(TextureMap, &scrollbar_vertical_arrow_up_rc, x, y);
    y += scrollbar_vertical_arrow_up_rc.h;

    // ================================================================
    // Koniec rysowania górnej strzałki scrollbara
    // ================================================================


    float middle_height = m_Position.h - m_Theme->scrollbar_vertical_arrow_up_rc.h - m_Theme->scrollbar_vertical_arrow_down_rc.h;
    // musimy obliczyć położenie suwaka
    float y_thumb = y + static_cast<int>(((static_cast<float>(m_ThumbPosition - m_MinValue)) / (m_MaxValue - m_MinValue)) * (middle_height - m_ThumbSize));
    int thumb_size = m_ThumbSize;
    if (thumb_size > middle_height)
    {
        thumb_size = middle_height;
    }
    // ================================================================
    // Rysujemy wnętrze scrollbara
    // ================================================================
    // najpierw tło nad suwakiem
    SDL_FRect DestRect;
    if (y_thumb > y)
    {
        DestRect = { m_Position.x + x, m_Position.y + y, m_Position.w, y_thumb - y };
        RenderTiledTexture(TextureMap, &rcAbove, &DestRect);
    }
    y = y_thumb;

    // teraz suwak
    RenderTexture(TextureMap, &m_Theme->scrollbar_vertical_thumb_top_rc, x, y);
    y += m_Theme->scrollbar_vertical_thumb_top_rc.h;
    // tło suwaka
    float thumb_size_bg = thumb_size - m_Theme->scrollbar_vertical_thumb_top_rc.h - m_Theme->scrollbar_vertical_thumb_bottom_rc.h;
    if (thumb_size_bg > 0)
    {
        DestRect = { m_Position.x + x, m_Position.y + y, m_Position.w, thumb_size_bg };
        RenderTiledTexture(TextureMap, &m_Theme->scrollbar_vertical_thumb_middle_rc, &DestRect);
        // teraz na tle suwaka centrujemy w pionie część środkową
        RenderTexture(TextureMap, &m_Theme->scrollbar_vertical_thumb_bg_rc, x, y + (thumb_size_bg - m_Theme->scrollbar_vertical_thumb_bg_rc.h) / 2.0f);
        y += thumb_size_bg;
    }
    RenderTexture(TextureMap, &m_Theme->scrollbar_vertical_thumb_bottom_rc, x, y);
    y += m_Theme->scrollbar_vertical_thumb_bottom_rc.h;
    // na koniec tło pod suwakiem
    if (y < m_Position.h - m_Theme->scrollbar_vertical_arrow_down_rc.h)
    {
        DestRect = { m_Position.x + x, m_Position.y + y, m_Position.w, m_Position.h - m_Theme->scrollbar_vertical_arrow_down_rc.h - y };
        RenderTiledTexture(TextureMap, &rcBottom, &DestRect);
        y += (m_Position.h - m_Theme->scrollbar_vertical_arrow_down_rc.h - y);
    }
    // ================================================================
    // Koniec rysowania wnętrza scrollbara
    // ================================================================


    // ================================================================
    // Rysujemy dolną strzałkę scrollbara
    // ================================================================
    SDL_FRect scrollbar_vertical_arrow_down_rc = m_Theme->scrollbar_vertical_arrow_down_rc;
    if (m_DetailedState == CSCROLLBAR_STATE_SMALL_SCROLLING_DOWN)
    {
        scrollbar_vertical_arrow_down_rc = m_Theme->scrollbar_vertical_arrow_down_pushed_rc;
    }
    else if (m_DetailedState == CSCROLLBAR_STATE_HOVER_DOWN_BUTTON)
    {
        scrollbar_vertical_arrow_down_rc = m_Theme->scrollbar_vertical_arrow_down_hover_rc;
    }

    if (y + scrollbar_vertical_arrow_down_rc.h >= m_Position.h)
    {
        y = m_Position.h - scrollbar_vertical_arrow_down_rc.h;
    }
    RenderTexture(TextureMap, &scrollbar_vertical_arrow_down_rc, x, y);
    // ================================================================
    // Koniec rysowania dolnej strzałki scrollbara
    // ================================================================

    if (m_Canvas)
    {
        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
}

/*
bool Tilc::Gui::TScrollBarVertical::Update(float DeltaTime)
{
    bool retval = false;

    if (!m_Visible) return false;

    TGuiControl::Update(DeltaTime);
    
    int step = INT_MIN;
    float localX = INT_MIN, thumbX = INT_MIN;
    float localY = INT_MIN, thumbY = INT_MIN;
    int calculatedPosition;
    int thumbMargin = 4;

    switch (m_DetailedState)
    {
    case CSCROLLBAR_STATE_SMALL_SCROLLING_UP:
        step = -m_SmallStep;
        retval = true;
        break;
    case CSCROLLBAR_STATE_SMALL_SCROLLING_DOWN:
        step = m_SmallStep;
        retval = true;
        break;
    case CSCROLLBAR_STATE_LARGE_SCROLLING_UP:
        step = -m_LargeStep;
        if (localX != INT_MIN && localY != INT_MIN)
        {
            calculatedPosition = CalculatePositionForStep(step);
            CalculateThumbLeftTopCorner(thumbX, thumbY, calculatedPosition);
            while (thumbY + m_ThumbSize < localY || thumbY + m_ThumbSize - localY < thumbMargin)
            {
                step /= 2;
                calculatedPosition = CalculatePositionForStep(step);
                CalculateThumbLeftTopCorner(thumbX, thumbY, calculatedPosition);
                if (step == 0)
                {
                    break;
                }
            }
            if (step != -m_LargeStep)
            {
                if (thumbY + m_ThumbSize < localY || thumbY + m_ThumbSize - localY < thumbMargin)
                {
                    step = 0;
                }
                SetDetailedState(CSCROLLBAR_STATE_NORMAL);
            }
        }
        retval = true;
        break;
    case CSCROLLBAR_STATE_LARGE_SCROLLING_DOWN:
        step = m_LargeStep;
        if (localX != INT_MIN && localY != INT_MIN)
        {
            calculatedPosition = CalculatePositionForStep(step);
            CalculateThumbLeftTopCorner(thumbX, thumbY, calculatedPosition);
            while (thumbY > localY || localY - thumbY < thumbMargin)
            {
                step /= 2;
                calculatedPosition = CalculatePositionForStep(step);
                CalculateThumbLeftTopCorner(thumbX, thumbY, calculatedPosition);
                if (step == 0)
                {
                    break;
                }
            }
            if (step != m_LargeStep)
            {
                if (thumbY > localY || localY - thumbY < thumbMargin)
                {
                    step = 0;
                }
                SetDetailedState(CSCROLLBAR_STATE_NORMAL);
            }
        }
        retval = true;
        break;
    }


    if (step != INT_MIN)
    {
        StepBy(step, true);
    }

    return retval;
}
*/

bool Tilc::Gui::TScrollBarVertical::MouseOnThumb(int localX, int localY)
{
    float CurrentThumbX, CurrentThumbY;
    CalculateThumbLeftTopCorner(CurrentThumbX, CurrentThumbY);

    localX -= m_OffsetX;
    localY -= m_OffsetY;
    if (localX >= CurrentThumbX &&
        localX <= CurrentThumbX + Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_thumb_middle_rc.w &&
        localY >= CurrentThumbY && localY <= CurrentThumbY + m_ThumbSize)
    {
        return true;
    }
    return false;
}

bool Tilc::Gui::TScrollBarVertical::MouseOnArrowUp(int localX, int localY)
{
    //std::cout << "local: (" << localX << ", " << localY << std::endl;
    if (localX >= 0 && localX <= Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_arrow_up_rc.w &&
        localY >= 0 && localY <= Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_arrow_up_rc.h)
    {
        return true;
    }

    return false;
}

bool Tilc::Gui::TScrollBarVertical::MouseOnArrowDown(int localX, int localY)
{
    if (localX >= 0  && localX <= Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_arrow_down_rc.w &&
        localY >= m_Position.h - Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_arrow_down_rc.h && localY <= m_Position.h)
    {
        return true;
    }

    return false;
}

bool Tilc::Gui::TScrollBarVertical::MouseOnBgUp(int localX, int localY)
{
    float CurrentThumbX, CurrentThumbY;
    CalculateThumbLeftTopCorner(CurrentThumbX, CurrentThumbY);

    if (localX >= 0 &&
        localX <= Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_bg_rc.w &&
        localY >= Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_arrow_up_rc.h && localY < CurrentThumbY)
    {
        return true;
    }
    return false;
}

bool Tilc::Gui::TScrollBarVertical::MouseOnBgDown(int localX, int localY)
{
    float CurrentThumbX, CurrentThumbY;
    CalculateThumbLeftTopCorner(CurrentThumbX, CurrentThumbY);

    if (localX >= 0 &&
        localX <= Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_bg_rc.w &&
        localY >= CurrentThumbY + m_ThumbSize &&
        localY < m_Position.h - Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_arrow_down_rc.h)
    {
        return true;
    }
    return false;
}

bool Tilc::Gui::TScrollBarVertical::OnMouseMove(const SDL_Event& event)
{
    if (!m_Visible) return false;
    Tilc::Gui::TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    if (OtherControlCapturedMouse())
    {
        return false;
    }

    Tilc::Gui::TGuiControl::OnMouseMove(event);

    SDL_FRect RealPosition = GetRealPosition();

    // jeśli przeciągamy suwak i jesteśmy poza nim, to i tak obsługujemy to zdarzenie
    if (m_ControlThatCapturedMouse == this)
    {
        //SDL_Log("m_Dragging: %d,  m_DetailedState: %d: ", m_Dragging, m_DetailedState);
        if (m_Dragging && m_DetailedState == CSCROLLBAR_STATE_THUMB_DRAGGING)
        {
            float localX;
            float localY;
            localX = event.motion.x - RealPosition.x;
            localY = event.motion.y - RealPosition.y;

            int position = CalculatePositionForThumbCoords(localX - m_ThumbOffsetX, localY - m_ThumbOffsetY);
            //SDL_Log("%6.2f x %6.2f          %6.2f x %6.2f          %6.2f x %6.2f          %6.2f x %6.2f:         %d", dx, dy, mx, my, m_DragStartX, m_DragStartY, m_ThumbOffsetX, m_ThumbOffsetY, position);
            //SDL_Log("Position: %d", position);
            SetPosition(position, true, true);
        }
        return true;
    }

    int OldDetailedState = m_DetailedState;

    m_DetailedState = 0;

    SDL_FPoint p{ event.motion.x, event.motion.y };
    SDL_FRect TestedRect = RealPosition;
    // sprawdzamy czy jestesmy nad strzałką do góry, po to by ustawić hovering state dla buttona tej strzalki
    TestedRect.h = t->scrollbar_vertical_arrow_up_rc.h;
    if (SDL_PointInRectFloat(&p, &TestedRect))
    {
        SetDetailedState(CSCROLLBAR_STATE_HOVER_UP_BUTTON);
    }
    else
    {
        // sprawdzamy czy jestesmy nad strzałką w dół, po to by ustawić hovering state dla buttona tej strzalki
        TestedRect.h = t->scrollbar_vertical_arrow_down_rc.h;
        TestedRect.y = RealPosition.y + RealPosition.h - TestedRect.h;
        if (SDL_PointInRectFloat(&p, &TestedRect))
        {
            SetDetailedState(CSCROLLBAR_STATE_HOVER_DOWN_BUTTON);
        }
    }
    if (m_DetailedState != OldDetailedState)
    {
        Invalidate();
    }

    if (PointIn(event.motion.x, event.motion.y))
    {
        // jeśli lewy przycisk myszki został wciśnięty, ale nie zwolniony, to
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            SDL_FRect Position = GetRealPosition();
            float localX = event.motion.x - Position.x;
            float localY = event.motion.y - Position.y;

            // jeśli jesteśmy nad suwakiem, to przechodzimy do odpowiedniego stanu
            if (m_StateInitiatedByMouseDown == CSCROLLBAR_STATE_THUMB_DRAGGING && MouseOnThumb(localX, localY))
            {
                SetDetailedState(CSCROLLBAR_STATE_THUMB_DRAGGING);
            }

            // jeśli przeciągamy suwak, to zmieniamy jego położenie
            if (m_DetailedState == CSCROLLBAR_STATE_THUMB_DRAGGING)
            {
                int position = CalculatePositionForThumbCoords(localX - m_ThumbOffsetX, localY - m_ThumbOffsetY);
                SetPosition(position, true, true);
            }

            // jeśli jesteśmy nad strzałką do góry, to przechodzimy do odpowiedniego stanu
            else if (m_StateInitiatedByMouseDown == CSCROLLBAR_STATE_SMALL_SCROLLING_UP && MouseOnArrowUp(localX, localY))
            {
                SetDetailedState(CSCROLLBAR_STATE_SMALL_SCROLLING_UP);
                StepBy(-m_SmallStep, true);
            }

            // jeśli jesteśmy nad strzałką w dół, to przechodzimy do odpowiedniego stanu
            else if (m_StateInitiatedByMouseDown == CSCROLLBAR_STATE_SMALL_SCROLLING_DOWN && MouseOnArrowDown(localX, localY))
            {
                SetDetailedState(CSCROLLBAR_STATE_SMALL_SCROLLING_DOWN);
                StepBy(m_SmallStep, true);
            }

            // jeśli jesteśmy nad tłem nad suwakiem, to przechodzimy do odpowiedniego stanu
            else if (m_StateInitiatedByMouseDown == CSCROLLBAR_STATE_LARGE_SCROLLING_UP && MouseOnBgUp(localX, localY))
            {
                SetDetailedState(CSCROLLBAR_STATE_LARGE_SCROLLING_UP);
                StepBy(-m_LargeStep, true);
            }

            // jeśli jesteśmy nad tłem pod suwakiem, to przechodzimy do odpowiedniego stanu
            else if (m_StateInitiatedByMouseDown == CSCROLLBAR_STATE_LARGE_SCROLLING_DOWN && MouseOnBgDown(localX, localY))
            {
                SetDetailedState(CSCROLLBAR_STATE_LARGE_SCROLLING_DOWN);
                StepBy(m_LargeStep, true);
            }
        }
        return true;
    }

    SetDetailedState(CSCROLLBAR_STATE_NORMAL);
    if (m_DetailedState != OldDetailedState)
    {
        Invalidate();
    }

    return false;
}

bool Tilc::Gui::TScrollBarVertical::OnMouseButtonDown(const SDL_Event& event)
{
    if (!m_Visible) return false;
    if (OtherControlCapturedMouse())
    {
        return false;
    }

    Tilc::Gui::TGuiControl::OnMouseButtonDown(event);

    SDL_FRect Position = GetRealPosition();

    /*
    std::cout << "Event: (" << event.motion.x << ", " << event.motion.y << ")  " << std::endl
        << "    Position: (" << m_Position.x << ", " << m_Position.y << ") " << std::endl
        << "    RealPosition: (" << Position.x << ", " << Position.y << ") " << std::endl
        << "    ParentPosition: (" << m_Parent->m_Position.x << ", " << m_Parent->m_Position.y << ") " << std::endl;
    */
    float localX = event.button.x - Position.x;
    float localY = event.button.y - Position.y;

    if (PointIn(event.button.x, event.button.y))
    {
        GetParentWindow()->CaptureMouse(this);

        // jeśli kliknięto w strzałkę do góry, to przechodzimy do odpowiedniego stanu
        if (MouseOnArrowUp(localX, localY))
        {
            SetDetailedState(CSCROLLBAR_STATE_SMALL_SCROLLING_UP);
            StepBy(-m_SmallStep, true);
        }

        // jeśli kliknięto w strzałkę w dół, to przechodzimy do odpowiedniego stanu
        if (MouseOnArrowDown(localX, localY))
        {
            SetDetailedState(CSCROLLBAR_STATE_SMALL_SCROLLING_DOWN);
            StepBy(m_SmallStep, true);
        }

        // jeśli kliknięto w tło nad suwakiem, to przechodzimy do odpowiedniego stanu
        if (MouseOnBgUp(localX, localY))
        {
            SetDetailedState(CSCROLLBAR_STATE_LARGE_SCROLLING_UP);
            StepBy(-m_LargeStep, true);
        }

        // jeśli kliknięto w tło pod suwakiem, to przechodzimy do odpowiedniego stanu
        if (MouseOnBgDown(localX, localY))
        {
            SetDetailedState(CSCROLLBAR_STATE_LARGE_SCROLLING_DOWN);
            StepBy(m_LargeStep, true);
        }

        // jeśli kliknięto w suwak, to przechodzimy do odpowiedniego stanu
        if (MouseOnThumb(localX, localY))
        {
            SetDetailedState(CSCROLLBAR_STATE_THUMB_DRAGGING);
            CalculateThumbLeftTopCorner(m_ThumbOffsetX, m_ThumbOffsetY);
            m_ThumbOffsetX = localX - m_ThumbOffsetX;
            m_ThumbOffsetY = localY - m_ThumbOffsetY;
        }

        m_StateInitiatedByMouseDown = m_DetailedState;
        return true;
    }

    return false;
}

bool Tilc::Gui::TScrollBarVertical::OnMouseButtonUp(const SDL_Event& event)
{
    if (!m_Visible) return false;
    if (OtherControlCapturedMouse())
    {
        return false;
    }

    Tilc::Gui::TGuiControl::OnMouseButtonUp(event);

    SDL_FRect Position = GetRealPosition();
    float localX = event.button.x - Position.x;
    float localY = event.button.y - Position.y;
    SetDetailedState(CSCROLLBAR_STATE_NORMAL);

    // jeśli jesteśmy na strzałce do góry, to przechodzimy do odpowiedniego stanu
    if (MouseOnArrowUp(localX, localY))
    {
        SetDetailedState(CSCROLLBAR_STATE_HOVER_UP_BUTTON);
    }
    // jeśli kliknięto w strzałkę w dół, to przechodzimy do odpowiedniego stanu
    else if (MouseOnArrowDown(localX, localY))
    {
        SetDetailedState(CSCROLLBAR_STATE_HOVER_DOWN_BUTTON);
    }

    m_StateInitiatedByMouseDown = CSCROLLBAR_STATE_NORMAL;
    Invalidate();
    if (PointIn(event.button.x, event.button.y))
    {
        return true;
    }
    return false;
}
