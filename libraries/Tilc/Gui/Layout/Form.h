#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"


namespace Tilc {
    namespace Gui {
        class TLayoutFile;
        class TGuiControl;
        class TStyledWindow;


        class DECLSPEC TForm
        {
        public:
            TForm() {};
            virtual ~TForm() {};
            // FormFields ma postać np:
            // { {"Label string 1", "Edit control name 1", "Edit control type name 1"}, {"Label string 2", "Edit control name 2"}, "Edit control type name 2"}, ... }
            static void CreateForm(TStyledWindow* Window, std::initializer_list<std::initializer_list<const char*>> FormFields, float TopY = 0.0f);
        };
    }
}
