#include "Tilc/Gui/Layout/JsonLayoutFile.h"
#include "Tilc/Gui/Label.h"
#include "Tilc/Gui/TextField.h"
#include "Tilc/Gui/Grid.h"
#include "Tilc/Gui/Button.h"
#include "Tilc/Gui/Checkbox.h"
#include "Tilc/Gui/Listbox.h"
#include "Tilc/Gui/MessageBox.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Utils/FileUtils.h"
#include "Tilc/Utils/JsonParser.h"

Tilc::Gui::TJsonLayoutFile::TJsonLayoutFile()
{
}

Tilc::Gui::TJsonLayoutFile::~TJsonLayoutFile()
{
}

bool Tilc::Gui::TJsonLayoutFile::loadLayoutFromFile(const Tilc::TExtString& fname, int layoutContentType, Tilc::Gui::TStyledWindow* wnd)
{
    bool result = false;

    if (!wnd)
    {
        return false;
    }
    if (!Tilc::FileExists(fname))
    {
        return false;
    }

    Tilc::TStdObject data;
    if (!data.loadFromFile(fname))
    {
        return false;
    }

    m_Controls.clear();
    m_Wnd = wnd;
    m_Filename = fname;
    processData(&data);

    m_Controls.clear();
    return true;
}

bool Tilc::Gui::TJsonLayoutFile::loadLayout(const Tilc::TExtString& layoutContent, int layoutContentType, Tilc::Gui::TStyledWindow* wnd)
{
    bool result = false;

    if (!wnd)
    {
        return false;
    }

    Tilc::TJsonParser parser;
    Tilc::TStdObject* data = parser.parse(layoutContent);
    if (!data)
    {
        if (!parser.isError())
        {
            AlertBox("No json objects found.");
        }
        else
        {
            ErrorBox(parser.getErrorMessage());
        }
        return false;
    }

    m_Controls.clear();
    m_Wnd = wnd;
    m_Filename = "";
    processData(data);

    m_Controls.clear();
    delete data;
    return true;
}
