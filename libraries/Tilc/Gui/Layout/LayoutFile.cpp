#include "Tilc/Gui/Layout/LayoutFile.h"
#include "Tilc/Gui/Label.h"
#include "Tilc/Gui/TextField.h"
#include "Tilc/Gui/Grid.h"
#include "Tilc/Gui/Button.h"
#include "Tilc/Gui/Checkbox.h"
#include "Tilc/Gui/Listbox.h"
#include "Tilc/Gui/MessageBox.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Utils/FileUtils.h"
#include <cstring>

Tilc::Gui::TLayoutFile::TLayoutFile()
{
}

Tilc::Gui::TLayoutFile::~TLayoutFile()
{
}

Tilc::Gui::TGuiControl* Tilc::Gui::TLayoutFile::getControl(const char* name)
{
    size_t size = m_Controls.size();
    for (auto it = m_Controls.begin(); it != m_Controls.end(); ++it)
    {
        if (*it && (*it)->GetName() == name)
        {
            return *it;
        }
    }

    return nullptr;
}

void Tilc::Gui::TLayoutFile::processData(Tilc::TStdObject* data)
{
    Tilc::TStdObjectProperty* root = data->get("root");
    Tilc::TStdObject* item;

    if (!root)
    {
        return;
    }

    if (root->type == Tilc::PROPERTY_OBJECT && root->oValue)
    {
        Tilc::TPropertiesVector* TopLevelItems = root->oValue->getAsArray("TopLevelItems");
        if (TopLevelItems)
        {
            processToplevelItems(TopLevelItems);
        }
    }
}

void Tilc::Gui::TLayoutFile::processToplevelItems(Tilc::TPropertiesVector* items)
{
    if (!items)
    {
        return;
    }

    size_t size = items->size();
    Tilc::TStdObjectProperty* item;
    Tilc::TStdObject* oValue;
    for (size_t i = 0; i < size; ++i)
    {
        item = (*items)[i];
        if (item || item->type == PROPERTY_OBJECT)
        {
            oValue = item->oValue;
            if (oValue)
            {
                if (oValue->getAsString("type") == "window" && oValue->getAsString("name") == m_Wnd->GetName())
                {
                    processWindowItem(oValue);
                    // we process only one window
                    break;
                }
            }
        }
    }
}

void Tilc::Gui::TLayoutFile::processItems(Tilc::TPropertiesVector* items, Tilc::Gui::TGuiControl* parent)
{
    if (!items)
    {
        return;
    }

    size_t size = items->size();
    Tilc::TStdObjectProperty* item;
    Tilc::TStdObject* oValue;
    for (size_t i = 0; i < size; i++)
    {
        item = (*items)[i];
        if (item || item->type == PROPERTY_OBJECT)
        {
            oValue = item->oValue;
            if (oValue)
            {
                Tilc::TExtString type = oValue->getAsString("type");
                if (type == "textfield")
                {
                    processTextfieldItem(oValue, parent);
                    continue;
                }
                if (type == "button")
                {
                    processButtonItem(oValue, parent);
                    continue;
                }
                if (type == "label")
                {
                    processLabelItem(oValue, parent);
                    continue;
                }
                if (type == "checkbox")
                {
                    processCheckboxItem(oValue, parent);
                    continue;
                }
                if (type == "listbox")
                {
                    processListboxItem(oValue, parent);
                    continue;
                }
                if (type == "grid")
                {
                    processGridItem(oValue, parent);
                    continue;
                }
                if (type == "menu")
                {
                    processMenuItem(oValue, parent);
                    continue;
                }
            }
        }
    }
}

void Tilc::Gui::TLayoutFile::processWindowItem(Tilc::TStdObject* item)
{
    float x, y, width, height;
    Tilc::TStdObjectProperty* property;
    Tilc::TExtString activeControl;
    Tilc::TExtString caption;
    bool allowResizing = true ;

    getDimensionProperties(item, &x, &y, &width, &height);
    property = item->get("active-control");
    if (property && property->type == PROPERTY_VALUE)
    {
        activeControl = property->value;
    }
    allowResizing = item->getAsString("allow-resizing") != "false";
    caption = item->getAsString("caption");
    
    property = item->get("items");
    if (property && property->type == PROPERTY_ARRAY)
    {
        Tilc::TPropertiesVector* items = property->aValue;
        if (items)
        {
            processItems(items, m_Wnd);
            // now activate default active control
            Tilc::Gui::TGuiControl* gc = getControl(activeControl.c_str());
            if (gc && gc->IsVisible() && gc->IsEnabled() && gc->CanTabStop())
            {
                m_Wnd->SetActiveControl(gc);
            }
        }
    }
    
    if (width != std::numeric_limits<int>::max() && height != std::numeric_limits<int>::max())
    {
        m_Wnd->SetSize(width, height);
    }
    
    if (x != std::numeric_limits<int>::max() && y != std::numeric_limits<int>::max())
    {
        m_Wnd->SetPosition(x, y);
    }
    
    m_Wnd->AllowResizing(allowResizing);
    if (!caption.empty())
    {
        m_Wnd->SetText(caption);
    }
}

void Tilc::Gui::TLayoutFile::processMenuItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent)
{
    if (!item || !parent)
    {
        return;
    }

    Tilc::TExtString name = item->getAsString("name");
    if (!name.empty())
    {
        Tilc::TExtString menuItemsFile = item->getAsString("menu-items-file");
        if (!menuItemsFile.empty())
        {
            if (!FileExists(menuItemsFile))
            {
                /*
                Tilc::TExtString resDir = ::application->getResourcesDir();
                if (resDir != COMMON_EMPTY_STRING) {
                    menuItemsFile = resDir + L"\\strings\\" + menuItemsFile;
                    if (FileExists(menuItemsFile)) {
                        CMenu *gc = new CMenu(parent, name);
                        if (gc) {
                            gc->loadMenuFromFile(menuItemsFile);
                            this->addControl(gc, parent);
                        }
                    }
                }
                */
            }
        }
    }
}

void Tilc::Gui::TLayoutFile::processLabelItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent)
{
    if (!item || !parent)
    {
        return;
    }

    Tilc::TExtString name = item->getAsString("name");
    if (!name.empty())
    {
        Tilc::TExtString caption, relativeControl, relativePosition;
        float x, y, width, height;
        bool autosize = false;
        Tilc::TStdObjectProperty* property;

        getDimensionProperties(item, &x, &y, &width, &height);
        getRelativePositionProperties(item, &relativeControl, &relativePosition);
        caption = item->getAsString("caption");
        
        property = item->get("autosize");
        if (property && property->type == PROPERTY_VALUE && property->value == "true")
        {
            autosize = true;
        }

        SDL_FRect Position = { x, y, width, height };
        Tilc::Gui::TLabel* gc = new Tilc::Gui::TLabel(parent, name, Position, caption, autosize);
        if (gc)
        {
            doRelativePositioning(gc, relativeControl.c_str(), relativePosition.c_str());
            addControl(gc, parent);
        }
    }
}

void Tilc::Gui::TLayoutFile::processTextfieldItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent)
{
    if (!item || !parent)
    {
        return;
    }

    Tilc::TExtString name = item->getAsString("name");
    if (!name.empty())
    {
        Tilc::TExtString text, relativeControl, relativePosition;
        float x, y, width, height;
        bool enterAsTab = true , canTabStop = true , disableTabkey = false;

        getDimensionProperties(item, &x, &y, &width, &height);
        getRelativePositionProperties(item, &relativeControl, &relativePosition);
        text = item->getAsString("text");
        enterAsTab = item->getAsString("enter-as-tab") != "false";
        canTabStop = item->getAsString("can-tab-stop") != "false";
        disableTabkey = item->getAsString("disable-tab-key") == "true";

        SDL_FRect Position = { x, y, width, -1 };
        Tilc::Gui::TTextField* gc = new Tilc::Gui::TTextField(parent, name, Position);
        if (gc)
        {
            doRelativePositioning(gc, relativeControl.c_str(), relativePosition.c_str());
            gc->SetTabStop(canTabStop);
            if (disableTabkey)
            {
                gc->DisableTabkey();
            }
            gc->DoEnterAsTabkey(enterAsTab);
            
            this->addControl(gc, parent);
        }
    }
}

void Tilc::Gui::TLayoutFile::processGridItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent)
{
    if (!item || !parent)
    {
        return;
    }

    Tilc::TExtString name = item->getAsString("name");
    if (!name.empty())
    {
        Tilc::TExtString relativeControl, relativePosition;
        float x, y, width, height;
        int columnCount = 10, rowCount = 10;
        bool enterAsTab = true, canTabStop = true, disableTabkey = false;
        bool entireRowSelect = false, entireColumnSelect = false;
        bool showTopHeader = true, showLeftHeader = true;
        bool drawVerticalLines = true, drawHorizontalLines = true;
        int leftHeaderWidth = 0;

        getDimensionProperties(item, &x, &y, &width, &height);
        getRelativePositionProperties(item, &relativeControl, &relativePosition);
        columnCount = item->getAsInt("columnCount");
        rowCount = item->getAsInt("rowCount");
        canTabStop = item->getAsString("can-tab-stop") != "false";
        disableTabkey = item->getAsString("disable-tab-key") == "true";
        entireRowSelect = item->getAsString("entire-row-select") == "true";
        entireColumnSelect = item->getAsString("entire-column-select") == "true";
        showTopHeader = item->getAsString("show-top-header") != "false";
        showLeftHeader = item->getAsString("show-left-header") != "false";
        drawVerticalLines = item->getAsString("draw-vertical-lines") != "false";
        drawHorizontalLines = item->getAsString("draw-horizontal-lines") != "false";
        leftHeaderWidth = item->getAsInt("left-header-width");

        if (!columnCount)
        {
            columnCount = 10;
        }
        if (!rowCount)
        {
            rowCount = 10;
        }

        SDL_FRect Position = { x, y, width, height };
        Tilc::Gui::TGrid* gc = new Tilc::Gui::TGrid(parent, name, Position, columnCount, rowCount, true,
                              showLeftHeader, showTopHeader, entireRowSelect, entireColumnSelect,
                              drawVerticalLines, drawHorizontalLines);
        if (gc)
        {
            Tilc::TStringVector captions;
            
            getAsStringList(item, "top-header-captions", captions);
            if (captions.size() > 0)
            {
                gc->SetTopHeaderCaptions(&captions);
            }
            
            getAsStringList(item, "left-header-captions", captions);
            if (captions.size() > 0)
            {
                gc->SetLeftHeaderCaptions(&captions);
            }
            
            if (leftHeaderWidth > 0)
            {
                gc->SetLeftHeaderWidth(leftHeaderWidth);
            }
            doRelativePositioning(gc, relativeControl.c_str(), relativePosition.c_str());            
            addControl(gc, parent);
        }
    }
}

void Tilc::Gui::TLayoutFile::processButtonItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent)
{
    if (!item || !parent)
    {
        return;
    }

    Tilc::TExtString name = item->getAsString("name");
    if (!name.empty())
    {
        Tilc::TExtString text, relativeControl, relativePosition;
        float x, y, width, height;
        bool canTabStop, disableTabkey, disabled;
        bool transparentDrawing;
        
        getCommonProperties(item, &transparentDrawing);
        getDimensionProperties(item, &x, &y, &width, &height);
        getRelativePositionProperties(item, &relativeControl, &relativePosition);
        text = item->getAsString("text");
        canTabStop = item->getAsString("can-tab-stop") != "false";
        disableTabkey = item->getAsString("disable-tab-key") == "true";
        disabled = item->getAsString("disabled") == "true";

        SDL_FRect Position = { x, y, width, -1 };

        Tilc::Gui::TButton* gc = new Tilc::Gui::TButton(parent, name, Position, text);
        if (gc)
        {
            //gc->SetTransparentDrawing(transparentDrawing);
            doRelativePositioning(gc, relativeControl.c_str(), relativePosition.c_str());
            if (disabled)
            {
                gc->Disable();
            }
        }
    }
}

void Tilc::Gui::TLayoutFile::processCheckboxItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent)
{
    if (!item || !parent) {
        return;
    }

    Tilc::TExtString name = item->getAsString("name");
    if (!name.empty())
    {
        Tilc::TExtString text, relativeControl, relativePosition, checkedStateText, uncheckedStateText, textPlacementStr;
        float x, y, width, height;
        int textSpacing, textPlacement;
        bool canTabStop, disableTabkey, disabled, checked;
        bool transparentDrawing;
        
        getCommonProperties(item, &transparentDrawing);
        getDimensionProperties(item, &x, &y, &width, &height);
        getRelativePositionProperties(item, &relativeControl, &relativePosition);
        text = item->getAsString("text");
        checkedStateText = item->getAsString("checked-state-text");
        uncheckedStateText = item->getAsString("unchecked-state-text");
        canTabStop = item->getAsString("can-tab-stop") != "false";
        disableTabkey = item->getAsString("disable-tab-key") == "true";
        disabled = item->getAsString("disabled") == "true";
        checked = item->getAsString("checked") == "true";
        textSpacing = item->getAsInt("text-spacing");
        textPlacementStr = item->getAsString("text-placement");

        if (!text.empty())
        {
            checkedStateText = text;
            uncheckedStateText = text;
        }
        if (textPlacementStr == "left")
        {
            textPlacement = Tilc::Gui::CONTROL_TEXT_PLACEMENT_LEFT;
        }
        else
        {
            textPlacement = CONTROL_TEXT_PLACEMENT_RIGHT;
        }
        if (textSpacing == 0)
        {
            textSpacing = CONTROL_DEFAULT_SPACING;
        }

        SDL_FRect Position = { x, y, width, -1 };
        Tilc::Gui::TCheckbox* gc = new Tilc::Gui::TCheckbox(parent, name, Position, checkedStateText, uncheckedStateText, checked, textPlacement, textSpacing);
        if (gc)
        {
            //gc->SetTransparentDrawing(transparentDrawing);
            doRelativePositioning(gc, relativeControl.c_str(), relativePosition.c_str());            
            if (disabled)
            {
                gc->Disable();
            }
            addControl(gc, parent);
        }
    }
}

void Tilc::Gui::TLayoutFile::processListboxItem(Tilc::TStdObject* item, Tilc::Gui::TGuiControl* parent)
{
    if (!item || !parent)
    {
        return;
    }

    Tilc::TExtString name = item->getAsString("name");
    if (!name.empty())
    {
        Tilc::TExtString text, relativeControl, relativePosition;
        float x, y, width, height;
        bool canTabStop, disableTabkey, disabled;
        bool transparentDrawing;
        
        getCommonProperties(item, &transparentDrawing);
        getDimensionProperties(item, &x, &y, &width, &height);
        getRelativePositionProperties(item, &relativeControl, &relativePosition);
        canTabStop = item->getAsString("can-tab-stop") != "false";
        disableTabkey = item->getAsString("disable-tab-key") == "true ";
        disabled = item->getAsString("disabled") == "true";

        SDL_FRect Position = { x, y, width, height };
        Tilc::Gui::TListbox* gc = new Tilc::Gui::TListbox(parent, name, Position, {});
        if (gc)
        {
            Tilc::TStringVector lst;
            getAsStringList(item, "items", lst);
            if (lst.size() > 0)
            {
                gc->SetItems(lst);
            }
            //gc->SetTransparentDrawing(transparentDrawing);
            doRelativePositioning(gc, relativeControl.c_str(), relativePosition.c_str());
            if (disabled)
            {
                gc->Disable();
            }
            addControl(gc, parent);
        }
    }
}

void Tilc::Gui::TLayoutFile::addControl(Tilc::Gui::TGuiControl* gc, Tilc::Gui::TGuiControl* parent)
{
    parent->AddChild(gc);
    m_Controls.push_back(gc);
}

void Tilc::Gui::TLayoutFile::getCommonProperties(Tilc::TStdObject* item, bool* transparentDrawing)
{
    // we set MAXLONG values
    *transparentDrawing = false;
    Tilc::TExtString transparent = item->getAsString("transparent-drawing");
    if (transparent.ToLowercase() == "true")
    {
        *transparentDrawing = true;
    }
}

void Tilc::Gui::TLayoutFile::getDimensionProperties(Tilc::TStdObject* item, float* x, float* y, float* width, float* height)
{
    // we set MAXLONG values
    *x = std::numeric_limits<int>::max();
    *y = std::numeric_limits<int>::max();
    *width = std::numeric_limits<int>::max();
    *height = std::numeric_limits<int>::max();
    int iValue;
    Tilc::TExtString sValue;
    Tilc::TStdObjectProperty* property;

    property = item->get("width");
    if (property)
    {
        if (property->type == PROPERTY_VALUE_INT || property->type == PROPERTY_VALUE_DOUBLE)
        {
            *width = item->getAsDouble("width");
        }
    }

    property = item->get("height");
    if (property)
    {
        if (property->type == PROPERTY_VALUE_INT || property->type == PROPERTY_VALUE_DOUBLE)
        {
            *height = item->getAsDouble("height");
        }
    }

    property = item->get("x");
    if (property)
    {
        if (property->type == PROPERTY_VALUE_INT || property->type == PROPERTY_VALUE_DOUBLE)
        {
            *x = item->getAsDouble("x");
        }
        else if (property->type == PROPERTY_VALUE)
        {
            sValue = property->value;
            if (sValue == "center")
            {
                *x = (m_Wnd->m_RealPosition.w - *width) / 2;
            }
        }
    }

    property = item->get("y");
    if (property)
    {
        if (property->type == PROPERTY_VALUE_INT || property->type == PROPERTY_VALUE_DOUBLE)
        {
            *y = item->getAsDouble("y");
        }
        else if (property->type == PROPERTY_VALUE)
        {
            sValue = property->value;
            if (sValue == "center")
            {
                *y = (m_Wnd->m_RealPosition.h - *height) / 2;
            }
        }
    }
}

void Tilc::Gui::TLayoutFile::getRelativePositionProperties(Tilc::TStdObject* item, Tilc::TExtString* relativeControl, Tilc::TExtString* relativePosition)
{
    *relativeControl = item->getAsString("position-relative-to");
    *relativePosition = item->getAsString("position");
}

void Tilc::Gui::TLayoutFile::doRelativePositioning(Tilc::Gui::TGuiControl* gc, const char* positionRelativeTo, const char* position)
{
    Tilc::Gui::TGuiControl* rel = this->getControl(positionRelativeTo);
    if (rel)
    {
        if (std::strcmp(position, "below") == 0)
        {
            gc->SetPosition(rel->m_RealPosition.x, rel->m_RealPosition.y + rel->m_RealPosition.h + 4);
        }
        else if (std::strcmp(position, "right") == 0)
        {
            gc->SetPosition(rel->m_RealPosition.x + rel->m_RealPosition.w + 4, rel->m_RealPosition.y);
        }
    }
}

void Tilc::Gui::TLayoutFile::getAsStringList(Tilc::TStdObject* item, const char* name, Tilc::TStringVector& Result)
{
    Result.clear();
    if (item)
    {
        Tilc::TStdObjectProperty* prop = item->get(name);
        if (prop && prop->type == PROPERTY_ARRAY)
        {
            Tilc::TPropertiesVector* lst = prop->aValue;
            if (lst && lst->size() > 0)
            {
                for (size_t i = 0; i < lst->size(); ++i)
                {
                    prop = (*lst)[i];
                    if (prop && (prop->type == PROPERTY_VALUE || prop->type == PROPERTY_VALUE_INT || prop->type == PROPERTY_VALUE_DOUBLE))
                    {
                        Result.push_back(prop->getAsString());
                    }
                }
            }
        }
    }
}
