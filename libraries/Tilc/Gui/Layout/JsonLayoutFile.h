#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Gui/Layout/LayoutFile.h"
#include <list>


namespace Tilc {
    namespace Gui {
        class DECLSPEC TJsonLayoutFile : public TLayoutFile
        {
        public:
            TJsonLayoutFile();
            virtual ~TJsonLayoutFile();

            virtual bool loadLayoutFromFile(const Tilc::TExtString& fname, Tilc::Gui::TStyledWindow* wnd) override;
            virtual bool loadLayout(const Tilc::TExtString& layoutContent, Tilc::Gui::TStyledWindow* wnd) override;
        };
    }
}
