#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/StdObject.h"
#include "Tilc/Game.h"
#include "SDL3/SDL.h"
#include <list>
#include <unordered_map>

constexpr const int CONTROL_STATE_NORMAL = 1;
constexpr const int CONTROL_STATE_HOVER = 2;
constexpr const int CONTROL_STATE_PUSHED = 4;
constexpr const int CONTROL_STATE_FOCUSED = 8;
constexpr const int CONTROL_STATE_DISABLED = 16;
constexpr const int CONTROL_STATE_ACTIVE = 32;
constexpr const int CONTROL_STATE_UPDATE_CURSOR_POS_ACCORDING_MOUSE_POS = 64;

constexpr const int MOUSE_EVENT_ONMOUSEDOWN = 1;
constexpr const int MOUSE_EVENT_ONMOUSEMOVE = 2;
constexpr const int MOUSE_EVENT_ONMOUSEUP = 3;

namespace Tilc {
    namespace Game2D {
        namespace Sprite {
            class TDirectionalAnimation;
        }
    }
    namespace Gui {

        class TCursor;
        class TKeyboard {};
        class TScrollBar;
        class TTheme;
        class TStyledWindow;

        enum class ENeedUpdate
        {
            ENU_None,
            ENU_Everything,
            ENU_Caption,
            ENU_CaptionButtons,
            ENU_Children
        };

        enum class EControlType
        {
            ECT_RegularControl,
            ECT_ScrollBarControl,
            ECT_WindowControl
        };
        class DECLSPEC TGuiControlItem
        {
        public:
            TGuiControlItem(const Tilc::TExtString& value, bool selected = false, bool checked = false, int imageIndex = -1, long long data = 0);

            Tilc::TExtString m_Value;
            bool m_Selected;
            bool m_Checked;
            int m_ImageIndex;
            long long m_Data;
            size_t m_Size;
        };

        class DECLSPEC TGuiControl
        {
        public:
            // Event Handlers
            std::function<int(float x, float y, int MouseButton, TGuiControl* Self)> OnClick;

            // Current position of control inside window
            SDL_FRect m_Position;
            // Original position of control inside window. This variable is used for calculating m_Position for children during scrolling vertical or horizontal scrollbar of parent
            SDL_FRect m_OriginalPosition;
            // m_PrevPosition is for the sake of handling window maximization and restore
            SDL_FRect m_PrevPosition;
            // COntrolType
            EControlType m_ControlType;

            // Below variable IsWindow tell us if we are adding StyledWindow or regular Control, because dynamic_cast to TStyledWindow class in constructor not works
            TGuiControl(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, EControlType ControlType, bool editable = false);
            virtual ~TGuiControl();

            virtual void Destroy();
            virtual void DestroyChildren();
            virtual void DestroyChildWindows();
            virtual void Draw() {};
            virtual void Draw(float x, float y,
                SDL_FRect* frame_top_left_rc, SDL_FRect* frame_top_rc, SDL_FRect* frame_top_right_rc,
                SDL_FRect* frame_bottom_left_rc, SDL_FRect* frame_bottom_rc, SDL_FRect* frame_bottom_right_rc,
                SDL_FRect* frame_left_rc, SDL_FRect* frame_right_rc, SDL_FRect* bg_rc);

            inline SDL_Rect FloorFRect(SDL_FRect* Rect)
            {
                SDL_Rect r;
                r.x = static_cast<int>(floor(Rect->x));
                r.y = static_cast<int>(floor(Rect->y));
                r.w = static_cast<int>(floor(Rect->w));
                r.h = static_cast<int>(floor(Rect->h));
                return r;
            }
            inline void RenderTexture(SDL_Texture* Src, float x, float y, bool RoundCoords = true)
            {
                SDL_FRect r{ m_Position.x + x, m_Position.y + y, static_cast<float>(Src->w), static_cast<float>(Src->h) };
                // If canvas is specified then we draw within the canvas without offsetting
                if (m_Canvas)
                {
                    r.x -= m_Position.x;
                    r.y -= m_Position.y;
                }
                if (RoundCoords)
                {
                    SDL_Rect ir = FloorFRect(&r);
                    SDL_RectToFRect(&ir, &r);
                }
                SDL_RenderTexture(GetRenderer(), Src, nullptr, &r);
            }
            inline void RenderTexture(SDL_Texture* Src, SDL_FRect* SrcRect, float x, float y, bool RoundCoords = true)
            {
                SDL_FRect r{ m_Position.x + x, m_Position.y + y, static_cast<float>(SrcRect->w), static_cast<float>(SrcRect->h) };
                // If canvas is specified then we draw within the canvas without offsetting
                if (m_Canvas)
                {
                    r.x -= m_Position.x;
                    r.y -= m_Position.y;
                }
                if (RoundCoords)
                {
                    SDL_FRect FloorSrcRect;
                    SDL_Rect iSrcRect = FloorFRect(SrcRect);
                    SDL_RectToFRect(&iSrcRect, &FloorSrcRect);
                    SDL_Rect ir = FloorFRect(&r);
                    SDL_RectToFRect(&ir, &r);
                    SDL_RenderTexture(GetRenderer(), Src, &FloorSrcRect, &r);
                }
                else
                {
                    SDL_RenderTexture(GetRenderer(), Src, SrcRect, &r);
                }
            }
            inline void RenderTexture(SDL_Texture* Src, float x, float y, float destW, float destH, bool RoundCoords = true)
            {
                SDL_FRect r{ m_Position.x + x, m_Position.y + y, destW, destH };
                if (RoundCoords)
                    // If canvas is specified then we draw within the canvas without offsetting
                    if (m_Canvas)
                    {
                        r.x -= m_Position.x;
                        r.y -= m_Position.y;
                    }
                {
                    SDL_Rect ir = FloorFRect(&r);
                    SDL_RectToFRect(&ir, &r);
                }
                SDL_RenderTexture(GetRenderer(), Src, nullptr, &r);
            }
            inline void RenderTexture(SDL_Texture* Src, SDL_FRect* SrcRect, float x, float y, float destW, float destH, bool RoundCoords = true)
            {
                SDL_FRect r{ m_Position.x + x, m_Position.y + y, destW, destH };
                // If canvas is specified then we draw within the canvas without offsetting
                if (m_Canvas)
                {
                    r.x -= m_Position.x;
                    r.y -= m_Position.y;
                }
                if (RoundCoords)
                {
                    SDL_FRect FloorSrcRect;
                    SDL_Rect iSrcRect = FloorFRect(SrcRect);
                    SDL_RectToFRect(&iSrcRect, &FloorSrcRect);
                    SDL_Rect ir = FloorFRect(&r);
                    SDL_RectToFRect(&ir, &r);
                    SDL_RenderTexture(GetRenderer(), Src, &FloorSrcRect, &r);
                }
                else
                {
                    SDL_RenderTexture(GetRenderer(), Src, SrcRect, &r);
                }
            }
            inline void RenderTiledTexture(SDL_Texture* Src, SDL_FRect* SrcRect, SDL_FRect* DestRect, bool RoundCoords = true)
            {
                if (RoundCoords)
                {
                    SDL_FRect FloorSrcRect, FloorDestRect;
                    SDL_Rect iSrcRect = FloorFRect(SrcRect);
                    SDL_RectToFRect(&iSrcRect, &FloorSrcRect);
                    SDL_Rect iDestRect = FloorFRect(DestRect);
                    SDL_RectToFRect(&iDestRect, &FloorDestRect);
                    SDL_RenderTextureTiled(GetRenderer(), Src, &FloorSrcRect, 1.0f, &FloorDestRect);
                }
                else
                {
                    SDL_RenderTextureTiled(GetRenderer(), Src, SrcRect, 1.0f, DestRect);
                }
            }
            virtual Tilc::TExtString GetName() { return m_Name; }
            virtual Tilc::TExtString GetText() { return m_Text; }
            virtual void SetText(const Tilc::TExtString& Text, bool redraw = true)
            {
                m_Text = Text;
                if (redraw)
                {
                    m_NeedUpdate = ENeedUpdate::ENU_Everything;
                }
            }
            bool PointIn(float x, float y);
            void Play(bool forward = true);
            virtual void SetPosition(float x, float y)
            {
                m_Position.x = x;
                m_Position.y = y;
            }
            virtual void SetSize(float width, float height)
            {
                m_Position.w = width;
                m_Position.h = height;
            }
            bool IsPlaying() const;
            void Stop();
            void Loop();
            bool IsLooping() const;
            void inline SetRewind(bool v) { m_Rewind = v; }
            bool inline IsRewind() const { return m_Rewind; }

            // resets state of control to its default state
            virtual void ResetControl()
            {
                SetState(CONTROL_STATE_NORMAL);
                m_Dragging = false;
                m_DetailedState = 0;
            };
            void ResetControls(bool Recursively = true);
            // Resets states, by default hover
            virtual bool ResetControlState(int StatesToClear = CONTROL_STATE_HOVER)
            {
                if (HasState(StatesToClear))
                {
                    RemoveState(StatesToClear);
                    return true;
                }
                return false;
            };
            bool ResetControlsState(int StatesToClear = CONTROL_STATE_HOVER, bool Recursively = true);

            virtual void SetSizeRelativeToParent(float parentWidth, float parentHeight) {}
            void SetProportionalSizeRelativeToParent();
            void SetCanvas(SDL_Texture* canvas);
            inline bool GetAutosize() const { return m_Autosize; };
            inline void SetAutoSize(bool autosize) { m_Autosize = autosize; };
            void inline Show()
            {
                if (!m_Visible)
                {
                    m_Visible = true;
                    m_NeedUpdate = ENeedUpdate::ENU_Everything;
                }
            }
            virtual void Hide();
            bool IsVisible() const { return m_Visible; }
            // Funkcja zwraca TRUE, jesli można do tego sprite'a przejść przy pomocy klawisza TabStop
            // (tzn. przekazać mu focus)
            bool CanTabStop() const { return m_Visible && m_Enabled && m_TabStop; }
            void SetTabStop(bool tabStop) { m_TabStop = tabStop; }

            bool IsEnabled() const { return m_Enabled; }
            virtual void Enable()
            {
                m_Enabled = true;
                SetState(CONTROL_STATE_NORMAL);
                m_NeedUpdate = ENeedUpdate::ENU_Everything;
            }
            virtual void Disable()
            {
                m_Enabled = false;
                SetState(CONTROL_STATE_DISABLED);
                m_NeedUpdate = ENeedUpdate::ENU_Everything;
            }

            void EnableTabkey() { m_DisableTabkey = false; }
            void DisableTabkey() { m_DisableTabkey = true; }
            bool IsTabkeyEnabled() { return !m_DisableTabkey; }

            void DoEnterAsTabkey(bool value) { m_DoEnterAsTabkey = value; }
            bool IsEnterAsTabkey() const { return m_DoEnterAsTabkey; }

            inline TGuiControl* GetParent() { return m_Parent; }
            inline void SetParent(TGuiControl* Parent) { m_Parent = Parent; }
            virtual void DrawChildren();
            virtual void DrawChildWindows();
            virtual void InvalidateAllChildren();

            // zwraca okno wewnątrz którego znajduje się ta kontrolka.
            TStyledWindow* GetParentWindow();

            // Zwraca sprite'a o podanej nazwie
            TGuiControl* GetChildByName(const Tilc::TExtString& name);

            // Funkcja ustawiająca focus na kontrolce - implementacja we właściwej kontrolce
            virtual void Focus()
            {
                Invalidate();
            }
            // Funkcja odbierająca kontrolce focus - implementacja we właściwej kontrolce
            virtual void LooseFocus()
            {
                Invalidate();
            }
            void SetState(int state, bool redraw = true);
            void SetDetailedState(int state, bool redraw = true);
            // dodaje do bieżącego stanu podany stan (wartości stanów muszą być potęgami dwójki, gdyż
            // operacja ta jest realizowana jako orowanie podanego i aktualnego stanu)
            void AddState(int state, bool redraw = true);
            // usuwa z bieżącego stanu podany stan (wartości stanów muszą być potęgami dwójki, gdyż
            // operacja ta jest realizowana jako andowanie zaprzeczenia podanego stanu i aktualnego stanu)
            void RemoveState(int state, bool redraw = true);
            inline bool HasState(int states) { return (m_State & states) != 0; }
            inline bool HasExactState(int state) { return m_State == state; }
            // Ustawia podaną wartość tekstową w spricie o podanej nazwie znajdującym się w oknie o podanej
            // nazwie. Jeśli jako nazwę okna podano pusty łańcuch, to docelowy sprite jest wyszukiwany
            // w oknie w którym leży ten sprite.
            // Metoda zwraca true jeśli docelowy sprite został znaleziony i ustawiono mu tekst.
            bool SetTextForSprite(Tilc::TExtString value, Tilc::TExtString spriteName, Tilc::TExtString wndName = "", bool redraw = true);
            // jeśli ta metoda zwróci true, to należy przerysować okienko
            virtual bool Update(float DeltaTime);

            virtual void AddChild(TGuiControl* child);
            virtual void PrependChild(TGuiControl* child);

            virtual size_t RemoveChild(TGuiControl* child);
            virtual inline void RemoveChild(Tilc::TExtString childName)
            {
                TGuiControl* c = GetChild(childName);
                if (c)
                {
                    RemoveChild(c);
                }
            }
            inline std::list<TGuiControl*>& GetChildren() { return m_Children; }

            // jeśli ta metoda zwróci false, to w zdarzeniu onMouseMove okna zostanie
            // sprawdzone czy podany punkt leży wewnątrz sprite'a i jeśli tak, to zostanie
            // uruchomiona animacja sprite'a. W przeciwnym razie animacja nie jest
            // odgrywana. Jeśli ta metoda zwróci true, to znaczy, że została już wykonana
            // jakaś akcja wewnątrz sprite'a uwzględniająca aktualną pozycję kursora myszy.
            virtual bool OnMouseMove(const SDL_Event& event);

            // Jeśli metoda zwróci true, to znaczy że sprite obsłużył to zdarzenie i nie zostanie
            // ono przekazane dalej.
            virtual bool OnMouseButtonDown(const SDL_Event& event);

            // Jeśli metoda zwróci true, to znaczy że sprite obsłużył to zdarzenie i nie zostanie
            // ono przekazane dalej.
            virtual bool OnMouseButtonUp(const SDL_Event& event);

            // Funkcja wywoływana w momencie wciśnięcia klawisza. Jeśli klawisz jest przytrzymany jako
            // wciśnięty, to za każdym następnym razem wywoływana jest tylko funkcja onKeyPressed.
            // Jeśli wciśnięto tylko klawisz systemowy jak np. ALT, SHIFT, CONTROL to argument
            // systemKey jest równy true.
            virtual bool OnKeyDown(const SDL_Event& event)
            {
                return false;
            }
            // Funkcja wywoływana w momencie zwolnienia wciśniętego klawisza
            // Jeśli wciśnięto tylko klawisz systemowy jak np. ALT, SHIFT, CONTROL to argument
            // systemKey jest równy true.
            virtual bool OnKeyUp(const SDL_Event& event)
            {
                return false;
            }

            virtual bool OnEscDown() { return false; }
            virtual bool OnEscPressed();
            virtual bool OnEscUp() { return false; }
            virtual bool OnEnterDown() { return false; }
            virtual bool OnEnterPressed();
            virtual bool OnEnterUp() { return false; }
            virtual bool OnTabDown() { return false; }
            virtual bool OnTabPressed() { return false; }
            virtual bool OnTabUp() { return false; }
            // method onChildNotification is called by the child contrl if any event occur during the time the child
            // has focus. It is for example used for notify parent that editing was ended either by ESC or Enter
            // keypressing.
            virtual int OnChildNotification(TGuiControl* child, uintptr_t data);

            // Functions that delegates mouse events to child controls
            virtual bool ProcessChildEvent(const SDL_Event& event);
            virtual bool ProcessEvent(const SDL_Event& event);

            virtual void Invalidate(ENeedUpdate WhatNeedUpdate = ENeedUpdate::ENU_Everything);

            // Funkcja wywoływana przez scrollbar który jest podczepiony do tego obiektu w momencie zmiany
            // jego pozycji (tj położenia suwaka).
            virtual void OnThumbChange(int oldPosition, int curPosition, TScrollBar* scrollbar) { return; }

            void SetEditable(bool value);

            virtual void AddScrollbars(bool vertical, bool horizontal, int vmin, int vmax, int hmin, int hmax);
            virtual void AddVerticalScrollbar(int min, int max, int size = 0);
            virtual bool RemoveVerticalScrollbar();
            virtual void AddHorizontalScrollbar(int min, int max, int size = 0);
            virtual bool RemoveHorizontalScrollbar();
            virtual void AddEditor();
            virtual bool RemoveEditor();

            virtual Tilc::TStdObject GetValue() { return {}; }
            virtual void SetValue(const Tilc::TStdObject& value, bool redraw = true) {}
            virtual void SetValue(const Tilc::TExtString& value, bool redraw = true) {}
            virtual void Edit();
            virtual void OnEditorShow() {};
            virtual void EndEdit(bool acceptChanges);
            // Do not delete value argument inside below method
            virtual void OnApplyEditorChanges(Tilc::TStdObject* value) {};

            virtual void ResetEditor();
            virtual void SetEditorValue(Tilc::TStdObject* value);

            TGuiControl* GetChild(const Tilc::TExtString& name);

            static inline SDL_Renderer* GetRenderer()
            {
                if (Renderer) return Renderer;
                Renderer = Tilc::GameObject->GetContext()->m_Window->GetRenderer();
                return Renderer;
            }
            static TTheme* GetTheme() { return Tilc::GameObject->GetContext()->m_Theme; }
            static TCursor* GetCursor() { return nullptr; }
            static SDL_Renderer* Renderer;

            // Funkcja zwraca bieżącą pozycję kursora względem lewego górnego narożnika kontrolki
            void GetCurrentMousePosition(float* x, float* y);
            // Zwraca współrzędne sprite'a względem lewego górnego rogu canvasu okna
            void GetPositionInWindow(float* x, float* y);

            virtual void RecreateCanvasForCurrentSize();

            // Method onGetEditedValues is called after child window is closed by button of name:
            // DEFAULT_BTN_OK or DEFAULT_BTN_CANCEL
            // In first case we get as a parameter map CMap object of type: TExtString (name) => TExtString (value)
            //   After processing map object passed to the function should not be deleted because it is done automatically
            // In second case we get empty map.
            virtual void OnGetEditedValues(std::unordered_map<TExtString, TExtString>& map) {}

            // Returns true if other control captured mouse events
            bool OtherControlCapturedMouse();
            inline void CaptureMouse(Tilc::Gui::TGuiControl* Control) {
                if (!Control && m_ControlThatCapturedMouse)
                {
                    //std::cout << "RemoveCapture: " << m_ControlThatCapturedMouse->m_Name << std::endl;
                    m_ControlThatCapturedMouse->SetState(CONTROL_STATE_NORMAL);
                    m_ControlThatCapturedMouse->m_DetailedState = 0;
                }
                m_ControlThatCapturedMouse = Control;
            }
            inline bool IsMouseCaptured() { return m_ControlThatCapturedMouse != nullptr; }
            inline static Tilc::Gui::TGuiControl* GetControlThatCapturedMouse() { return m_ControlThatCapturedMouse; }

            // Reset control and all it's children to default state
            void ResetToDefaultState();

            virtual void SetActiveControl(TGuiControl* Control);
            inline TGuiControl* GetActiveControl() { return m_ActiveControl; }
            inline void SetOnlyActiveControlPointer(Tilc::Gui::TGuiControl* Control) { m_ActiveControl = Control; }

            // Variable controlling dragging
            float m_DragStartX{};
            float m_DragStartY{};
            bool m_Dragging{};

            // Attribute _vscrollbar is redundant. It is for quicker access to vertical scrollbar which is in in _sprites
            // list. It is destroyed during destroying the list.
            TScrollBar* m_VScrollBar{};
            // Attribute _hscrollbar is redundant. It is for quicker access to horizontal scrollbar which is in _sprites
            // list. It is destroyed during destroying the list.
            TScrollBar* m_HScrollBar{};
            // Attribute _editor is redundant. It is for quicker access to an editor which is in _sprites list.
            // It is destroyed during destroying the list.
            TGuiControl* m_Editor{};

            SDL_FRect GetRealPosition();
            void SetOffsetX(float Offset);
            void SetOffsetY(float Offset);
            inline float GetOffsetX() const { return m_OffsetX; }
            inline float GetOffsetY() const { return m_OffsetY; }

            // ustawia aktywne okno
            void SetActiveWindow(Tilc::Gui::TStyledWindow* Window, bool Redraw = true);

            std::list<Tilc::Gui::TStyledWindow*>& AllWindowsList() { return m_AllWindows; }
            Tilc::Gui::TStyledWindow* GetActiveWindow() const { return m_ActiveWindow; }

        protected:
            SDL_Texture* m_Canvas{};
            TStyledWindow* m_ParentWindow{};
            TGuiControl* m_Parent{};
            Tilc::Game2D::Sprite::TDirectionalAnimation* m_Animation = nullptr;
            SDL_FPoint m_ClientPos;
            Tilc::TExtString m_Name;
            Tilc::TExtString m_Text;
            // Aktywna kontrolka - ta która aktualnie ma focus
            TGuiControl* m_ActiveControl;
            // lista wszystkich okien na potrzeby z-orderingu
            std::list<Tilc::Gui::TStyledWindow*> m_AllWindows{};
            // Aktywne okno, pozwala ograniczyc wysylke zdarzen do kontrolek tylko tego okna
            Tilc::Gui::TStyledWindow* m_ActiveWindow{};

            // Liczba okreslajaca z-order kontrolek, czyli ktora ma być wyżej a która ma być pod jeśli zajmują to samo miejsce na ekranie, dotyczy to także okien
            int ZOrder = 1000;

            // stan sprite'a - wykorzystywany tylko przez niektóre klasy dziedziczące
            int m_State;
            // stan szczegółowy sprite'a - wykorzystywany tylko przez niektóre klasy dziedziczące i przechowuje stany zdefiniowane przez daną podklasę
            int m_DetailedState;
            // czy sprite jest widoczny
            bool m_Visible;
            // czy sprite jest dostępny
            bool m_Enabled;
            // czy po zakonczeniu animacji ma byc przeskok do pierwszej klatki
            bool m_Rewind;
            // czy do sprite'a można przejść przy pomocy klawisza Tab lub Shift+Tab
            bool m_TabStop;
            // if TRUE then tab key is disabled if this sprite has focus
            bool  m_DisableTabkey;
            // if TRUE then pressing ENTER key do the same as pressing TAB key
            bool m_DoEnterAsTabkey;
            // if TRUE the control can be resized. FALSE by default.
            bool m_AllowResizing{};

            // poniższy atrybut określa, czy przy zmianie rozmiaru okienka dany sprite ma
            // mieć rozmiar zmieniony relatywnie do swojego parenta
            bool m_Autosize;
            // atrybut określa czy w metodzie onDraw ma być wywołana metoda
            // updateCanvas odpowiedzialna za przerysowanie canvasu
            ENeedUpdate m_NeedUpdate;
            // bitmap filling color when creating canvas, default to this->_transparentColor
            SDL_Color m_BackgroundColor;
            // Czy mamy zwolnić Canvas. Jeśli została wykonana metoda setSize, to tak.
            bool m_DestroyCanvas{ false };

            // if control is editable
            bool m_Editable{};
            // if we are editor control
            bool m_IsEditor{};
            // if caret position must be updated
            bool m_UpdateCaretPos{};

            // Zmienne poniżej kontrolują przewijanie zawartości okienka w zależności od wartości suwaków pionowego i poziomego
            float m_OffsetX{};
            float m_OffsetY{};

            // List of child controls
            std::list<TGuiControl*> m_Children;

            // Wskaźnik na sprite'a, na którym zarezerwowano zdarzenia WM_MOUSE (w wyniku kliknięcia na nim).
            // Jeśli jest różny od NULL, to inne Sprite'y powinny ignorować zdarzenia myszki.
            static TGuiControl* m_ControlThatCapturedMouse;

            void CommonInit(bool editable);
            void DestroyCanvasIfNeedDestroy();

            void MoveAllSubWindowsToTheEndOfGlobalWindowsOrder();
        };
    }
}
