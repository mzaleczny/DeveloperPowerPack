#include "Tilc/Gui/ScrollBarHorizontal.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/StyledWindow.h"

Tilc::Gui::TScrollBarHorizontal::TScrollBarHorizontal(Tilc::Gui::TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, int thumbPosition, int maxValue, int minValue, bool autosize)
    : Tilc::Gui::TScrollBar(parent, name, position, thumbPosition, maxValue, minValue, autosize)
{
    // w konstruktorze ustawiamy minimalną wielkość rozmiaru suwaka
    SetThumbSize();
}

void Tilc::Gui::TScrollBarHorizontal::SetSize(float width, float height)
{
    if (!m_Autosize)
    {
        Tilc::Gui::TGuiControl::SetSize(width, Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_thumb_middle_rc.h);
        SetThumbSize();
    }
}

void Tilc::Gui::TScrollBarHorizontal::SetSizeRelativeToParent(float parentWidth, float parentHeight)
{
    if (m_Autosize)
    {
        Tilc::Gui::TGuiControl::SetSize(parentWidth, Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_arrow_left_rc.h);
        SetThumbSize();
    }
}

void Tilc::Gui::TScrollBarHorizontal::SetThumbSize()
{
    // ustawiamy wielkość suwaka
    int delta = m_MaxValue - m_MinValue;
    int thumb_size = m_Position.w / delta;
    if (thumb_size < m_Position.w / 5)
    {
        thumb_size = m_Position.w / 5;
    }
    if (thumb_size > m_Position.w - Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_arrow_left_rc.w - Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_arrow_right_rc.w)
    {
        thumb_size = m_Position.w - Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_arrow_left_rc.w - Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_arrow_right_rc.w;
    }
    if (thumb_size < Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_thumb_left_rc.w + Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_thumb_middle_rc.w + Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_thumb_right_rc.w)
    {
        thumb_size = Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_thumb_left_rc.w + Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_thumb_middle_rc.w + Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_thumb_right_rc.w;
    }

    m_ThumbSize = thumb_size;
}

void Tilc::Gui::TScrollBarHorizontal::CalculateThumbLeftTopCorner(float& x, float& y, int position)
{
    if (position == INT_MIN)
    {
        position = m_ThumbPosition;
    }

    float middle_width = m_Position.w - Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_arrow_left_rc.w - Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_arrow_right_rc.w;
    // musimy obliczyć położenie suwaka
    float x_thumb = Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_arrow_left_rc.w + (static_cast<float>(position - m_MinValue) / (m_MaxValue - m_MinValue)) * (middle_width - m_ThumbSize);

    x = x_thumb;
    y = 0;
}

int Tilc::Gui::TScrollBarHorizontal::CalculatePositionForThumbCoords(float x, float y)
{
    if (x == INT_MIN)
    {
        CalculateThumbLeftTopCorner(x, y);
    }

    float middle_width = m_Position.w - Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_arrow_left_rc.w - Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_arrow_right_rc.w;
    x -= Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_arrow_right_rc.w;

    float scale = x / (middle_width - m_ThumbSize);
    long position = m_MinValue + scale * (m_MaxValue - m_MinValue);
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

void Tilc::Gui::TScrollBarHorizontal::Draw()
{
    m_NeedUpdate = ENeedUpdate::ENU_None;

    TTheme* m_Theme = Tilc::GameObject->GetContext()->m_Theme;
    SDL_Texture* OldRenderTarget{};
    float x = 0;
    float y = 0;
    SDL_Texture* TextureMap = m_Theme->GuiTextureMap1;
    SDL_FRect rcLeft = m_Theme->scrollbar_horizontal_bg_rc;
    SDL_FRect rcRight = m_Theme->scrollbar_horizontal_bg_rc;
    if (m_DetailedState == CSCROLLBAR_STATE_LARGE_SCROLLING_LEFT)
    {
        rcLeft = m_Theme->scrollbar_horizontal_bg_pushed_rc;
        rcRight = m_Theme->scrollbar_horizontal_bg_rc;
    }
    if (m_DetailedState == CSCROLLBAR_STATE_LARGE_SCROLLING_RIGHT)
    {
        rcLeft = m_Theme->scrollbar_horizontal_bg_rc;
        rcRight = m_Theme->scrollbar_horizontal_bg_pushed_rc;
    }

    if (m_Canvas)
    {
        OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);
    }
    // ================================================================
    // Rysujemy lewą strzałkę scrollbara
    // ================================================================
    SDL_FRect scrollbar_horizontal_arrow_left_rc = m_Theme->scrollbar_horizontal_arrow_left_rc;
    if (m_DetailedState == CSCROLLBAR_STATE_SMALL_SCROLLING_LEFT)
    {
        scrollbar_horizontal_arrow_left_rc = m_Theme->scrollbar_horizontal_arrow_left_pushed_rc;
    }
    else if (m_DetailedState == CSCROLLBAR_STATE_HOVER_LEFT_BUTTON)
    {
        scrollbar_horizontal_arrow_left_rc = m_Theme->scrollbar_horizontal_arrow_left_hover_rc;
    }
    RenderTexture(TextureMap, &scrollbar_horizontal_arrow_left_rc, x, y);
    x += scrollbar_horizontal_arrow_left_rc.w;

    // ================================================================
    // Koniec rysowania lewej strzałki scrollbara
    // ================================================================


    float middle_width = m_Position.w - m_Theme->scrollbar_horizontal_arrow_left_rc.w - m_Theme->scrollbar_horizontal_arrow_right_rc.w;
    // musimy obliczyć położenie suwaka
    float x_thumb = x + (static_cast<float>(m_ThumbPosition - m_MinValue) / (m_MaxValue - m_MinValue)) * (middle_width - m_ThumbSize);
    long thumb_size = m_ThumbSize;
    if (thumb_size > middle_width)
    {
        thumb_size = middle_width;
    }
    // ================================================================
    // Rysujemy wnętrze scrollbara
    // ================================================================
    // najpierw tło po lewej stronie suwaka
    SDL_FRect DestRect;
    if (x_thumb > x)
    {
        DestRect = { m_Position.x + x, m_Position.y + y, x_thumb - x, m_Position.h };
        RenderTiledTexture(TextureMap, &rcLeft, &DestRect);
    }
    x = x_thumb;

    // teraz suwak
    RenderTexture(TextureMap, &m_Theme->scrollbar_horizontal_thumb_left_rc, x, y);
    x += m_Theme->scrollbar_horizontal_thumb_left_rc.w;
    // tło suwaka
    float thumb_size_bg = thumb_size - m_Theme->scrollbar_horizontal_thumb_left_rc.w - m_Theme->scrollbar_horizontal_thumb_right_rc.w;
    if (thumb_size_bg > 0)
    {
        DestRect = { m_Position.x + x, m_Position.y + y, thumb_size_bg, m_Position.h };
        RenderTiledTexture(TextureMap, &m_Theme->scrollbar_horizontal_thumb_middle_rc, &DestRect);
        // teraz na tle suwaka centrujemy w poziomie część środkową
        RenderTexture(TextureMap, &m_Theme->scrollbar_horizontal_thumb_bg_rc, x + (thumb_size_bg - m_Theme->scrollbar_horizontal_thumb_bg_rc.w) / 2.0f, y);
        x += thumb_size_bg;
    }
    RenderTexture(TextureMap, &m_Theme->scrollbar_horizontal_thumb_right_rc, x, y);
    x += m_Theme->scrollbar_horizontal_thumb_right_rc.w;
    // na koniec tło po prawej stronie suwaka
    if (x < m_Position.w - m_Theme->scrollbar_horizontal_arrow_right_rc.w)
    {
        DestRect = { m_Position.x + x, m_Position.y + y, m_Position.w - m_Theme->scrollbar_horizontal_arrow_right_rc.w - x, m_Position.h };
        RenderTiledTexture(TextureMap, &rcRight, &DestRect);
        x += m_Position.w - m_Theme->scrollbar_horizontal_arrow_right_rc.w - x;
    }
    // ================================================================
    // Koniec rysowania wnętrza scrollbara
    // ================================================================


    // ================================================================
    // Rysujemy prawą strzałkę scrollbara
    // ================================================================
    SDL_FRect scrollbar_horizontal_arrow_right_rc = m_Theme->scrollbar_horizontal_arrow_right_rc;
    if (m_DetailedState == CSCROLLBAR_STATE_SMALL_SCROLLING_RIGHT)
    {
        scrollbar_horizontal_arrow_right_rc = m_Theme->scrollbar_horizontal_arrow_right_pushed_rc;
    }
    else if (m_DetailedState == CSCROLLBAR_STATE_HOVER_RIGHT_BUTTON)
    {
        scrollbar_horizontal_arrow_right_rc = m_Theme->scrollbar_horizontal_arrow_right_hover_rc;
    }

    if (x + m_Theme->scrollbar_horizontal_arrow_right_pushed_rc.w >= m_Position.w)
    {
        x = m_Position.w - m_Theme->scrollbar_horizontal_arrow_right_pushed_rc.w;
    }
    RenderTexture(TextureMap, &scrollbar_horizontal_arrow_right_rc, x, y);
    // ================================================================
    // Koniec rysowania prawej strzałki scrollbara
    // ================================================================

    if (m_Canvas)
    {
        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
}

/*
bool Tilc::Gui::TScrollBarHorizontal::Update(float DeltaTime)
{
    bool retval = false;

    if (!m_Visible) return false;

    TGuiControl::Update(DeltaTime);

    long step = INT_MIN;
    float localX = INT_MIN, thumbX = INT_MIN;
    float localY = INT_MIN, thumbY = INT_MIN;
    long calculatedPosition;
    long thumbMargin = 4;

    switch (m_DetailedState)
    {
    case CSCROLLBAR_STATE_SMALL_SCROLLING_LEFT:
        step = -m_SmallStep;
        retval = true;
        break;
    case CSCROLLBAR_STATE_SMALL_SCROLLING_RIGHT:
        step = m_SmallStep;
        retval = true;
        break;
    case CSCROLLBAR_STATE_LARGE_SCROLLING_LEFT:
        step = -m_LargeStep;
        if (localX != INT_MIN && localY != INT_MIN)
        {
            calculatedPosition = CalculatePositionForStep(step);
            CalculateThumbLeftTopCorner(thumbX, thumbY, calculatedPosition);
            while (thumbX + m_ThumbSize < localX || thumbX + m_ThumbSize - localX < thumbMargin)
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
                if (thumbX + m_ThumbSize < localX || thumbX + m_ThumbSize - localX < thumbMargin)
                {
                    step = 0;
                }
                SetDetailedState(CSCROLLBAR_STATE_NORMAL);
            }
        }
        retval = true;
        break;
    case CSCROLLBAR_STATE_LARGE_SCROLLING_RIGHT:
        step = m_LargeStep;
        if (localX != INT_MIN && localY != INT_MIN)
        {
            calculatedPosition = CalculatePositionForStep(step);
            CalculateThumbLeftTopCorner(thumbX, thumbY, calculatedPosition);
            while (thumbX > localX || localX - thumbX < thumbMargin)
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
                if (thumbX > localX || localX - thumbX < thumbMargin)
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

bool Tilc::Gui::TScrollBarHorizontal::MouseOnThumb(int localX, int localY)
{
    float CurrentThumbX, CurrentThumbY;
    CalculateThumbLeftTopCorner(CurrentThumbX, CurrentThumbY);

    if (localX >= CurrentThumbX && localX <= CurrentThumbX + m_ThumbSize &&
        localY >= CurrentThumbY &&
        localY <= CurrentThumbY + Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_thumb_middle_rc.h)
    {
        return true;
    }
    return false;
}

bool Tilc::Gui::TScrollBarHorizontal::MouseOnArrowLeft(int localX, int localY)
{
    if (localX >= 0 && localX <= Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_arrow_left_rc.w &&
        localY >= 0 && localY <= Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_arrow_left_rc.h)
    {
        return true;
    }

    return false;
}

bool Tilc::Gui::TScrollBarHorizontal::MouseOnArrowRight(int localX, int localY)
{
    if (localX >= m_Position.w - Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_arrow_right_rc.w  && localX <= m_Position.w &&
        localY >= 0 && localY <= Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_arrow_right_rc.h)
    {
        return true;
    }

    return false;
}

bool Tilc::Gui::TScrollBarHorizontal::MouseOnBgLeft(int localX, int localY)
{
    float CurrentThumbX, CurrentThumbY;
    CalculateThumbLeftTopCorner(CurrentThumbX, CurrentThumbY);

    if (localX >= Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_arrow_left_rc.w &&
        localX <= CurrentThumbX &&
        localY >= 0 && localY < Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_bg_rc.h)
    {
        return true;
    }
    return false;
}

bool Tilc::Gui::TScrollBarHorizontal::MouseOnBgRight(int localX, int localY)
{
    float CurrentThumbX, CurrentThumbY;
    CalculateThumbLeftTopCorner(CurrentThumbX, CurrentThumbY);

    if (localX >= CurrentThumbX + m_ThumbSize &&
        localX <= m_Position.w - Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_arrow_right_rc.w &&
        localY >= 0 &&
        localY < Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_bg_rc.h)
    {
        return true;
    }
    return false;
}

bool Tilc::Gui::TScrollBarHorizontal::OnMouseMove(const SDL_Event& event)
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
    // sprawdzamy czy jestesmy nad strzałką w lewo, po to by ustawić hovering state dla buttona tej strzalki
    TestedRect.w = t->scrollbar_horizontal_arrow_left_rc.w;
    if (SDL_PointInRectFloat(&p, &TestedRect))
    {
        //SDL_Log("Call SetDetailedState to: %d - CSCROLLBAR_STATE_HOVER_LEFT_BUTTON", CSCROLLBAR_STATE_HOVER_LEFT_BUTTON);
        SetDetailedState(CSCROLLBAR_STATE_HOVER_LEFT_BUTTON);
    }
    else
    {
        // sprawdzamy czy jestesmy nad strzałką w prawo, po to by ustawić hovering state dla buttona tej strzalki
        TestedRect.w = t->scrollbar_horizontal_arrow_right_rc.w;
        TestedRect.x = RealPosition.x + RealPosition.w - TestedRect.w;
        if (SDL_PointInRectFloat(&p, &TestedRect))
        {
            //SDL_Log("Call SetDetailedState to: %d - CSCROLLBAR_STATE_HOVER_RIGHT_BUTTON", CSCROLLBAR_STATE_HOVER_RIGHT_BUTTON);
            SetDetailedState(CSCROLLBAR_STATE_HOVER_RIGHT_BUTTON);
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
            float localX = event.motion.x - m_Position.x;
            float localY = event.motion.y - m_Position.y;

            // jeśli jesteśmy nad suwakiem, to przechodzimy do odpowiedniego stanu
            if (m_StateInitiatedByMouseDown == CSCROLLBAR_STATE_THUMB_DRAGGING && MouseOnThumb(localX, localY))
            {
                //SDL_Log("Call SetDetailedState to: %d - CSCROLLBAR_STATE_THUMB_DRAGGING", CSCROLLBAR_STATE_THUMB_DRAGGING);
                SetDetailedState(CSCROLLBAR_STATE_THUMB_DRAGGING);
            }

            // jeśli przeciągamy suwak, to zmieniamy jego położenie
            if (m_DetailedState == CSCROLLBAR_STATE_THUMB_DRAGGING)
            {
                long position = CalculatePositionForThumbCoords(localX - m_ThumbOffsetX, localY - m_ThumbOffsetY);
                SetPosition(position, true, true);
            }

            // jeśli jesteśmy nad strzałką w lewo, to przechodzimy do odpowiedniego stanu
            else if (m_StateInitiatedByMouseDown == CSCROLLBAR_STATE_SMALL_SCROLLING_LEFT && MouseOnArrowLeft(localX, localY))
            {
                //SDL_Log("Call SetDetailedState to: %d - CSCROLLBAR_STATE_SMALL_SCROLLING_LEFT", CSCROLLBAR_STATE_SMALL_SCROLLING_LEFT);
                SetDetailedState(CSCROLLBAR_STATE_SMALL_SCROLLING_LEFT);
                StepBy(-m_SmallStep, true);
            }

            // jeśli jesteśmy nad strzałką w prawo, to przechodzimy do odpowiedniego stanu
            else if (m_StateInitiatedByMouseDown == CSCROLLBAR_STATE_SMALL_SCROLLING_RIGHT && MouseOnArrowRight(localX, localY))
            {
                //SDL_Log("Call SetDetailedState to: %d - CSCROLLBAR_STATE_SMALL_SCROLLING_RIGHT", CSCROLLBAR_STATE_SMALL_SCROLLING_RIGHT);
                SetDetailedState(CSCROLLBAR_STATE_SMALL_SCROLLING_RIGHT);
                StepBy(m_SmallStep, true);
            }

            // jeśli jesteśmy nad tłem na lewo od suwaka, to przechodzimy do odpowiedniego stanu
            else if (m_StateInitiatedByMouseDown == CSCROLLBAR_STATE_LARGE_SCROLLING_LEFT && MouseOnBgLeft(localX, localY))
            {
                //SDL_Log("Call SetDetailedState to: CSCROLLBAR_STATE_LARGE_SCROLLING_LEFT - %d", CSCROLLBAR_STATE_LARGE_SCROLLING_LEFT);
                SetDetailedState(CSCROLLBAR_STATE_LARGE_SCROLLING_LEFT);
                StepBy(-m_LargeStep, true);
            }

            // jeśli jesteśmy nad tłem na prawo od suwaka, to przechodzimy do odpowiedniego stanu
            else if (m_StateInitiatedByMouseDown == CSCROLLBAR_STATE_LARGE_SCROLLING_RIGHT && MouseOnBgRight(localX, localY))
            {
                //SDL_Log("Call SetDetailedState to: CSCROLLBAR_STATE_LARGE_SCROLLING_RIGHT - %d", CSCROLLBAR_STATE_LARGE_SCROLLING_RIGHT);
                SetDetailedState(CSCROLLBAR_STATE_LARGE_SCROLLING_RIGHT);
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

bool Tilc::Gui::TScrollBarHorizontal::OnMouseButtonDown(const SDL_Event& event)
{
    if (!m_Visible) return false;
    if (OtherControlCapturedMouse())
    {
        return false;
    }

    Tilc::Gui::TGuiControl::OnMouseButtonDown(event);

    SDL_FRect Position = GetRealPosition();
    float localX = event.button.x - Position.x;
    float localY = event.button.y - Position.y;

    if (PointIn(event.button.x, event.button.y))
    {
        GetParentWindow()->CaptureMouse(this);

        // jeśli kliknięto w strzałkę w lewo, to przechodzimy do odpowiedniego stanu
        if (MouseOnArrowLeft(localX, localY))
        {
            SetDetailedState(CSCROLLBAR_STATE_SMALL_SCROLLING_LEFT);
            StepBy(-m_SmallStep, true);
        }

        // jeśli kliknięto w strzałkę w prawo, to przechodzimy do odpowiedniego stanu
        else if (MouseOnArrowRight(localX, localY))
        {
            SetDetailedState(CSCROLLBAR_STATE_SMALL_SCROLLING_RIGHT);
            StepBy(m_SmallStep, true);
        }

        // jeśli kliknięto w tło na lewo od suwaka, to przechodzimy do odpowiedniego stanu
        else if (MouseOnBgLeft(localX, localY))
        {
            SetDetailedState(CSCROLLBAR_STATE_LARGE_SCROLLING_LEFT);
            StepBy(-m_LargeStep, true);
        }

        // jeśli kliknięto w tło na prawo od suwaka, to przechodzimy do odpowiedniego stanu
        else if (MouseOnBgRight(localX, localY))
        {
            SetDetailedState(CSCROLLBAR_STATE_LARGE_SCROLLING_RIGHT);
            StepBy(m_LargeStep, true);
        }

        // jeśli kliknięto w suwak, to przechodzimy do odpowiedniego stanu
        else if (MouseOnThumb(localX, localY))
        {
            SetDetailedState(CSCROLLBAR_STATE_THUMB_DRAGGING);
            CalculateThumbLeftTopCorner(m_ThumbOffsetX, m_ThumbOffsetY);
            m_ThumbOffsetX = localX - m_ThumbOffsetX;
            m_ThumbOffsetY = localY - m_ThumbOffsetY;
        }

        m_StateInitiatedByMouseDown = m_DetailedState;
        //SDL_Log("m_DetailedState: %d", m_DetailedState);
        return true;
    }

    return false;
}

bool Tilc::Gui::TScrollBarHorizontal::OnMouseButtonUp(const SDL_Event& event)
{
    if (!m_Visible) return false;
    if (OtherControlCapturedMouse())
    {
        return false;
    }

    Tilc::Gui::TGuiControl::OnMouseButtonUp(event);

    //this->getParentWindow()->captureMouse(NULL);

    SDL_FRect Position = GetRealPosition();
    float localX = event.button.x - Position.x;
    float localY = event.button.y - Position.y;
    SetDetailedState(CSCROLLBAR_STATE_NORMAL);

    // jeśli jesteśmy na strzałce w lewo, to przechodzimy do odpowiedniego stanu
    if (MouseOnArrowLeft(localX, localY))
    {
        SetDetailedState(CSCROLLBAR_STATE_HOVER_LEFT_BUTTON);
    }

    // jeśli kliknięto w strzałkę w prawo, to przechodzimy do odpowiedniego stanu
    else if (MouseOnArrowRight(localX, localY))
    {
        SetDetailedState(CSCROLLBAR_STATE_HOVER_RIGHT_BUTTON);
    }

    m_StateInitiatedByMouseDown = CSCROLLBAR_STATE_NORMAL;
    Invalidate();
    if (PointIn(event.button.x, event.button.y))
    {
        return true;
    }
    return false;
}
