#include "Tilc/Gui/SliderHorizontal.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/StyledWindow.h"

Tilc::Gui::TSliderHorizontal::TSliderHorizontal(Tilc::Gui::TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, int thumbPosition, int maxValue, int minValue, bool autosize)
    : Tilc::Gui::TScrollBarHorizontal(parent, name, position, Tilc::Gui::EControlType::ECT_Slider, thumbPosition, maxValue, minValue, autosize)
{
    TTheme * m_Theme = Tilc::GameObject->GetContext()->m_Theme;
    m_ThumbSize = m_Theme->slider_horizontal_thumb_rc.w;
}

void Tilc::Gui::TSliderHorizontal::SetSize(float width, float height)
{
    if (!m_Autosize)
    {
        Tilc::Gui::TGuiControl::SetSize(width, Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_thumb_middle_rc.h);
    }
}

void Tilc::Gui::TSliderHorizontal::SetSizeRelativeToParent(float parentWidth, float parentHeight)
{
    if (m_Autosize)
    {
        Tilc::Gui::TGuiControl::SetSize(parentWidth, Tilc::GameObject->GetContext()->m_Theme->scrollbar_horizontal_thumb_middle_rc.h);
    }
}

void Tilc::Gui::TSliderHorizontal::CalculateThumbLeftTopCorner(float& x, float& y, int position)
{
    if (position == INT_MIN)
    {
        position = m_ThumbPosition;
    }

    float middle_width = m_Position.w - m_ThumbSize;
    // musimy obliczyć położenie suwaka
    float x_thumb = ((static_cast<float>(position - m_MinValue)) / (m_MaxValue - m_MinValue)) * middle_width;

    x = x_thumb;
    y = 0;
}

int Tilc::Gui::TSliderHorizontal::CalculatePositionForThumbCoords(float x, float y)
{
    if (x == INT_MIN)
    {
        CalculateThumbLeftTopCorner(x, y);
    }

    float middle_width = m_Position.w - m_ThumbSize;

    float scale = x / middle_width;
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

void Tilc::Gui::TSliderHorizontal::Draw()
{
    m_NeedUpdate = ENeedUpdate::ENU_None;

    TTheme* m_Theme = Tilc::GameObject->GetContext()->m_Theme;
    SDL_Texture* OldRenderTarget{};
    float x = 0;
    float y = 0;
    SDL_Texture* TextureMap = m_Theme->GuiTextureMap1;
    SDL_FRect rcLeft = m_Theme->slider_horizontal_rail_left_rc;
    SDL_FRect rcRight = m_Theme->slider_horizontal_rail_right_rc;
    SDL_FRect rcMiddle = m_Theme->slider_horizontal_rail_middle_rc;
    float MiddlePos = (m_ThumbSize - rcMiddle.h) / 2.0f;
    if (m_Canvas)
    {
        OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);
    }
    // ================================================================
    // Rysujemy szynę
    // ================================================================
    RenderTexture(TextureMap, &rcLeft, x, y + MiddlePos);
    x += rcLeft.w;

    float MiddleWidth = m_Position.w - rcLeft.w - rcRight.w;
    RenderTexture(TextureMap, &rcMiddle, x, y + MiddlePos, MiddleWidth, rcMiddle.h);
    x += MiddleWidth;

    RenderTexture(TextureMap, &rcRight, x, y + MiddlePos);
    x += rcRight.w;
    // ================================================================
    // Koniec rysowania szyny
    // ================================================================

    // musimy obliczyć położenie suwaka
    float x_thumb = (static_cast<float>(m_ThumbPosition - m_MinValue) / (m_MaxValue - m_MinValue)) * (m_Position.w - m_ThumbSize);
    x = x_thumb;

    // teraz suwak
    RenderTexture(TextureMap, &m_Theme->slider_horizontal_thumb_rc, x, y);

    if (m_Canvas)
    {
        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
}

bool Tilc::Gui::TSliderHorizontal::MouseOnThumb(int localX, int localY)
{
    float CurrentThumbX, CurrentThumbY;
    CalculateThumbLeftTopCorner(CurrentThumbX, CurrentThumbY);

    if (localX >= CurrentThumbX && localX <= CurrentThumbX + m_ThumbSize && localY >= CurrentThumbY && localY <= CurrentThumbY + m_ThumbSize)
    {
        return true;
    }
    return false;
}

bool Tilc::Gui::TSliderHorizontal::MouseOnBgLeft(int localX, int localY)
{
    float CurrentThumbX, CurrentThumbY;
    CalculateThumbLeftTopCorner(CurrentThumbX, CurrentThumbY);

    if (localX >= 0 && localX < CurrentThumbX && localY >= 0 && localY < m_ThumbSize)
    {
        return true;
    }
    return false;
}

bool Tilc::Gui::TSliderHorizontal::MouseOnBgRight(int localX, int localY)
{
    float CurrentThumbX, CurrentThumbY;
    CalculateThumbLeftTopCorner(CurrentThumbX, CurrentThumbY);

    if (localX >= CurrentThumbX + m_ThumbSize && localX <= m_Position.w && localY >= 0 && localY < m_ThumbSize)
    {
        return true;
    }
    return false;
}

bool Tilc::Gui::TSliderHorizontal::OnMouseMove(const SDL_Event& event)
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

            // jeśli jesteśmy nad tłem na lewo od suwaka, to przechodzimy do odpowiedniego stanu
            else if (m_StateInitiatedByMouseDown == CSCROLLBAR_STATE_LARGE_SCROLLING_LEFT && MouseOnBgLeft(localX, localY))
            {
                SetDetailedState(CSCROLLBAR_STATE_LARGE_SCROLLING_LEFT);
                StepBy(-m_LargeStep, true);
            }

            // jeśli jesteśmy nad tłem na prawo od suwaka, to przechodzimy do odpowiedniego stanu
            else if (m_StateInitiatedByMouseDown == CSCROLLBAR_STATE_LARGE_SCROLLING_RIGHT && MouseOnBgRight(localX, localY))
            {
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

bool Tilc::Gui::TSliderHorizontal::OnMouseButtonDown(const SDL_Event& event)
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

        // jeśli kliknięto w tło na lewo od suwaka, to przechodzimy do odpowiedniego stanu
        if (MouseOnBgLeft(localX, localY))
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
        return true;
    }

    return false;
}

bool Tilc::Gui::TSliderHorizontal::OnMouseButtonUp(const SDL_Event& event)
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
