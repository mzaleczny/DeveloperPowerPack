#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Commerce/Shop.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/JsonParser.h"
#include "Tilc/Gui/Layout/JsonLayoutFile.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/Label.h"
#include "Tilc/Gui/Button.h"
#include "Tilc/Gui/Listbox.h"
#include "Tilc/Gui/MultiColumnListbox.h"
#include "Tilc/Game.h"
#include "Tilc/Apps/Www/RestAPI.h"
#include <SDL3/SDL.h>

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
            static void CreateForm(TStyledWindow* Window, std::initializer_list<std::initializer_list<const char*>> FormFields, const char* SaveButtonLabel = nullptr, float TopY = 0.0f);
        };

        template <typename TItemType>
        class DECLSPEC TFormPackage
        {
        public:
            Tilc::TExtString m_Name;
            Tilc::Gui::TStyledWindow* m_ParentWindow;
            SDL_FRect m_Position;

            Tilc::TExtString Url_GetItem;
            Tilc::TExtString Url_List;
            Tilc::TExtString Url_Save;
            Tilc::TExtString Url_Delete;

            std::vector<TItemType> m_ChangedItems{};
            std::vector<int> m_ItemsToDelete{};
            Tilc::Gui::TStyledWindow* WndListForm{};
            Tilc::Gui::TStyledWindow* WndEditForm{};
            Tilc::Gui::TLabel* lblTitle{};

            Tilc::Gui::TMultiColumnListbox* lbList{};
            std::vector<float> lbListColumnsWidths;
            std::vector<TExtString> lbListColumnsCaptions;

            std::function<void()> HideAllWindowsFromParent;

            TFormPackage(Tilc::Gui::TStyledWindow* ParentWindow, const char* Name, SDL_FRect& Position,
                std::initializer_list<float> ListColumnsWidths,
                std::initializer_list<Tilc::TExtString> ListColumnsCaptions,
                std::initializer_list<std::initializer_list<const char*>> EditFormFields,
                std::function<void()>&& HideFunc,
                Tilc::Gui::TLabel* TitleLabel
                )
                : m_ParentWindow(ParentWindow), m_Name(Name), m_Position(Position),
                    lbListColumnsWidths(ListColumnsWidths), lbListColumnsCaptions(ListColumnsCaptions),
                    HideAllWindowsFromParent(HideFunc), lblTitle(TitleLabel)
            {
                InitListWindow(m_ParentWindow);
                InitEditWindow(m_ParentWindow, EditFormFields);
                // Poniższa funkcja zapisuje danemu oknu wskaźnik na drugie okno List->Edit i Edit->List
                // Jest to potrzebne, żeby w funkcji obsługi zdarzeń móc sobie pobrać wskaźnik na drugi formularz konwertując go z postaci string na wskaźnik.
                SetPropsLinkingToStyledWindows();
            }
            ~TFormPackage()
            {
                if (WndListForm)
                {
                    delete WndListForm;
                    WndListForm = nullptr;
                }
                if (WndEditForm)
                {
                    delete WndEditForm;
                    WndEditForm = nullptr;
                }
            }

            void HideAll()
            {
                if (WndListForm)    WndListForm->Hide();
                if (WndEditForm)    WndEditForm->Hide();
            }

            static void* GetPointer(Tilc::Gui::TGuiControl* Control, const char* WindowType)
            {
                if (Control->m_Props.find(WindowType) != Control->m_Props.end())
                {
                    uintptr_t StyledWindowPointer = std::stoull(Control->m_Props[WindowType]);
                    return reinterpret_cast<void*>(StyledWindowPointer);
                }

                Tilc::Gui::TStyledWindow* ParentWindow = Control->GetParentWindow();
                if (!ParentWindow)
                {
                    return nullptr;
                }

                uintptr_t StyledWindowPointer = std::stoull(ParentWindow->m_Props[WindowType]);
                return reinterpret_cast<void*>(StyledWindowPointer);
            }

            static void ShowEditWindow(Tilc::Gui::TStyledWindow* EditWindow, const TItemType& Item, int Index = -1)
            {
                if (EditWindow)
                {
                    EditWindow->Show();
                    EditWindow->Focus();
                    EditWindow->m_Props["IndexInChangedList"] = std::to_string(Index);
                    Tilc::TExtString Caption;
                    if (Item.IsEmpty())
                    {
                        Caption = Item.AddLabel;
                    }
                    else
                    {
                        Caption = Item.EditLabel;
                    }
                    EditWindow->SetChildrenTextes(Item.GetDataMap());

                    auto* FormPackage = reinterpret_cast<Tilc::Gui::TFormPackage<TItemType>*>(GetPointer(EditWindow, "FormPackage"));
                    if (FormPackage)
                    {
                        FormPackage->lblTitle->SetText(Caption);
                    }
                }
            }
            static void ShowListWindow(Tilc::Gui::TStyledWindow* ListWindow)
            {
                static bool FetchItemsFromDatabase = true;
                if (ListWindow)
                {
                    if (auto* FormPackage = reinterpret_cast<Tilc::Gui::TFormPackage<TItemType>*>(GetPointer(ListWindow, "FormPackage")); FormPackage)
                    {
                        if (FormPackage->HideAllWindowsFromParent)
                        {
                            FormPackage->HideAllWindowsFromParent();
                        }
                        ListWindow->Show();
                        ListWindow->Focus();
                        TItemType Item;
                        FormPackage->lblTitle->SetText(Item.ListLabel);
                        if (FetchItemsFromDatabase)
                        {
                            FetchItemsFromDatabase = false;
                            if (FormPackage->lbList)
                            {
                                FormPackage->RefreshList(FormPackage->lbList, Fetch(FormPackage->Url_List));
                            }
                        }
                    }
                }
            }

            // *******************************************************************************************************************
            // EVENT HANDLERS
            // *******************************************************************************************************************
            static int OnListAddClick(float x, float y, Uint8 MouseButton, Tilc::Gui::TGuiControl* Control)
            {
                if (auto* Button = dynamic_cast<Tilc::Gui::TButton*>(Control); Button)
                {
                    auto* ListWindow = reinterpret_cast<Tilc::Gui::TStyledWindow*>(GetPointer(Control, "ListWindow"));
                    auto* EditWindow = reinterpret_cast<Tilc::Gui::TStyledWindow*>(GetPointer(Control, "EditWindow"));
                    auto* FormPackage = reinterpret_cast<Tilc::Gui::TFormPackage<TItemType>*>(GetPointer(Control, "FormPackage"));
                    if (EditWindow)
                    {
                        if (auto* btnSave = EditWindow->GetChild("btnSave"); btnSave)
                        {
                            btnSave->SetText("Dodaj");
                            if (FormPackage->HideAllWindowsFromParent)
                            {
                                FormPackage->HideAllWindowsFromParent();
                            }
                            TFormPackage::ShowEditWindow(EditWindow, {});
                            if (FormPackage->lblTitle)
                            {
                                Tilc::TExtString Caption = "Dodaj kategorię";
                                FormPackage->lblTitle->SetText(Caption);
                            }
                        }
                    }
                }
                return 0;
            }

            static int OnListEditClick(float x, float y, Uint8 MouseButton, Tilc::Gui::TGuiControl* Control)
            {
                if (auto* Button = dynamic_cast<Tilc::Gui::TButton*>(Control); Button)
                {
                    auto* ListWindow = reinterpret_cast<Tilc::Gui::TStyledWindow*>(GetPointer(Control, "ListWindow"));
                    auto* EditWindow = reinterpret_cast<Tilc::Gui::TStyledWindow*>(GetPointer(Control, "EditWindow"));
                    auto* FormPackage = reinterpret_cast<Tilc::Gui::TFormPackage<TItemType>*>(GetPointer(Control, "FormPackage"));
                    if (EditWindow)
                    {
                        if (auto* btnSave = EditWindow->GetChild("btnSave"); btnSave)
                        {
                            btnSave->SetText("Zapisz");
                            if (FormPackage->HideAllWindowsFromParent)
                            {
                                FormPackage->HideAllWindowsFromParent();
                            }

                            if (FormPackage->lbList)
                            {
                                Tilc::Gui::TGuiControlItem* SelectedItem = FormPackage->lbList->GetSelectedItem();
                                if (SelectedItem)
                                {
                                    TItemType Item;
                                    Tilc::TExtString Id = SelectedItem->m_Props["id"];
                                    Tilc::TExtString IndexInChangedList = SelectedItem->m_Props["IndexInChangedList"];
                                    int Index = -1;
                                    if (IndexInChangedList != "-1")
                                    {
                                        Index = std::stoi(IndexInChangedList);
                                        Item = FormPackage->m_ChangedItems[Index];
                                    }
                                    else
                                    {
                                        // find the category in m_ChangedCategories vector
                                        auto it = std::ranges::find_if(FormPackage->m_ChangedItems, [Id](const TItemType& c)
                                        {
                                            return c.id == Id;
                                        });
                                        // Get Index of found element or -1 when not found in changed vector
                                        if (it != FormPackage->m_ChangedItems.end())
                                        {
                                            Index = static_cast<int>(std::ranges::distance(FormPackage->m_ChangedItems.begin(), it));
                                            Item = FormPackage->m_ChangedItems[Index];
                                        }
                                        else
                                        {
                                            // Tutaj pobieramy dane kategorii z serwera i dodajemy do listy zmienionych, które będą czekać w kolejce na synchronizację
                                            Item = Fetch<TItemType>(FormPackage->Url_GetItem, Id);
                                            FormPackage->m_ChangedItems.push_back(Item);
                                            Index = static_cast<int>(FormPackage->m_ChangedItems.size() - 1);
                                        }
                                    }

                                    TFormPackage::ShowEditWindow(EditWindow, Item, Index);

                                    FormPackage->lblTitle->SetText(Item.EditLabel);
                                }
                            }
                        }
                    }
                }
                return 0;
            }

            static int OnListDeleteClick(float x, float y, Uint8 MouseButton, Tilc::Gui::TGuiControl* Control)
            {
                auto* ListWindow = reinterpret_cast<Tilc::Gui::TStyledWindow*>(GetPointer(Control, "ListWindow"));
                auto* EditWindow = reinterpret_cast<Tilc::Gui::TStyledWindow*>(GetPointer(Control, "EditWindow"));
                auto* FormPackage = reinterpret_cast<Tilc::Gui::TFormPackage<TItemType>*>(GetPointer(Control, "FormPackage"));
                if (FormPackage->lbList)
                {
                    Tilc::Gui::TGuiControlItem* SelectedItem = FormPackage->lbList->GetSelectedItem();
                    if (SelectedItem)
                    {
                        int Index = FormPackage->lbList->GetSelectedIndex();
                        Tilc::TExtString IndexInChangedList = SelectedItem->m_Props["IndexInChangedList"];
                        if (IndexInChangedList != "-1")
                        {
                            // Nie możemy usunąć pozycji z listy m_ChangedItems, bo musielibyśmy zrobić szereg uaktualnień indeksów w atrybutach elementów
                            // na liście lbList
                            // TStateGame::m_ChangedCategories.erase(TStateGame::m_ChangedCategories.begin() + std::stoi(IndexInChangedList));
                            // Zamiast tego wyzerujemy usuwany element, żeby zaznaczyć, że go nie ma
                            FormPackage->m_ChangedItems[std::stoi(IndexInChangedList)] = {};
                        }
                        if (!SelectedItem->m_Props["id"].empty())
                        {
                            FormPackage->m_ItemsToDelete.push_back(std::stoi(SelectedItem->m_Props["id"]));
                        }
                        FormPackage->DeleteItem(Index);
                    }
                }
                return 0;
            }

            static int OnListDuplicateClick(float x, float y, Uint8 MouseButton, Tilc::Gui::TGuiControl* Control)
            {
                auto* ListWindow = reinterpret_cast<Tilc::Gui::TStyledWindow*>(GetPointer(Control, "ListWindow"));
                auto* EditWindow = reinterpret_cast<Tilc::Gui::TStyledWindow*>(GetPointer(Control, "EditWindow"));
                auto* FormPackage = reinterpret_cast<Tilc::Gui::TFormPackage<TItemType>*>(GetPointer(Control, "FormPackage"));
                if (FormPackage->lbList)
                {
                    Tilc::Gui::TGuiControlItem* SelectedItem = FormPackage->lbList->GetSelectedItem();
                    if (SelectedItem)
                    {
                        TItemType NewItem{};
                        int Index = FormPackage->lbList->GetSelectedIndex();
                        Tilc::TExtString IndexInChangedList = SelectedItem->m_Props["IndexInChangedList"];
                        if (IndexInChangedList != "-1")
                        {
                            NewItem = FormPackage->m_ChangedItems[std::stoi(IndexInChangedList)];
                            FormPackage->m_ChangedItems.erase(FormPackage->m_ChangedItems.begin() + std::stoi(IndexInChangedList));
                        }
                        else
                        {
                            NewItem = Tilc::Apps::Www::Fetch<Tilc::Commerce::TCategory>(FormPackage->Url_GetItem, SelectedItem->m_Props["id"]);
                        }
                        NewItem.id = "";
                        FormPackage->m_ChangedCategories.push_back(NewItem);
                        Tilc::Gui::TGuiControlItem* Item = FormPackage->lbList->AddItem(NewItem.GetDataForListColumns());
                        Item->m_Props["id"] = "";
                        Item->m_Props["IndexInChangedList"] = std::to_string(FormPackage->m_ChangedItems.size() - 1);
                    }
                }
                return 0;
            }

            static int OnEditOkClick(float x, float y, Uint8 MouseButton, Tilc::Gui::TGuiControl* Control)
            {
                auto* ListWindow = reinterpret_cast<Tilc::Gui::TStyledWindow*>(GetPointer(Control, "ListWindow"));
                auto* EditWindow = reinterpret_cast<Tilc::Gui::TStyledWindow*>(GetPointer(Control, "EditWindow"));
                auto* FormPackage = reinterpret_cast<Tilc::Gui::TFormPackage<TItemType>*>(GetPointer(Control, "FormPackage"));
                TItemType Item;
                Item.SetDataFromMap(EditWindow->GetChildrenTextes());

                if (FormPackage->lbList)
                {
                    int Index = std::stoi(EditWindow->m_Props["IndexInChangedList"]);
                    if (Index == -1)
                    {
                        FormPackage->m_ChangedItems.push_back(Item);
                        Tilc::Gui::TGuiControlItem* lbItem = FormPackage->lbList->AddItem(Item.GetDataForListColumns());
                        Item->m_Props["id"] = "";
                        Item->m_Props["IndexInChangedList"] = std::to_string(FormPackage->m_ChangedItems.size() - 1);
                    }
                    else
                    {
                        Item.id = FormPackage->m_ChangedItems[Index].id;
                        FormPackage->m_ChangedItems[Index] = Item;

                        Tilc::Gui::TGuiControlItem* lbItem = FormPackage->lbList->GetSelectedItem();
                        lbItem->SetFromColumnList(Item.GetDataForListColumns());
                        lbItem->m_Props["IndexInChangedList"] = std::to_string(Index);
                    }

                }
                TFormPackage::ShowListWindow(ListWindow);
                return 0;
            }

            static int OnEditCancelClick(float x, float y, Uint8 MouseButton, Tilc::Gui::TGuiControl* Control)
            {
                auto* ListWindow = reinterpret_cast<Tilc::Gui::TStyledWindow*>(GetPointer(Control, "ListWindow"));
                TFormPackage::ShowListWindow(ListWindow);
                return 0;
            }
            // *******************************************************************************************************************
            // *******************************************************************************************************************

        protected:
            void SetPropsLinkingToStyledWindows()
            {
                if (WndListForm && WndEditForm)
                {
                    WndListForm->m_Props["ListWindow"] = std::format("0x{:X}", reinterpret_cast<uintptr_t>(WndListForm));
                    WndListForm->m_Props["EditWindow"] = std::format("0x{:X}", reinterpret_cast<uintptr_t>(WndEditForm));
                    WndListForm->m_Props["FormPackage"] = std::format("0x{:X}", reinterpret_cast<uintptr_t>(this));

                    WndEditForm->m_Props["ListWindow"] = std::format("0x{:X}", reinterpret_cast<uintptr_t>(WndListForm));
                    WndEditForm->m_Props["EditWindow"] = std::format("0x{:X}", reinterpret_cast<uintptr_t>(WndEditForm));
                    WndEditForm->m_Props["FormPackage"] = std::format("0x{:X}", reinterpret_cast<uintptr_t>(this));
                }
            }

            void InitListWindow(Tilc::Gui::TStyledWindow* ParentWindow)
            {
                Tilc::TExtString Json = (R"##(
                {
                    "TopLevelItems": [
                        {
                            "type": "window",
                            "name": "{NAME}",
                            "x": {POS_X},
                            "y": {POS_Y},
                            "width": {POS_W},
                            "height": {POS_H},
                            "caption": "",
                            "items": [
                                {
                                    "name": "lbList",
                                    "type": "multicolumn-listbox",
                                    "x": 10,
                                    "y": 10,
                                    "width": 1160,
                                    "height": 720,
                                    "text": ""
                                },
                                {
                                    "name": "btnAdd",
                                    "type": "button",
                                    "x": 10,
                                    "y": 738,
                                    "width": 100,
                                    "height": 25,
                                    "text": "Dodaj"
                                },
                                {
                                    "name": "btnEdit",
                                    "type": "button",
                                    "x": 125,
                                    "y": 738,
                                    "width": 100,
                                    "height": 25,
                                    "text": "Edytuj"
                                },
                                {
                                    "name": "btnDelete",
                                    "type": "button",
                                    "x": 240,
                                    "y": 738,
                                    "width": 100,
                                    "height": 25,
                                    "text": "Usuń"
                                },
                                {
                                    "name": "btnDuplicate",
                                    "type": "button",
                                    "x": 375,
                                    "y": 738,
                                    "width": 100,
                                    "height": 25,
                                    "text": "Duplikuj"
                                }
                            ]
                        }
                    ]
                }
                )##");

                Tilc::TExtString Name = m_Name + "_List";
                Json.StrReplace("{NAME}", Name);
                Json.StrReplace("{POS_X}", std::to_string(static_cast<int>(m_Position.x)));
                Json.StrReplace("{POS_Y}", std::to_string(static_cast<int>(m_Position.y)));
                Json.StrReplace("{POS_W}", std::to_string(static_cast<int>(m_Position.w)));
                Json.StrReplace("{POS_H}", std::to_string(static_cast<int>(m_Position.h)));
                WndListForm = new Tilc::Gui::TStyledWindow(Tilc::GameObject->m_Window, ParentWindow, Name, m_Position);
                if (WndListForm)
                {
                    WndListForm->SetHasBackground(true);
                    WndListForm->SetHasBorder(true);
                    WndListForm->SetHasCaption(false);
                    WndListForm->Hide();

                    Tilc::Gui::TJsonLayoutFile layout;
                    layout.loadLayout(Json, WndListForm);

                    Tilc::Gui::TGuiControl* btnAdd = WndListForm->GetChild("btnAdd");
                    btnAdd->OnClick = &TFormPackage::OnListAddClick;
                    Tilc::Gui::TGuiControl* btnEdit = WndListForm->GetChild("btnEdit");
                    btnEdit->OnClick = &TFormPackage::OnListEditClick;
                    Tilc::Gui::TGuiControl* btnDelete = WndListForm->GetChild("btnDelete");
                    btnDelete->OnClick = &TFormPackage::OnListDeleteClick;
                    Tilc::Gui::TGuiControl* btnDuplicate = WndListForm->GetChild("btnDuplicate");
                    btnDuplicate->OnClick = &TFormPackage::OnListDuplicateClick;


                    lbList = dynamic_cast<Tilc::Gui::TMultiColumnListbox*>(WndListForm->GetChild("lbList"));
                    if (lbList)
                    {
                        lbList->SetColumnWidths(lbListColumnsWidths);
                        lbList->SetHeaderCaptions(lbListColumnsCaptions);
                    }
                }
            }


            void InitEditWindow(Tilc::Gui::TStyledWindow* ParentWindow, std::initializer_list<std::initializer_list<const char*>> FormFields)
            {
                SDL_FRect Position;
                Position.x = 310.0f;
                Position.y = 120.0f;
                Position.w = 1180.0f;
                Position.h = 770.0f;
                WndEditForm = new Tilc::Gui::TStyledWindow(Tilc::GameObject->m_Window, ParentWindow, "WndProductEdit", Position);
                WndEditForm->SetHasBackground(true);
                WndEditForm->SetHasBorder(true);
                WndEditForm->SetHasCaption(false);
                WndEditForm->Hide();

                Tilc::Gui::TForm::CreateForm(WndEditForm, FormFields);
            }

            void RefreshList(Tilc::Gui::TMultiColumnListbox* lb, Tilc::TExtString ItemsJson)
            {
                lbList->DeleteItems();
                lbList->SetColumnWidths(lbListColumnsWidths);
                lbList->SetHeaderCaptions(lbListColumnsCaptions);

                TItemType Item;
                Item.FromJson(ItemsJson);
                auto* lbItem = lb->AddItem(Item.GetDataForListColumns());
                if (lbItem)
                {
                    lbItem->m_Props["id"] = Item.id;
                    lbItem->m_Props["IndexInChangedList"] = "-1";
                }
            }
        };
    }
}
