#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"

namespace Tilc
{
    namespace Gui {

        DECLSPEC int MessageBox(Tilc::TExtString caption, Tilc::TExtString message);
        DECLSPEC int AlertBox(Tilc::TExtString message);
        DECLSPEC int WarningBox(Tilc::TExtString message);
        DECLSPEC int QuestionBox(Tilc::TExtString message);
        DECLSPEC int QuestionCancelBox(Tilc::TExtString message);
        DECLSPEC int ErrorBox(Tilc::TExtString message);
        DECLSPEC int InformationBox(Tilc::TExtString message);
        DECLSPEC bool IsMessageBoxOpened();

        DECLSPEC void ShowSystemError(const Tilc::TExtString PrependString = "");

    }
}
