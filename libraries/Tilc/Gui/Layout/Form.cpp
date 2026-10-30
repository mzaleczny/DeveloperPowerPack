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
#include "Tilc/Gui/ImageListView.h"


DECLSPEC void* Tilc::Gui::GetPointer(Tilc::Gui::TGuiControl* Control, const char* PropName)
{
    if (Control->m_Props.find(PropName) != Control->m_Props.end())
    {
        uintptr_t StyledWindowPointer = std::stoull(Control->m_Props[PropName].substr(2), 0, 16);
        return reinterpret_cast<void*>(StyledWindowPointer);
    }

    Tilc::Gui::TStyledWindow* ParentWindow = dynamic_cast<Tilc::Gui::TStyledWindow*>(Control->GetParent());
    if (!ParentWindow)
    {
        return nullptr;
    }

    uintptr_t StyledWindowPointer = std::stoull(ParentWindow->m_Props[PropName].substr(2), 0, 16);
    return reinterpret_cast<void*>(StyledWindowPointer);
}

void Tilc::Gui::TForm::CreateForm(TStyledWindow* Window, std::initializer_list<std::initializer_list<const char*>> FormFields, const char* SaveButtonLabel, float TopY)
{
    float const PaddingLeft = 10.0f;
    float const PaddingRight = 10.0f;
    float const PaddingTop = (TopY > 10.0f) ? TopY : 10.0f;
    float const PaddingBottom = (TopY > 10.0f) ? TopY : 10.0f;
    float const SpacerW = 15.0f;
    float const SpacerH = 15.0f;
    SDL_FRect Position{PaddingLeft, PaddingTop, Window->m_Position.w, 25.0f };
    Tilc::Gui::TGuiControl* gc{};

    for (auto Item : FormFields)
    {
        const char* Label = *Item.begin();
        const char* Name = *(Item.begin()+1);
        const char* Type = *(Item.begin()+2);

        if (std::strncmp(Type, "label", 5) == 0)
        {
            Position.w = Window->m_Position.w;
            gc = new Tilc::Gui::TLabel(Window, Name, Position, Label, false);
            Position.y += Position.h;
            Position.x = PaddingLeft;
            continue;
        }
        else if (std::strncmp(Type, "textfield", 9) == 0)
        {
            Position.x = PaddingLeft;
            Position.w = Window->m_Position.w - PaddingLeft - PaddingRight;
            Position.h = 25.0f;
            Tilc::Gui::TLabel* lbl = new Tilc::Gui::TLabel(Window, Tilc::TExtString("lbl") + Name, Position, Label, false);
            if (lbl)
            {
                lbl->SetFontToUse("DefaultBold");
            }
            gc = lbl;
            Position.x = PaddingLeft;
            Position.y += Position.h;
            Position.h = 25.0f;
            gc = new Tilc::Gui::TTextField(Window, Name, Position);
            Position.y += Position.h + SpacerH;
            continue;
        }
        else if (std::strncmp(Type, "multiline-textfield", 19) == 0)
        {
            Position.w = Window->m_Position.w - PaddingLeft - PaddingRight;
            Position.h = 25.0f;
            Tilc::Gui::TLabel* lbl = new Tilc::Gui::TLabel(Window, Tilc::TExtString("lbl") + Name, Position, Label, false);
            if (lbl)
            {
                lbl->SetFontToUse("DefaultBold");
            }
            gc = lbl;
            Position.y += Position.h;
            Position.x = PaddingLeft;
            Position.h = 5*25.0f;
            gc = new Tilc::Gui::TMultilineTextField(Window, Name, Position);
            Position.y += Position.h + SpacerH;
            continue;
        }
        else if (std::strncmp(Type, "button", 6) == 0)
        {
            Position.w = std::atoi(*(Item.begin() + 3));
            Position.h = 25.0f;
            gc = new Tilc::Gui::TButton(Window, Name, Position, Label);
            Position.x += Position.w + SpacerW;
            continue;
        }
        else if (std::strncmp(Type, "checkbox", 8) == 0)
        {
            Position.w = std::atoi(*(Item.begin() + 3));
            Position.h = 25.0f;
            gc = new Tilc::Gui::TCheckbox(Window, Name, Position, Label, Label, false);
            Position.x += Position.w + SpacerW;
            continue;
        }
        else if (std::strncmp(Type, "option", 6) == 0)
        {
            int GroupNumber = std::atoi(*(Item.begin() + 3));
            Position.w = std::atoi(*(Item.begin() + 4));
            Position.h = 25.0f;
            gc = new Tilc::Gui::TOption(Window, Name, Position, Label, Label, false);
            Position.x += Position.w + SpacerW;
            if (Tilc::Gui::TGuiControl::m_OptionGroups.find(GroupNumber) == Tilc::Gui::TGuiControl::m_OptionGroups.end())
            {
                Tilc::Gui::TGuiControl::m_OptionGroups[GroupNumber] = Tilc::Gui::TOptionGroup();
            }
            Tilc::Gui::TGuiControl::m_OptionGroups[GroupNumber].AddOptions({ dynamic_cast<Tilc::Gui::TOption*>(gc) });
            continue;
        }
        else if (std::strncmp(Type, "listbox", 7) == 0)
        {
            Position.w = Window->m_RealPosition.w;
            Position.h = 25.0f;
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
        else if (std::strncmp(Type, "multicolumn-listbox", 1) == 0)
        {
            continue;
        }
        else if (std::strncmp(Type, "grid", 4) == 0)
        {
            Position.w = Window->m_RealPosition.w;
            Position.h = 25.0f;
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
        else if (std::strncmp(Type, "slider", 6) == 0)
        {
            Position.w = Window->m_RealPosition.w;
            Position.h = 25.0f;
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
        else if (std::strncmp(Type, "scrollbar", 9) == 0)
        {
            Position.w = Window->m_RealPosition.w;
            Position.h = 25.0f;
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
        else if (std::strncmp(Type, "br", 5) == 0)
        {
            Position.y += Position.h;
            continue;
        }
        else if (std::strncmp(Type, "pictures", 4) == 0)
        {
            // Label
            Position.w = Window->m_RealPosition.w;
            Position.h = 25.0f;
            Tilc::Gui::TLabel* lbl = new Tilc::Gui::TLabel(Window, Tilc::TExtString("lbl") + Name, Position, Label, false);
            if (lbl)
            {
                lbl->SetFontToUse("DefaultBold");
            }

            // Button to the right of label
            Position.x = lbl->m_Position.x + 75.0f;
            Position.w = 125.0f;
            Position.h = 25.0f;
            gc = new Tilc::Gui::TButton(Window, Tilc::TExtString("btnAddPicture_") + Name, Position, "Dodaj zdjęcie");

            // ImageListView belo the label
            Position.x = lbl->m_Position.x;
            Position.y = lbl->m_Position.y + 30.0f;
            Position.w = Window->m_RealPosition.w - 10;
            Position.h = 96.0f;
            Tilc::Gui::TImageListView* lst = new Tilc::Gui::TImageListView(Window, Tilc::TExtString("lstImageList_") + Name, Position, 96, 96);
            if (lst)
            {
                lst->SetAllowRemoveImage(true);
                lst->OnRemoveImage = OnRemoveMiniatures;
            }
            Position.y += Position.h + 15.0f;
            continue;
        }
    }

    Tilc::TExtString SaveButtonText = "Dodaj";
    if (SaveButtonLabel)
    {
        SaveButtonText = SaveButtonLabel;
    }

    Position.w = 100.0f;
    Position.h = 25.0f;
    gc = new Tilc::Gui::TButton(Window, "SaveButton", Position, SaveButtonText);
    Position.x += Position.w + SpacerW;
    gc = new Tilc::Gui::TButton(Window, "CancelButton", Position, "Anuluj");
}

int Tilc::Gui::OnRemoveMiniatures(TImageListView* ListView, int ImageIndex, Tilc::TExtString& ImageName)
{
    auto *ParentWindow = dynamic_cast<Tilc::Gui::TStyledWindow*>(ListView->GetParent());
    if (ParentWindow)
    {
        auto* CachedMiniatures = reinterpret_cast<std::unordered_map<Tilc::TExtString, Tilc::TExtString>*>(GetPointer(ParentWindow, "CachedMiniatures"));
        auto* CachedMiniaturesMap = reinterpret_cast<std::unordered_map<Tilc::TExtString, Tilc::TExtString>*>(GetPointer(ParentWindow, "CachedMiniaturesMap"));
        Tilc::TExtString TableName = ParentWindow->m_Props["TableName"];
        if (CachedMiniatures && CachedMiniaturesMap)
        {
            if (auto pos = ImageName.find("Pic_"); pos != std::string::npos)
            {
                int Removed = 0;
                Tilc::TExtString Miniature = ImageName.substr(pos);

                // Usuwamy miniatuurę z list cachujących
                CachedMiniatures->erase(Miniature);
                for (auto it = CachedMiniaturesMap->begin(); it != CachedMiniaturesMap->end(); )
                {
                    if (it->second == Miniature)
                    {
                        it = CachedMiniaturesMap->erase(it); // erase zwraca iterator do kolejnego elementu
                        Removed = 1;
                        break;
                    }
                    else
                    {
                        ++it;
                    }
                }

                // Usuwamy miniaturę z listy plików przypisanych do itemu
                Tilc::TStringVector v;
                ParentWindow->m_Props["Pictures"].Explode(',', v);
                v.erase(std::remove(v.begin(), v.end(), Miniature), v.end());
                ParentWindow->m_Props["Pictures"] = Tilc::Implode(',', v);

                ListView->Invalidate();
                return Removed;
            }
        }
    }
    return 0;
}
