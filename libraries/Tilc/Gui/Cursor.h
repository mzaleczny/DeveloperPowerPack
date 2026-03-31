#pragma once

#include "Tilc/DllGlobals.h"
#include "SDL3/SDL.h"

namespace Tilc {
    namespace Gui {

        constexpr const int CURSOR_TYPE_NORMAL = 1;
        constexpr const int CURSOR_TYPE_BEAM = 2;
        constexpr const int CURSOR_TYPE_WAIT = 3;
        constexpr const int CURSOR_TYPE_CROSS = 4;
        constexpr const int CURSOR_TYPE_ARROW_UP = 5;
        constexpr const int CURSOR_TYPE_SIZE = 6;
        constexpr const int CURSOR_TYPE_SIZE_ALL = 7;
        constexpr const int CURSOR_TYPE_NWSE = 8;
        constexpr const int CURSOR_TYPE_NESW = 9;
        constexpr const int CURSOR_TYPE_WE = 10;
        constexpr const int CURSOR_TYPE_NS = 11;
        constexpr const int CURSOR_TYPE_WIN_LOGO = 12;
        constexpr const int CURSOR_TYPE_NO = 13;
        constexpr const int CURSOR_TYPE_HAND = 14;
        constexpr const int CURSOR_TYPE_APP_STARTING = 15;
        constexpr const int CURSOR_TYPE_HELP = 16;

        class DECLSPEC TCursor {
        public:
            TCursor();
            virtual ~TCursor();

            inline void SetNormalCursor();
            inline void SetIBeamCursor();
            inline void SetWaitCursor();
            inline void SetCrossCursor();
            inline void SetArrowUpCursor();
            inline void SetSizeCursor();
            inline void SetSizeAllCursor();
            inline void SetSizeNWSECursor();
            inline void SetSizeNESWCursor();
            inline void SetSizeWECursor();
            inline void SetSizeNSCursor();
            inline void SetWinLogoCursor();
            inline void SetNoCursor();
            inline void SetHandCursor();
            inline void SetAppStartingCursor();
            inline void SetHelpCursor();

            void Destroy();
            SDL_Cursor* LoadCursorFromResource(const char* image[]);

        protected:
            // uchwyt na aktualnie wyświetlany kursor
            SDL_Cursor* m_Cursor;
            int m_Type;
        };

    }
}
