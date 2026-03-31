#include "Tilc/Gui/Cursor.h"
#include "Tilc/Gui/CursorResources.h"
#include <stdio.h>

Tilc::Gui::TCursor::TCursor()
{
    SetNormalCursor();
}

Tilc::Gui::TCursor::~TCursor()
{
    Destroy();
}

void Tilc::Gui::TCursor::Destroy()
{
    if (m_Cursor)
    {
        SDL_DestroyCursor(m_Cursor);
        m_Cursor = nullptr;
    }
}

void Tilc::Gui::TCursor::SetNormalCursor()
{
    m_Type = CURSOR_TYPE_NORMAL;
    m_Cursor = LoadCursorFromResource(CursorResourceArrow);
}

void Tilc::Gui::TCursor::SetIBeamCursor()
{
    m_Type = CURSOR_TYPE_BEAM;
    m_Cursor = LoadCursorFromResource(CursorResourceBeam);
}

void Tilc::Gui::TCursor::SetWaitCursor()
{
    m_Type = CURSOR_TYPE_WAIT;
    m_Cursor = LoadCursorFromResource(CursorResourceWait);
}

void Tilc::Gui::TCursor::SetCrossCursor()
{
    m_Type = CURSOR_TYPE_CROSS;
    m_Cursor = LoadCursorFromResource(CursorResourceCross);
}

void Tilc::Gui::TCursor::SetArrowUpCursor()
{
    m_Type = CURSOR_TYPE_ARROW_UP;
    m_Cursor = LoadCursorFromResource(CursorResourceArrowUp);
}

void Tilc::Gui::TCursor::SetSizeCursor()
{
    m_Type = CURSOR_TYPE_SIZE;
    m_Cursor = LoadCursorFromResource(CursorResourceSize);
}

void Tilc::Gui::TCursor::SetSizeAllCursor()
{
    m_Type = CURSOR_TYPE_SIZE_ALL;
    m_Cursor = LoadCursorFromResource(CursorResourceSizeAll);
}

void Tilc::Gui::TCursor::SetSizeNWSECursor()
{
    m_Type = CURSOR_TYPE_NWSE;
    m_Cursor = LoadCursorFromResource(CursorResourceNWSE);
}

void Tilc::Gui::TCursor::SetSizeNESWCursor()
{
    m_Type = CURSOR_TYPE_CROSS;
    m_Cursor = LoadCursorFromResource(CursorResourceCross);
}

void Tilc::Gui::TCursor::SetSizeWECursor()
{
    m_Type = CURSOR_TYPE_WE;
    m_Cursor = LoadCursorFromResource(CursorResourceWE);
}

void Tilc::Gui::TCursor::SetSizeNSCursor()
{
    m_Type = CURSOR_TYPE_NS;
    m_Cursor = LoadCursorFromResource(CursorResourceNS);
}

void Tilc::Gui::TCursor::SetWinLogoCursor()
{
    m_Type = CURSOR_TYPE_WIN_LOGO;
    m_Cursor = LoadCursorFromResource(CursorResourceWinLogo);
}

void Tilc::Gui::TCursor::SetNoCursor()
{
    m_Type = CURSOR_TYPE_NO;
    m_Cursor = LoadCursorFromResource(CursorResourceNo);
}

void Tilc::Gui::TCursor::SetHandCursor()
{
    m_Type = CURSOR_TYPE_HAND;
    m_Cursor = LoadCursorFromResource(CursorResourceHand);
}

void Tilc::Gui::TCursor::SetAppStartingCursor()
{
    m_Type = CURSOR_TYPE_APP_STARTING;
    m_Cursor = LoadCursorFromResource(CursorResourceAppStarting);
}

void Tilc::Gui::TCursor::SetHelpCursor()
{
    m_Type = CURSOR_TYPE_HELP;
    m_Cursor = LoadCursorFromResource(CursorResourceHelp);
}


SDL_Cursor* Tilc::Gui::TCursor::LoadCursorFromResource(const char* image[])
{
    int i, row, col;
    Uint8 data[4 * 32];
    Uint8 mask[4 * 32];
    int hot_x, hot_y;

    if (!image) return nullptr;

    i = -1;
    for (row = 0; row < 32; ++row)
    {
        for (col = 0; col < 32; ++col)
        {
            if (col % 8)
            {
                data[i] <<= 1;
                mask[i] <<= 1;
            }
            else
            {
                ++i;
                data[i] = mask[i] = 0;
            }
            switch (image[4 + row][col])
            {
            case 'X':
                data[i] |= 0x01;
                mask[i] |= 0x01;
                break;
            case '.':
                mask[i] |= 0x01;
                break;
            case ' ':
                break;
            }
        }
    }
    sscanf(image[4 + row], "%d,%d", &hot_x, &hot_y);
    return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}
