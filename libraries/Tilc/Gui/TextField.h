#pragma once
#include <SDL3/SDL.h>
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/EventManager.h"
#include "Tilc/Gui/GuiControl.h"

namespace Tilc
{
    namespace Gui
    {
        class TCaret;
        class TClipboard;


        class DECLSPEC TTextField : public TGuiControl
        {
        public:
            TTextField(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const TExtString& text = "", bool tabStop = true);
            virtual ~TTextField();

            // Funkcje obsługi zdarzeń
            virtual void Draw() override;
            virtual void SetText(const Tilc::TExtString& Text) override;

            // czyści tekst kontrolki (atrybut this->_text) ze znaków o kodzie < 32.
            virtual void CleanupText();

            // Metoda odpowiada za przewijanie i zaznaczanie tekstu w polu tekstowym podczas wciśnięcia
            // klawisza myszki i przeciągnięcia myszki na jeden z końców pola lub poza jego obszar.
            // ZWRACA TRUE, jeśli zaszła zmiana w wyświetlanej zawartości i należy odświeżyć kontrlkę.
            virtual bool Update(float DeltaTime) override;
            virtual void Hide() override;

            // zwraca minimalną szerokość kontrolki (width) w zależności od szerokości bitmap użytych
            // w aktualnym temacie graficznym (theme)
            int GetMinWidth();

            // Umieszcza focus na tej kontrolce a dokładnie:
            //      Jeśli aktywną kontrolką okna Parent nie jest ta kontrolka, to jest wywoływane metoda
            //      ParentWnd->setActiveControl(this) tego okna.
            //      Przełącza stan kontrolki w tryb CTEXTFIELD_STATE_ACTIVE.
            //      Ustawia wielkość i położenie karetki wewnątrz tej kontrolki a następnie pokazuje
            //      karetkę i ustawia czas ostatniego przełączenia widoczności karetki (aktualizowany
            //      przy każdym pojawieniu się i zniknięciu karetki)..
            virtual void Focus() override;
            // Usuwa focus z tej kontrolki a dokładnie:
            //      NULL-uje wskaźnik aktywnej kontrolki w ParentWindow
            //      Usuwa zaznaczenie (nie usuwając tekstu na którym ono jest)
            //      Przełącza kontrolkę w stan CTEXTFIELD_STATE_HOVER lub CTEXTFIELD_STATE_NORMAL
            //      w zależności od aktualnego położenia myszy.
            virtual void LooseFocus() override;

            // Zwraca wysokość (height) karetki
            inline int GetCaretHeight() { return m_LineHeight + 4; }
            // Aktualizuje położenie karetki i pokazuje ją (jeśli aktualnie była niewidoczna) w obrębie
            // tej kontrolki.
            void UpdateCaretPos();
            // zwraca położenie karetki dla bieżących wartości atrybutów: this->_startChar oraz
            // this->_caretAtChar. Współrzędne są liczone względem lewego górnego rogu tej kontrolki.
            virtual SDL_FPoint CalculateCaretPos();

            // zwraca współrzędne lewego górnego rogu prostokąta dla podanego znaku. Współrzędne są
            // liczone względem lewego górnego rogu tej kontrolki z uwzględnieniem wartości atrybutu:
            // m_StartChar. Jeśli karetka dla podanego znaku znajduje się poza wyświetlanym obszarem
            // po lewej stronie to Result=-1 a jeśli jest po prawej stronie wyświetlanego obszaru,
            // to Result=-2. Otherwise if all ok then Result = 0;
            virtual SDL_FPoint CalculateCharPos(int CurrentChar, int& Result);

            // Odpowiada za bezpośrednie wyrysowanie karetki na DC (nie bitmapie offscreenow-ej)
            // ParentOkna
            void DrawCaret();

            // Usuwa zaznaczenie (bez usunięcia tekstu, który ono wskazywało) i ewentualnie odrysowuje
            // kontrolkę
            void ClearSelection(bool redraw = true);
            // Zwraca TRUE, jeśli w kontrolce jest ustawion zaznaczenie.
            inline bool IsSelection()
            {
                return m_SelStart != m_SelEnd;
            }
            // Usuwa ewentualne zaznaczenie (bez usunięcia tekstu) i wstawia podany tekst na podanej
            // pozycji.
            inline void InsertText(size_t pos, const Tilc::TExtString& s, bool redraw = true)
            {
                ClearSelection(false);
                m_Text.InsertAt(pos, s);
                if (redraw)
                {
                    Invalidate();
                }
            }

            // Wstawia tekst w tej kontrolce i ewentualnie ją odrysowuje. Jeśli w kontrolce było ustawione
            // zaznaczenie to zastępowany jest wskazany przez nie tekst a karetka ustawia się na końcu
            // nowo wklejonego tekstu. Jeśli nie było zaznaczenia, to tekst wstawiony jest w miejscu
            // w którym znajduje się karetka. Po wklejeniu karetka ustawiana jest na końcu wstawionego
            // tekstu.
            void InsertText(Tilc::TExtString s, bool redraw = true);
            // zaznacza całą zawartość pola tekstowego.
            void SelectAll(bool redraw = true);
            // Ustawia zaznaczenie na podstawie podanych parametrów: jego początku (start) i długości
            // (length) liczonej w znakach. Ewentualnie odrysowuje kontrolkę.
            void SetSelection(size_t start, size_t length, bool redraw = true);
            // zastępuje tekst wskazywany przez zaznaczenie i ewentualnie odrysowuje kontrolkę.
            void ReplaceSelectionWith(const Tilc::TExtString& replaceWith, bool redraw = true);
            // usuwa tekst wskazywany przez zaznaczenie i ewentualnie odrysowuje kontrolkę.
            void RemoveSelectedText(bool redraw = true);

            // Zwraca długość zaznaczenia liczoną w znakach.
            virtual size_t GetSelectionLength();
            // Zwraca łańcuch wskazywany przez zaznaczenie.
            virtual Tilc::TExtString GetSelectedText();
            // ustawia pozycję początkową: this->_startChar tak by znak wskazywany przez this->_caretAtChar
            // mieścił się na widoku w tym polu tekstowym i był możliwie bliska jego prawej krawędzi.
            // Zwraca TRUE, jesli nastą piła miana wartości this->_startChar.
            virtual bool AdjustStartCharForCaretAtChar();

            // zwraca numer ostatniego widocznego (wyświetlonego w całości a nie tylko w części)
            // w polu tesktowym znaku w danej szerokości. Jeśli parametr max_inner_width jest równy -1,
            // to zwracany jest numer ostatniego znaku w całym polu tekstowym. Jeśli jest > 0, to
            // zwracany jest ostatni znak mieszczący się w podanej szerokości.
            virtual int GetLastVisibleCharPos(int max_inner_width = -1);
            // Wykorzystuje funkcję this->getLastVisibleCharPos i zwraca ostatni widoczny w całości
            // w podanej szerokości znak.
            virtual char GetLastVisibleChar(int max_inner_width = -1);
/*
            // Funkcje obsługi zdarzeń
            virtual BOOL WINAPI onMouseMove(LONG x, LONG y);
            virtual BOOL WINAPI onMouseDown(LONG x, LONG y);
            virtual BOOL WINAPI onMouseUp(LONG x, LONG y);

            virtual BOOL WINAPI onKeyDown(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
                BOOL vkLAlt, BOOL vkRAlt,
                BOOL vkLShift, BOOL vkRShift,
                BOOL vkLControl, BOOL vkRControl,
                BOOL systemKey,
                ULONG virtualCode, ULONG scanCode, WCHAR ch);
            virtual BOOL WINAPI onKeyPressed(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
                BOOL vkLAlt, BOOL vkRAlt,
                BOOL vkLShift, BOOL vkRShift,
                BOOL vkLControl, BOOL vkRControl,
                BOOL systemKey,
                ULONG virtualCode, ULONG scanCode, WCHAR ch);
            virtual BOOL WINAPI onKeyUp(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
                BOOL vkLAlt, BOOL vkRAlt,
                BOOL vkLShift, BOOL vkRShift,
                BOOL vkLControl, BOOL vkRControl,
                BOOL systemKey,
                ULONG virtualCode, ULONG scanCode, WCHAR ch);
*/
        protected:
            int m_PaddingLeft{ 8 };
            int m_PaddingRight{ 8 };
            int m_PaddingTop{ 5 };
            int m_PaddingBottom{ 5 };
            int m_LineHeight;
            // numer znaku licząc od 0, od którego jest wyświetlany tekst w polu tekstowym
            int m_StartChar;
            // numer znaku licząc od 0, przed którym znajduje się karetka
            int m_CaretAtChar;
            // Wskaźnik na karetkę przechowywaną w ParentWindow - nie trzeba go zwalniać bo robi to
            // ParentWindow.
            TCaret* m_Caret;
            // Wskaźnik na schowek przechowywany w TGameObject - nie trzeba go zwalniać bo robi to
            // TGameObject.
            TClipboard* m_Clipboard;

            // Poniższe dwa atrybuty określają znaki przed którymi zaczyna się i kończy zaznaczenie.
            int m_SelStart;
            int m_SelEnd;
            // Numer znaku przed którym znajdował się kursor w momencie pojawienia się zaznaczenia
            int m_SelBegin;

            // USed font name
            TExtString FontNameInUse{ "Default" };

            // Zwraca prostokąt położenia selekcji. Współrzędne tego prostokąta są liczone względem
            // początku tej kontrolki.
            virtual SDL_FRect CalculateSelectionRectForText(const Tilc::TExtString& s);

            /*
            // poniższa funkcja odpowiada za ustawienie karetki najbliżej miejsca kliknięcia, ale tak by
            // znajdowała się między literami.
            // localX i localY, to współrzędne kliknięcia względem początku tego pola tekstowego.
            virtual VOID WINAPI _positionCaretNearClickedPoint(LONG localX, LONG localY);
*/
            virtual void UpdateSelection(unsigned int vkKey, int lastCaretAtChar, bool& updateCaretPos, bool& redraw);
            /*
            virtual VOID WINAPI _updateCursorPosition(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
                BOOL vkLAlt, BOOL vkRAlt,
                BOOL vkLShift, BOOL vkRShift,
                BOOL vkLControl, BOOL vkRControl, ULONG vkKey, BOOL& updateCaretPos, BOOL& redraw);
            // Poniższa funkcja zwraca TRUE, jeśli przetworzyła klawisz i FALSE jeśli go zignorowała.
            virtual BOOL WINAPI _commonKeyProcessing(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
                BOOL vkLAlt, BOOL vkRAlt,
                BOOL vkLShift, BOOL vkRShift,
                BOOL vkLControl, BOOL vkRControl,
                BOOL systemKey,
                ULONG virtualCode, ULONG scanCode, WCHAR ch, BOOL& updateCaretPos, BOOL& redraw);
*/
            virtual int CalculateInnerWidth();
            virtual int GetMaxXPosAllowedForContent();

        private:
/*
            virtual VOID WINAPI _updateCanvas();
*/
        };

    }
}
