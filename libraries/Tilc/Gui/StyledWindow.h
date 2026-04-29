#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Gui/GuiControl.h"
#include "Tilc/Window.h"
#include <list>


namespace Tilc {
    class TGame;
    class TStdObject;

    namespace Gui {

        class TCaret;
        class TPanel;
        class TScrollBar;

        class DECLSPEC TStyledWindow : public TGuiControl
        {
        public:
            TStyledWindow(TGuiControl* parent, Tilc::TExtString name, const SDL_FRect& position);
            TStyledWindow(TGuiControl* parent, Tilc::TExtString name, const SDL_FRect& position, Tilc::TExtString layoutFilename);
            TStyledWindow(TGuiControl* parent, Tilc::TExtString name, const SDL_FRect& position, Tilc::TExtString layout, int layoutContentType, bool returnEditedValues);
            virtual ~TStyledWindow();
            void RemoveFromParent();

            virtual void Draw();
            void DrawCaptionButtons();
            void DrawCaption();

            virtual void LoadGuiLayout(Tilc::TExtString content, bool clearActualContent = true);
            virtual void LoadGuiLayoutFromFile(Tilc::TExtString fname, bool clearActualContent = true);
            TCaret* GetCaret() const;

            // odpowiada za wyrysowanie karetki
            //void DrawCaret();

            virtual bool OnMouseButtonDown(const SDL_Event& event) override;
            virtual bool OnMouseButtonUp(const SDL_Event& event) override;
            /*
            virtual LRESULT onMove(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam);
            virtual LRESULT onSize(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam);
            virtual LRESULT onSizing(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam);
            virtual LRESULT onGetMinMaxInfo(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam);
            virtual LRESULT onLButtonMouseDown(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam);
            virtual LRESULT onLButtonMouseUp(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam);
            virtual LRESULT onNCLButtonMouseDown(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam);
            virtual LRESULT onNCLButtonMouseUp(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam);
            virtual LRESULT onEraseBkgnd(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam) { return 1; }
            virtual LRESULT onExitSizeMove(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam);
            virtual LRESULT onWmUserEvent(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam);
            virtual LRESULT onNCLButtonDblClk(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam);
            virtual void onTimer(unsigned int uID, unsigned int curTickCount);
            virtual void onActivation();
            virtual void onDeactivation();
            */

            // Funkcje obsługi klawiatury wywoływane przed ewentualnym wywołaniem funkcji obsługi
            // klawiatury aktywnej kontrolki (o ile jest taka). Jeśli którakolwiek z tych funkcji zwróci
            // TRUE, to zdarzenie nie jest przekazywane do aktywnej kontrolki.
            virtual bool CommonKeyProcessing(const SDL_Event& event);
            virtual bool OnKeyDown(const SDL_Event& event) override;
            virtual bool OnKeyUp(const SDL_Event& event) override;


            virtual void AllowResizing(bool value);

            TGuiControl* GetControl(const Tilc::TExtString& ControlName);
            Tilc::TExtString GetControlValue(const Tilc::TExtString& ControlName);
            void SetControlValue(const Tilc::TExtString& ControlName, const Tilc::TExtString& value);

            // Przenosi focus do następnej kontrolki. Jeśli żadna kontrolka nie była w tym momencie
            // aktywna, to focus ustawiany jest na pierwszej kontrolce, która na to pozwala.
            void MoveFocusForward();
            // Przenosi focus do poprzedniej kontrolki. Jeśli żadna kontrolka nie była w tym momencie
            // aktywna, to focus ustawiany jest na pierwszej kontrolce, która na to pozwala.
            void MoveFocusBackward();
            // Wypełnia podaną listę wszystkimi GUI CONTROL'ami, które mogą przyjąć focus
            void GetAllFocusableGuiControls(std::list<TGuiControl*>& lst);

            inline void CenterOnScreen()
            {
                Tilc::GameObject->GetContext()->m_Window->CenterOnScreen();
            }

            // Method onGetEditedValues is called after child window is closed by button of name:
            // DEFAULT_BTN_OK or DEFAULT_BTN_CANCEL
            // In first case we get as a parameter map CMap object of type: CString (name) => CString (value)
            //   After processing map object passed to the function should not be deleted because it is done automatically
            // In second case we get map=NULL.
            virtual void OnGetEditedValues(std::unordered_map<TExtString, TExtString>& map) override;

            void SetVerticalScrollRange(int min, int max);
            void SetHorizontalScrollRange(int min, int max);

            virtual void SetActiveControl(TGuiControl* Control) override;

            // Ads this window on top of Render Window's modal stack
            void SetModal();

        protected:
            // Parametry wpływające na szybkość obsługi zdarzeń klawiatury
            // opóźnienie po pierwszym wciśnięciu klawisza
            float m_DelayAfterKeyDown;
            // opóźnienie pomiędzy kolejnymi zdarzeniami onKeyPress (wywoływanymi w przypadku, gdy klawisz
            // jest wciśnięty)
            float m_DelayAfterKeyPress;
            // przechowuje aktualne opóźnienie w obsłudze kolejnego zdarzenia klawiatury onKeyDown lub
            // onKeyPress. Ma jedną z wartości: this->_delayAfterKeyDown lub this->_delayAfterKeyPress
            float m_CurrentDelay;
            // czas obsługi ostatniego zdarzenia klawiatury onKeyDown lub onKeyPress. W zdarzeniu
            // onKeyUp czas ten jest zerowany.
            float m_LastKeyEventTime;

            // if _layoutFilename file exists, then it is used to create layout in createGuiLayout function.
            Tilc::TExtString m_LayoutFilename;

            // If all values edited in window shoul be returned on window destroy. If yes, in window destructor there is created
            // CMap object containing edited values (CExtPairs Tilc::TExtString=>Tilc::TExtString: "editedVariableName"=>"edtitedVariableValue")
            // and to parent window is sended message with pointer to the object.
            bool m_ReturnEditedValues;

            // If window has Caption
            bool m_WithCaption{ true };

            // Poniższa funkcja zwraca TRUE, jeśli przetworzyła klawisz i FALSE jeśli go zignorowała.
            /*
            virtual bool CommonKeyProcessing(bool vkAlt, bool vkShift, bool vkControl,
                bool vkLAlt, bool vkRAlt,
                bool vkLShift, bool vkRShift,
                bool vkLControl, bool vkRControl,
                bool systemKey,
                unsigned int virtualCode, unsigned int scanCode, char ch, bool& updateCaretPos, bool& redraw);
            */
            void CommonInit(Tilc::TExtString layoutFilename = "");

            void OnScrollHorizontal();
            void OnScrollVertical();

            // Funkcja wywoływana przez scrollbar który jest podczepiony do tego obiektu w momencie zmiany
            // jego pozycji (tj położenia suwaka).
            virtual void OnThumbChange(int oldPosition, int curPosition, TScrollBar* scrollbar) override;

        protected:
            float GAP_X_BETWEEN_CAPTION_BUTTON_AND_WINDOW_FRAME{ 6.0f };
            float GAP_X_BETWEEN_CAPTION_BUTTONS{ 8.0f };
            float GAP_Y_BETWEEN_CAPTION_BUTTON_AND_WINDOW_FRAME{ 1.0f } ;
            void InternalProcessOnGetEditedValues(Tilc::TStdObject* eventInfo);
        };

    }
}
