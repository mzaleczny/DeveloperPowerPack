#pragma once

#include "Tilc/Gui/GuiControl.h"
#include "Tilc/Utils/ExtString.h"
#include <list>
#include <functional>

constexpr const int CSCROLLBAR_TYPE_UNDEFINED               = 0;
constexpr const int CSCROLLBAR_TYPE_VERTICAL                = 1;
constexpr const int CSCROLLBAR_TYPE_HORIZONTAL              = 2;

constexpr const int CSCROLLBAR_STATE_NORMAL                 = 0;
constexpr const int CSCROLLBAR_STATE_SMALL_SCROLLING_UP     = 1;
constexpr const int CSCROLLBAR_STATE_SMALL_SCROLLING_DOWN   = 2;
constexpr const int CSCROLLBAR_STATE_LARGE_SCROLLING_UP     = 3;
constexpr const int CSCROLLBAR_STATE_LARGE_SCROLLING_DOWN   = 4;
constexpr const int CSCROLLBAR_STATE_SMALL_SCROLLING_LEFT   = 5;
constexpr const int CSCROLLBAR_STATE_SMALL_SCROLLING_RIGHT  = 6;
constexpr const int CSCROLLBAR_STATE_LARGE_SCROLLING_LEFT   = 7;
constexpr const int CSCROLLBAR_STATE_LARGE_SCROLLING_RIGHT  = 8;
constexpr const int CSCROLLBAR_STATE_THUMB_DRAGGING         = 9;
constexpr const int CSCROLLBAR_STATE_HOVER_UP_BUTTON         = 10;
constexpr const int CSCROLLBAR_STATE_HOVER_DOWN_BUTTON       = 11;
constexpr const int CSCROLLBAR_STATE_HOVER_LEFT_BUTTON       = 12;
constexpr const int CSCROLLBAR_STATE_HOVER_RIGHT_BUTTON      = 13;
constexpr const int CSCROLLBAR_STATE_HOVER_ABOVE_THUMB       = 14;
constexpr const int CSCROLLBAR_STATE_HOVER_BELOW_THUMB       = 15;
constexpr const int CSCROLLBAR_STATE_HOVER_THUMB             = 16;


namespace Tilc::Gui {

    class TStyledWindow;

    class DECLSPEC TScrollBar : public TGuiControl
    {
    public:
        TScrollBar(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, EControlType ControlType, int thumbPosition = 1, int maxValue = 100, int minValue = 1, bool autosize = false);
        virtual ~TScrollBar();

        void SetSteps(int _small, int _large);
        void SetMinValue(int value);
        void SetMaxValue(int value);
        void StepBy(int step, bool redraw = true);
        void SetPosition(int position, bool fireEvent = true, bool redraw = true);
        int CalculatePositionForStep(int step);
        void RegisterPositionNotificationForControl(TGuiControl* control);
        void RegisterPositionNotificationForWindow(TStyledWindow* window);

        // resets state of control to its default state
        virtual void ResetControl() override
        {
            // Resetujemy stan suwaka tylko jeśli nie jest on przeciągany. Powodem jest to, że na początku każdej pętli gry wszystkie kontrolki są resetowane, żeby poprawnie się odrysowały
            // i przy tej okazji stan przeciągfania byłby utracony uniemożliwiwjąc przeciąganie.
            if (m_DetailedState != CSCROLLBAR_STATE_THUMB_DRAGGING)
            {
                TGuiControl::ResetControl();
                SetDetailedState(CSCROLLBAR_STATE_NORMAL);
            }
        };

        virtual void SetThumbSize() {}
        // Poniższa funkcja zwraca współrzędne (x,y) suwaka dla bieżącej pozycji w przypadku, gdy
        // parametr position jest równy MININT. Jeśli jest różny od MININT, to wyliczone są
        // współrzędne dla podanej pozycji.
        virtual void CalculateThumbLeftTopCorner(float& x, float& y, int position = INT_MIN) { x = 0; y = 0; }
        // Wylicza najbliższą pozycję (this->_position) dla podanych współrzędnych suwaka.
        virtual int CalculatePositionForThumbCoords(float x, float y) { return 0; }

        virtual bool MouseOnThumb(float localX, float localY) { return false; }
        virtual bool MouseOnArrowUp(float localX, float localY) { return false; }
        virtual bool MouseOnArrowDown(float localX, float localY) { return false; }
        virtual bool MouseOnBgUp(float localX, float localY) { return false; }
        virtual bool MouseOnBgDown(float localX, float localY) { return false; }
        virtual bool MouseOnArrowLeft(float localX, float localY) { return false; }
        virtual bool MouseOnArrowRight(float localX, float localY) { return false; }
        virtual bool MouseOnBgLeft(float localX, float localY) { return false; }
        virtual bool MouseOnBgRight(float localX, float localY) { return false; }

        virtual inline int GetMinValue() const { return m_MinValue; }
        virtual inline int GetMaxValue() const { return m_MaxValue; }
        virtual inline int GetThumbPosition() const { return m_ThumbPosition; }

        template<typename T>
        void SetOnPositionChangeCallback(void(T::* Func)(void*), T* Instance)
        {
            OnPositionChangeCallback = std::bind(Func, Instance, std::placeholders::_1);
        }
        void SetOnPositionChangeCallback(void(*Func)(void*))
        {
            OnPositionChangeCallback = std::bind(Func, std::placeholders::_1);
        }
        virtual bool ResetControlState(int StatesToClear = CONTROL_STATE_HOVER) override
        {
            bool Result = Tilc::Gui::TGuiControl::ResetControlState(StatesToClear);
            if (m_DetailedState >= CSCROLLBAR_STATE_HOVER_UP_BUTTON && m_DetailedState <= CSCROLLBAR_STATE_HOVER_THUMB)
            {
                m_DetailedState = CSCROLLBAR_STATE_NORMAL;
                Invalidate();
                Result = true;
            }
            return Result;
        };

    protected:
        int m_ThumbPosition;
        int m_MinValue;
        int m_MaxValue;
        int m_Type;
        // Wielkość suwaka paska. Zależy od wielkości różnicy pomiędzy this->_minValue a this->_maxValue paska.
        int m_ThumbSize;
        // wielkość o jaką przeswany jest suwak po kliknięciu strzałki paska
        int m_SmallStep;
        // wielkość o jaką przeswany jest suwak po kliknięciu tła pod lub nad nim
        int m_LargeStep;
        // stan w który przeszliśmy podczas ostatniego wcśnięcia przycisku myszki
        int m_StateInitiatedByMouseDown;
        // współrzędna x-owa względem lewego górnego rogu suwaka paska w momencie rozpoczęcia
        // przeciągania suwaka (kliknięcia lewym przyciskiem myszki na suwaku)
        float m_ThumbOffsetX;
        // współrzędna y-owa względem lewego górnego rogu suwaka paska w momencie rozpoczęcia
        // przeciągania suwaka (kliknięcia lewym przyciskiem myszki na suwaku)
        float m_ThumbOffsetY;

        std::function<void(void*)> OnPositionChangeCallback;

        // Listy wskaźniki na obiekty dziedziczące po klasie CSprite lub CStyledWindow, do których
        // należy wysyłać powiadomienia o zmianie pozycji suwaka. Przechowywanych tu wskaźników
        // nie zwalniamy.
        std::list<TGuiControl*> m_ControlsToNotify;
        std::list<TStyledWindow*> m_WindowsToNotify;

        // Funkcja wywoływana po każdej zmianie wartości atrybutu this->_position
        void OnPositionChanged(int oldPosition);
    };

}
