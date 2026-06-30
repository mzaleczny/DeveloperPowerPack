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
        SDL_SetCursor(nullptr);
        SDL_DestroyCursor(m_Cursor);
        m_Cursor = nullptr;
    }
}

void Tilc::Gui::TCursor::SetNormalCursor()
{
    if (m_Type != CURSOR_TYPE_NORMAL)
    {
        m_Type = CURSOR_TYPE_NORMAL;
        if (m_Cursor)
        {
            SDL_DestroyCursor(m_Cursor);
        }
        m_Cursor = LoadCursorFromResource(CursorResourceArrow);
        SDL_SetCursor(m_Cursor);
    }
}

void Tilc::Gui::TCursor::SetIBeamCursor()
{
    if (m_Type != CURSOR_TYPE_BEAM)
    {
        m_Type = CURSOR_TYPE_BEAM;
        if (m_Cursor)
        {
            SDL_DestroyCursor(m_Cursor);
        }
        m_Cursor = LoadCursorFromResource(CursorResourceBeam);
        SDL_SetCursor(m_Cursor);
    }
}

void Tilc::Gui::TCursor::SetWaitCursor()
{
    if (m_Type != CURSOR_TYPE_WAIT)
    {
        m_Type = CURSOR_TYPE_WAIT;
        if (m_Cursor)
        {
            SDL_DestroyCursor(m_Cursor);
        }
        m_Cursor = LoadCursorFromResource(CursorResourceWait);
        SDL_SetCursor(m_Cursor);
    }
}

void Tilc::Gui::TCursor::SetCrossCursor()
{
    if (m_Type != CURSOR_TYPE_CROSS)
    {
        m_Type = CURSOR_TYPE_CROSS;
        if (m_Cursor)
        {
            SDL_DestroyCursor(m_Cursor);
        }
        m_Cursor = LoadCursorFromResource(CursorResourceCross);
        SDL_SetCursor(m_Cursor);
    }
}

void Tilc::Gui::TCursor::SetArrowUpCursor()
{
    if (m_Type != CURSOR_TYPE_ARROW_UP)
    {
        m_Type = CURSOR_TYPE_ARROW_UP;
        if (m_Cursor)
        {
            SDL_DestroyCursor(m_Cursor);
        }
        m_Cursor = LoadCursorFromResource(CursorResourceArrowUp);
        SDL_SetCursor(m_Cursor);
    }
}

void Tilc::Gui::TCursor::SetSizeCursor()
{
    if (m_Type != CURSOR_TYPE_SIZE)
    {
        m_Type = CURSOR_TYPE_SIZE;
        if (m_Cursor)
        {
            SDL_DestroyCursor(m_Cursor);
        }
        m_Cursor = LoadCursorFromResource(CursorResourceSize);
        SDL_SetCursor(m_Cursor);
    }
}

void Tilc::Gui::TCursor::SetSizeAllCursor()
{
    if (m_Type != CURSOR_TYPE_SIZE_ALL)
    {
        m_Type = CURSOR_TYPE_SIZE_ALL;
        if (m_Cursor)
        {
            SDL_DestroyCursor(m_Cursor);
        }
        m_Cursor = LoadCursorFromResource(CursorResourceSizeAll);
        SDL_SetCursor(m_Cursor);
    }
}

void Tilc::Gui::TCursor::SetSizeNWSECursor()
{
    if (m_Type != CURSOR_TYPE_NWSE)
    {
        m_Type = CURSOR_TYPE_NWSE;
        if (m_Cursor)
        {
            SDL_DestroyCursor(m_Cursor);
        }
        m_Cursor = LoadCursorFromResource(CursorResourceNWSE);
        SDL_SetCursor(m_Cursor);
    }
}

void Tilc::Gui::TCursor::SetSizeNESWCursor()
{
    if (m_Type != CURSOR_TYPE_CROSS)
    {
        m_Type = CURSOR_TYPE_CROSS;
        if (m_Cursor)
        {
            SDL_DestroyCursor(m_Cursor);
        }
        m_Cursor = LoadCursorFromResource(CursorResourceCross);
        SDL_SetCursor(m_Cursor);
    }
}

void Tilc::Gui::TCursor::SetSizeWECursor()
{
    if (m_Type != CURSOR_TYPE_WE)
    {
        m_Type = CURSOR_TYPE_WE;
        if (m_Cursor)
        {
            SDL_DestroyCursor(m_Cursor);
        }
        m_Cursor = LoadCursorFromResource(CursorResourceWE);
        SDL_SetCursor(m_Cursor);
    }
}

void Tilc::Gui::TCursor::SetSizeNSCursor()
{
    if (m_Type != CURSOR_TYPE_NS)
    {
        m_Type = CURSOR_TYPE_NS;
        if (m_Cursor)
        {
            SDL_DestroyCursor(m_Cursor);
        }
        m_Cursor = LoadCursorFromResource(CursorResourceNS);
        SDL_SetCursor(m_Cursor);
    }
}

void Tilc::Gui::TCursor::SetWinLogoCursor()
{
    if (m_Type != CURSOR_TYPE_WIN_LOGO)
    {
        m_Type = CURSOR_TYPE_WIN_LOGO;
        if (m_Cursor)
        {
            SDL_DestroyCursor(m_Cursor);
        }
        m_Cursor = LoadCursorFromResource(CursorResourceWinLogo);
        SDL_SetCursor(m_Cursor);
    }
}

void Tilc::Gui::TCursor::SetNoCursor()
{
    if (m_Type != CURSOR_TYPE_NO)
    {
        m_Type = CURSOR_TYPE_NO;
        if (m_Cursor)
        {
            SDL_DestroyCursor(m_Cursor);
        }
        m_Cursor = LoadCursorFromResource(CursorResourceNo);
        SDL_SetCursor(m_Cursor);
    }
}

void Tilc::Gui::TCursor::SetHandCursor()
{
    if (m_Type != CURSOR_TYPE_HAND)
    {
        m_Type = CURSOR_TYPE_HAND;
        if (m_Cursor)
        {
            SDL_DestroyCursor(m_Cursor);
        }
        m_Cursor = LoadCursorFromResource(CursorResourceHand);
        SDL_SetCursor(m_Cursor);
    }
}

void Tilc::Gui::TCursor::SetAppStartingCursor()
{
    if (m_Type != CURSOR_TYPE_APP_STARTING)
    {
        m_Type = CURSOR_TYPE_APP_STARTING;
        if (m_Cursor)
        {
            SDL_DestroyCursor(m_Cursor);
        }
        m_Cursor = LoadCursorFromResource(CursorResourceAppStarting);
        SDL_SetCursor(m_Cursor);
    }
}

void Tilc::Gui::TCursor::SetHelpCursor()
{
    if (m_Type != CURSOR_TYPE_HELP)
    {
        m_Type = CURSOR_TYPE_HELP;
        if (m_Cursor)
        {
            SDL_DestroyCursor(m_Cursor);
        }
        m_Cursor = LoadCursorFromResource(CursorResourceHelp);
        SDL_SetCursor(m_Cursor);
    }
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
