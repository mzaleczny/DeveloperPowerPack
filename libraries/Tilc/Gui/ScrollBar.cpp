#include "Tilc/Gui/ScrollBar.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/StyledWindow.h"

Tilc::Gui::TScrollBar::TScrollBar(Tilc::Gui::TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, int thumbPosition, int maxValue, int minValue, bool autosize)
    : Tilc::Gui::TGuiControl(parent, name, position), m_ThumbOffsetX{}, m_ThumbOffsetY{}
{
    m_Type = CSCROLLBAR_TYPE_UNDEFINED;
    m_State = CSCROLLBAR_STATE_NORMAL;
    m_StateInitiatedByMouseDown = CSCROLLBAR_STATE_NORMAL;

    m_Autosize = autosize;
    m_SmallStep = 1;
    m_LargeStep = 10;
    m_LeftMouseButtonPressed = false;

    m_MinValue = minValue;
    if (maxValue > minValue)
    {
        m_MaxValue = maxValue;
    }
    else
    {
        m_MaxValue = minValue + 1;
    }
    if (thumbPosition >= m_MinValue && thumbPosition <= m_MaxValue)
    {
        m_ThumbPosition = thumbPosition;
    }
    else
    {
        m_ThumbPosition = m_MinValue;
    }

    // w konstruktorze ustawiamy minimalną wielkość rozmiaru suwaka
    m_ThumbSize = 0;
    
    int delta = m_MaxValue - m_MinValue;
    if (m_LargeStep >= delta)
    {
        m_LargeStep = delta / 2;
    }
}

Tilc::Gui::TScrollBar::~TScrollBar()
{
    m_ControlsToNotify.clear();
    m_WindowsToNotify.clear();
}

void Tilc::Gui::TScrollBar::RegisterPositionNotificationForControl(Tilc::Gui::TGuiControl* control)
{
    m_ControlsToNotify.push_back(control);
}

void Tilc::Gui::TScrollBar::RegisterPositionNotificationForWindow(Tilc::Gui::TStyledWindow* window)
{
    m_WindowsToNotify.push_back(window);
}

void Tilc::Gui::TScrollBar::SetMinValue(int value)
{
    m_MinValue = value;
    SetThumbSize();
}

void Tilc::Gui::TScrollBar::SetMaxValue(int value)
{
    m_MaxValue = value;
    SetThumbSize();
}

void Tilc::Gui::TScrollBar::OnPositionChanged(int oldPosition)
{
    if (!OnPositionChangeCallback && m_ControlsToNotify.empty())
    {
        return;
    }

    Tilc::Gui::TGuiControl* control;
    Tilc::Gui::TStyledWindow* window;

    // wysyłamy powiadomienia do kontrolek
    for (auto it = m_ControlsToNotify.begin(); it != m_ControlsToNotify.end(); ++it)
    {
        (*it)->OnThumbChange(oldPosition, m_ThumbPosition, this);
    }

    if (OnPositionChangeCallback)
    {
        OnPositionChangeCallback(nullptr);
    }
}

int Tilc::Gui::TScrollBar::CalculatePositionForStep(int step)
{
    int position = m_ThumbPosition;

    if (step < 0)
    {
        if (position + step < m_MinValue)
        {
            position = m_MinValue;
        }
        else
        {
            position += step;
        }
    }
    else
    {
        if (position + step > m_MaxValue)
        {
            position = m_MaxValue;
        }
        else
        {
            position += step;
        }
    }

    return position;
}

void Tilc::Gui::TScrollBar::StepBy(int step, bool redraw)
{
    if (step == 0)
    {
        return;
    }

    int oldPosition = m_ThumbPosition;
    m_ThumbPosition = CalculatePositionForStep(step);

    if (m_ThumbPosition != oldPosition)
    {
        OnPositionChanged(oldPosition);
        if (redraw)
        {
            Invalidate();
        }
    }
}

void Tilc::Gui::TScrollBar::SetPosition(int position, bool fireEvent, bool redraw)
{
    //SDL_Log("SetPosition to: %d", position);
    if (m_ThumbPosition == position)
    {
        return;
    }

    if (position < m_MinValue)
    {
        position = m_MinValue;
    }
    if (position > m_MaxValue)
    {
        position = m_MaxValue;
    }

    int oldPosition = m_ThumbPosition;
    m_ThumbPosition = position;

    if (fireEvent)
    {
        OnPositionChanged(oldPosition);
    }
    Invalidate();
}

void Tilc::Gui::TScrollBar::SetSteps(int _small, int _large)
{
    m_SmallStep = _small;
    m_LargeStep = _large;
}
