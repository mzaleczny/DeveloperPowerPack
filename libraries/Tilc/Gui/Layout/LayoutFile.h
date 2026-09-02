#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/StdObject.h"
#include "Tilc/Gui/PanelMenu.h"
#include <list>


namespace Tilc {
    namespace Gui {
        class TLayoutFile;
        class TGuiControl;
        class TStyledWindow;


        class DECLSPEC TLayoutFile
        {
        public:
            TLayoutFile();
            virtual ~TLayoutFile();

            virtual bool loadLayoutFromFile(const Tilc::TExtString& fname, Tilc::Gui::TStyledWindow* wnd) = 0;
            virtual bool loadLayout(const Tilc::TExtString& layoutContent, Tilc::Gui::TStyledWindow* wnd) = 0;
            void addControl(Tilc::Gui::TGuiControl* gc, Tilc::Gui::TGuiControl* parent);
            void getCommonProperties(Tilc::TStdObject* item, bool* transparentDrawing);
            void getDimensionProperties(Tilc::TStdObject* item, float* x, float* y, float* width, float* height);
            void getRelativePositionProperties(Tilc::TStdObject* item, Tilc::TExtString* relativeControl, Tilc::TExtString* relativePosition);
            void doRelativePositioning(Tilc::Gui::TGuiControl* gc, const char* positionRelativeTo, const char* position);

        protected:
            // auxiliary array of controls to add to window
            std::list< Tilc::Gui::TGuiControl*> m_Controls;
            Tilc::Gui::TGuiControl* getControl(const char* name);
            // Variable valid only during call to drawJsonFile method.
            // It is set to canvas pointer and NULL-ed just before drawJsonFile method return.
            Tilc::Gui::TStyledWindow* m_Wnd{};
            Tilc::TExtString m_Filename;

            void getAsStringList(Tilc::TStdObject* item, const char* name, Tilc::TStringVector& Result);
            void processData(Tilc::TStdObject* data);
            void processToplevelItems(Tilc::TPropertiesVector* items);
            void processItems(Tilc::TPropertiesVector* items, Tilc::Gui::TGuiControl* parent);
            void processWindowItem(Tilc::TStdObject* item);
            void processMenuItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent);
            void processLabelItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent);
            void processTextfieldItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent);
            void processMultilineTextfieldItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent);
            void processGridItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent);
            void processButtonItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent);
            void processCheckboxItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent);
            void processOptionItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent);
            void processListboxItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent);
            void processMultiColumnListboxItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent);
            void processSliderItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent);
            void processScrollbarItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent);
            void processPanelMenuItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent);
            Tilc::Gui::TPanelMenuItem processPanelMenuItem_ReadPanelMenuItem(Tilc::TStdObject* item);
        };
    }
}
