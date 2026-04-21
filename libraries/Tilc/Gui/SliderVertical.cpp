#include "Tilc/Gui/SliderVertical.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/StyledWindow.h"

Tilc::Gui::TSliderVertical::TSliderVertical(Tilc::Gui::TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, int thumbPosition, int maxValue, int minValue, bool autosize)
    : Tilc::Gui::TScrollBarVertical(parent, name, position, thumbPosition, maxValue, minValue, autosize)
{
    TTheme * m_Theme = Tilc::GameObject->GetContext()->m_Theme;
    m_ThumbSize = m_Theme->slider_vertical_thumb_rc.h;
}

void Tilc::Gui::TSliderVertical::SetSize(float width, float height)
{
    if (!m_Autosize)
    {
        Tilc::Gui::TGuiControl::SetSize(Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_thumb_middle_rc.w, height);
    }
}

void Tilc::Gui::TSliderVertical::SetSizeRelativeToParent(float parentWidth, float parentHeight)
{
    if (m_Autosize)
    {
        Tilc::Gui::TGuiControl::SetSize(Tilc::GameObject->GetContext()->m_Theme->scrollbar_vertical_thumb_middle_rc.w, parentHeight);
    }
}

void Tilc::Gui::TSliderVertical::CalculateThumbLeftTopCorner(float& x, float& y, int position)
{
    if (position == INT_MIN)
    {
        position = m_ThumbPosition;
    }

    float middle_height = m_Position.h - m_ThumbSize;
    // musimy obliczyć położenie suwaka
    float y_thumb = ((static_cast<float>(position - m_MinValue)) / (m_MaxValue - m_MinValue)) * middle_height;

    x = 0;
    y = y_thumb;
}

int Tilc::Gui::TSliderVertical::CalculatePositionForThumbCoords(float x, float y)
{
    if (x == INT_MIN)
    {
        CalculateThumbLeftTopCorner(x, y);
    }

    float middle_height = m_Position.h - m_ThumbSize;

    float scale = y / middle_height;
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

void Tilc::Gui::TSliderVertical::Draw()
{
    m_NeedUpdate = ENeedUpdate::ENU_None;

    TTheme* m_Theme = Tilc::GameObject->GetContext()->m_Theme;
    SDL_Texture* OldRenderTarget{};
    float x = 0;
    float y = 0;
    SDL_Texture* TextureMap = m_Theme->GuiTextureMap1;
    SDL_FRect rcTop = m_Theme->slider_vertical_rail_top_rc;
    SDL_FRect rcBottom = m_Theme->slider_vertical_rail_bottom_rc;
    SDL_FRect rcMiddle = m_Theme->slider_vertical_rail_middle_rc;
    float MiddlePos = (m_ThumbSize - rcMiddle.w) / 2.0f;
    if (m_Canvas)
    {
        OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);
    }
    // ================================================================
    // Rysujemy szynę
    // ================================================================
    RenderTexture(TextureMap, &rcTop, x + MiddlePos, y);
    y += rcTop.h;

    float MiddleHeight = m_Position.h - rcTop.h - rcBottom.h;
    RenderTexture(TextureMap, &rcMiddle, x + MiddlePos, y, rcMiddle.w, MiddleHeight);
    y += MiddleHeight;

    RenderTexture(TextureMap, &rcBottom, x + MiddlePos, y);
    y += rcBottom.h;
    // ================================================================
    // Koniec rysowania szyny
    // ================================================================

    // musimy obliczyć położenie suwaka
    float y_thumb = (static_cast<float>(m_ThumbPosition - m_MinValue) / (m_MaxValue - m_MinValue)) * (m_Position.h - m_ThumbSize);
    y = y_thumb;

    // teraz suwak
    RenderTexture(TextureMap, &m_Theme->slider_vertical_thumb_rc, x, y);

    if (m_Canvas)
    {
        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
}

bool Tilc::Gui::TSliderVertical::MouseOnThumb(int localX, int localY)
{
    float CurrentThumbX, CurrentThumbY;
    CalculateThumbLeftTopCorner(CurrentThumbX, CurrentThumbY);

    if (localX >= CurrentThumbX && localX <= CurrentThumbX + m_ThumbSize && localY >= CurrentThumbY && localY <= CurrentThumbY + m_ThumbSize)
    {
        return true;
    }
    return false;
}

bool Tilc::Gui::TSliderVertical::MouseOnBgUp(int localX, int localY)
{
    float CurrentThumbX, CurrentThumbY;
    CalculateThumbLeftTopCorner(CurrentThumbX, CurrentThumbY);

    if (localX >= 0 && localX < m_ThumbSize && localY >= 0 && localY < CurrentThumbY)
    {
        return true;
    }
    return false;
}

bool Tilc::Gui::TSliderVertical::MouseOnBgDown(int localX, int localY)
{
    float CurrentThumbX, CurrentThumbY;
    CalculateThumbLeftTopCorner(CurrentThumbX, CurrentThumbY);

    if (localX >= 0 && localX < m_ThumbSize && localY >= CurrentThumbY + m_ThumbSize && localY <= m_Position.h)
    {
        return true;
    }
    return false;
}

bool Tilc::Gui::TSliderVertical::OnMouseMove(const SDL_Event& event)
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
        if (m_Dragging && m_DetailedState == CSCROLLBAR_STATE_THUMB_DRAGGING)
        {
            float localX;
            float localY;
            localX = event.motion.x - RealPosition.x;
            localY = event.motion.y - RealPosition.y;

            int position = CalculatePositionForThumbCoords(localX - m_ThumbOffsetX, localY - m_ThumbOffsetY);
            SetPosition(position, true, true);
        }
        return true;
    }

    int OldDetailedState = m_DetailedState;

    m_DetailedState = 0;

    SDL_FPoint p{ event.motion.x, event.motion.y };

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
                SetDetailedState(CSCROLLBAR_STATE_THUMB_DRAGGING);
            }

            // jeśli przeciągamy suwak, to zmieniamy jego położenie
            if (m_DetailedState == CSCROLLBAR_STATE_THUMB_DRAGGING)
            {
                long position = CalculatePositionForThumbCoords(localX - m_ThumbOffsetX, localY - m_ThumbOffsetY);
                SetPosition(position, true, true);
            }

            // jeśli jesteśmy nad tłem u góry od suwaka, to przechodzimy do odpowiedniego stanu
            else if (m_StateInitiatedByMouseDown == CSCROLLBAR_STATE_LARGE_SCROLLING_LEFT && MouseOnBgUp(localX, localY))
            {
                SetDetailedState(CSCROLLBAR_STATE_LARGE_SCROLLING_UP);
                StepBy(-m_LargeStep, true);
            }

            // jeśli jesteśmy nad tłem na dół od suwaka, to przechodzimy do odpowiedniego stanu
            else if (m_StateInitiatedByMouseDown == CSCROLLBAR_STATE_LARGE_SCROLLING_RIGHT && MouseOnBgDown(localX, localY))
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

bool Tilc::Gui::TSliderVertical::OnMouseButtonDown(const SDL_Event& event)
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

        // jeśli kliknięto w tło u góry od suwaka, to przechodzimy do odpowiedniego stanu
        if (MouseOnBgUp(localX, localY))
        {
            SetDetailedState(CSCROLLBAR_STATE_LARGE_SCROLLING_UP);
            StepBy(-m_LargeStep, true);
        }

        // jeśli kliknięto w tło na dół od suwaka, to przechodzimy do odpowiedniego stanu
        else if (MouseOnBgDown(localX, localY))
        {
            SetDetailedState(CSCROLLBAR_STATE_LARGE_SCROLLING_DOWN);
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
        return true;
    }

    return false;
}

bool Tilc::Gui::TSliderVertical::OnMouseButtonUp(const SDL_Event& event)
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

    m_StateInitiatedByMouseDown = CSCROLLBAR_STATE_NORMAL;
    Invalidate();
    if (PointIn(event.button.x, event.button.y))
    {
        return true;
    }
    return false;
}
