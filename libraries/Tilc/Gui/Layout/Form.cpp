#include "Form.h"
#include <cstring>
#include "Tilc/Gui/GuiControl.h"
#include "Tilc/Gui/Label.h"
#include "Tilc/Gui/TextField.h"
#include "Tilc/Gui/MultilineTextField.h"
#include "Tilc/Gui/Grid.h"
#include "Tilc/Gui/Button.h"
#include "Tilc/Gui/Checkbox.h"
#include "Tilc/Gui/Option.h"
#include "Tilc/Gui/Listbox.h"
#include "Tilc/Gui/MultiColumnListbox.h"
#include "Tilc/Gui/MessageBox.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/SliderVertical.h"
#include "Tilc/Gui/SliderHorizontal.h"
#include "Tilc/Gui/ScrollBarVertical.h"
#include "Tilc/Gui/ScrollBarHorizontal.h"

void Tilc::Gui::TForm::CreateForm(TStyledWindow* Window, std::initializer_list<std::initializer_list<const char*>> FormFields)
{
    float const PaddingLeft = 10.0f;
    float const PaddingTop = 10.0f;
    float const Spacer = 15.0f;
    SDL_FRect Position{PaddingLeft, PaddingTop, Window->m_RealPosition.w, 25.0f };
    for (auto Item : FormFields)
    {
        const char* Label = *Item.begin();
        const char* Name = *(Item.begin()+1);
        const char* Type = *(Item.begin()+2);
        Tilc::Gui::TGuiControl* gc{};

        if (std::strncmp(Type, "label", 5) == 0)
        {
            Position.w = Window->m_RealPosition.w;
            gc = new Tilc::Gui::TLabel(Window, Name, Position, Label, false);
            Position.y += Position.h;
            Position.x = PaddingLeft;
            continue;
        }
        if (std::strncmp(Type, "textfield", 9) == 0)
        {
            Position.x = PaddingLeft;
            Position.w = Window->m_RealPosition.w;
            Position.h = 25.0f;
            Tilc::Gui::TLabel* lbl = new Tilc::Gui::TLabel(Window, Tilc::TExtString("lbl") + Name, Position, Label, false);
            if (lbl)
            {
                lbl->SetFontToUse("DefaultBold");
            }
            gc = lbl;
            Position.x = PaddingLeft;
            Position.y += 2*Position.h;
            Position.h = 25.0f;
            gc = new Tilc::Gui::TTextField(Window, Name, Position);
            Position.y += Position.h;
            continue;
        }
        if (std::strncmp(Type, "multiline-textfield", 19) == 0)
        {
            Position.w = Window->m_RealPosition.w;
            Tilc::Gui::TLabel* lbl = new Tilc::Gui::TLabel(Window, Tilc::TExtString("lbl") + Name, Position, Label, false);
            if (lbl)
            {
                lbl->SetFontToUse("DefaultBold");
            }
            gc = lbl;
            Position.y += Position.h;
            Position.x = PaddingLeft;
            gc = new Tilc::Gui::TMultilineTextField(Window, Name, Position);
            Position.y += Position.h;
            continue;
        }
        if (std::strncmp(Type, "button", 6) == 0)
        {
            Position.w = std::atoi(*(Item.begin() + 3));
            gc = new Tilc::Gui::TButton(Window, Name, Position, Label);
            Position.x += Position.w + Spacer;
            continue;
        }
        if (std::strncmp(Type, "checkbox", 8) == 0)
        {
            Position.w = std::atoi(*(Item.begin() + 3));
            gc = new Tilc::Gui::TCheckbox(Window, Name, Position, Label, Label, false);
            Position.x += Position.w + Spacer;
            continue;
        }
        if (std::strncmp(Type, "option", 6) == 0)
        {
            int GroupNumber = std::atoi(*(Item.begin() + 3));
            Position.w = std::atoi(*(Item.begin() + 4));
            gc = new Tilc::Gui::TOption(Window, Name, Position, Label, Label, false);
            Position.x += Position.w + Spacer;
            if (Tilc::Gui::TGuiControl::m_OptionGroups.find(GroupNumber) == Tilc::Gui::TGuiControl::m_OptionGroups.end())
            {
                Tilc::Gui::TGuiControl::m_OptionGroups[GroupNumber] = Tilc::Gui::TOptionGroup();
            }
            Tilc::Gui::TGuiControl::m_OptionGroups[GroupNumber].AddOptions({ dynamic_cast<Tilc::Gui::TOption*>(gc) });
            continue;
        }
        if (std::strncmp(Type, "listbox", 7) == 0)
        {
            Position.w = Window->m_RealPosition.w;
            Tilc::Gui::TLabel* lbl = new Tilc::Gui::TLabel(Window, Tilc::TExtString("lbl") + Name, Position, Label, false);
            if (lbl)
            {
                lbl->SetFontToUse("DefaultBold");
            }
            gc = lbl;
            Position.y += Position.h;
            Position.x = PaddingLeft;
            gc = new Tilc::Gui::TListbox(Window, Name, Position, {});
            Position.y += 5 * 25.0f;
            if (gc)
            {
                Tilc::Gui::TListbox* lb = dynamic_cast<Tilc::Gui::TListbox*>(gc);
                Tilc::TStringVector Items;
                for (auto it = Item.begin() + 3; it != Item.end(); ++it)
                {
                    Items.push_back(*it);
                }
                lb->SetItems(Items);
            }
            continue;
        }
        if (std::strncmp(Type, "multicolumn-listbox", 1) == 0)
        {
            continue;
        }
        if (std::strncmp(Type, "grid", 4) == 0)
        {
            Position.w = Window->m_RealPosition.w;
            Tilc::Gui::TLabel* lbl = new Tilc::Gui::TLabel(Window, Tilc::TExtString("lbl") + Name, Position, Label, false);
            if (lbl)
            {
                lbl->SetFontToUse("DefaultBold");
            }
            gc = lbl;
            Position.y += Position.h;
            Position.x = PaddingLeft;
            gc = new Tilc::Gui::TGrid(Window, Name, Position, 10, 10, true,
                true, true, false, false,
                true, true);
            Position.y += 10 * 30.0f;
            continue;
        }
        if (std::strncmp(Type, "slider", 6) == 0)
        {
            Position.w = Window->m_RealPosition.w;
            Tilc::Gui::TLabel* lbl = new Tilc::Gui::TLabel(Window, Tilc::TExtString("lbl") + Name, Position, Label, false);
            if (lbl)
            {
                lbl->SetFontToUse("DefaultBold");
            }
            gc = lbl;
            Position.y += Position.h;
            Position.x = PaddingLeft;
            const char* SliderType = *(Item.begin() + 3);
            if (std::strncmp(SliderType, "vertical", 8) == 0)
            {
                Position.h = 200.0f;
                gc = new Tilc::Gui::TSliderVertical(Window, Name, Position, 1, 100, 1);
                Position.y += Position.h;
            }
            else if (std::strncmp(SliderType, "horizontal", 10) == 0)
            {
                Position.h = 25.0f;
                gc = new Tilc::Gui::TSliderHorizontal(Window, Name, Position, 1, 100, 1);
                Position.y += Position.h;
            }
            continue;
        }
        if (std::strncmp(Type, "scrollbar", 9) == 0)
        {
            Position.w = Window->m_RealPosition.w;
            Tilc::Gui::TLabel* lbl = new Tilc::Gui::TLabel(Window, Tilc::TExtString("lbl") + Name, Position, Label, false);
            if (lbl)
            {
                lbl->SetFontToUse("DefaultBold");
            }
            gc = lbl;
            Position.y += Position.h;
            Position.x = PaddingLeft;
            const char* SliderType = *(Item.begin() + 3);
            if (std::strncmp(SliderType, "vertical", 8) == 0)
            {
                Position.h = 200.0f;
                gc = new Tilc::Gui::TScrollBarVertical(Window, Name, Position, Tilc::Gui::EControlType::ECT_ScrollBar, 1, 100, 1);
                Position.y += Position.h;
            }
            else if (std::strncmp(SliderType, "horizontal", 10) == 0)
            {
                Position.h = 25.0f;
                gc = new Tilc::Gui::TScrollBarHorizontal(Window, Name, Position, Tilc::Gui::EControlType::ECT_ScrollBar, 1, 100, 1);
                Position.y += Position.h;
            }
            continue;
        }
        if (std::strncmp(Type, "br", 5) == 0)
        {
            Position.y += Position.h;
            continue;
        }
    }
}
