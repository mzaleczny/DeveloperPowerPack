#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/StdObject.h"
#include "Tilc/Game.h"
#include "SDL3/SDL.h"
#include <vector>
#include <list>
#include <unordered_map>

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

        constexpr const int CONTROL_DEFAULT_SPACING = 4;
        constexpr const int CONTROL_TEXT_PLACEMENT_LEFT = 1;
        constexpr const int CONTROL_TEXT_PLACEMENT_RIGHT = 2;

        enum class ENeedUpdate
        {
            ENU_None,
            ENU_Everything,
            ENU_Caption,
            ENU_CaptionButtons,
            ENU_Children,
            ENU_Caret
        };

        enum class EControlType
        {
            ECT_WindowControl,
            ECT_Menu,
            ECT_ScrollBar,
            ECT_Slider,
            ECT_Button,
            ECT_Label,
            ECT_TextField,
            ECT_MultilineTextField,
            ECT_Checkbox,
            ECT_Option,
            ECT_Grid,
            ECT_Listbox,
            ECT_MultiColumnListbox,
            ECT_ComboBox,
            ECT_PanelMenu
        };

        enum class EControlBorderType
        {
            ECBT_None,
            ECBT_Solid
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
            SDL_FPoint m_Size;
            TStringVector m_Columns;
        };
        using TGuiControlItemList = std::vector<TGuiControlItem*>;

        class DECLSPEC TGuiControl
        {
        public:
            // Event Handlers
            std::function<int(float x, float y, int MouseButton, TGuiControl* Self)> OnClick;

            // Current position of control inside window
            SDL_FRect m_Position;
            // Current real position of control inside window. It is calculated by adding all parents positions
            SDL_FRect m_RealPosition;
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
            virtual void Draw(SDL_Texture* Canvas, SDL_FRect* Position) {};
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
                SDL_FRect r{ m_RealPosition.x + x, m_RealPosition.y + y, static_cast<float>(Src->w), static_cast<float>(Src->h) };
                // If canvas is specified then we draw within the canvas without offsetting
                if (m_Canvas)
                {
                    r.x -= m_RealPosition.x;
                    r.y -= m_RealPosition.y;
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
                SDL_FRect r{ m_RealPosition.x + x, m_RealPosition.y + y, static_cast<float>(SrcRect->w), static_cast<float>(SrcRect->h) };
                // If canvas is specified then we draw within the canvas without offsetting
                if (m_Canvas)
                {
                    r.x -= m_RealPosition.x;
                    r.y -= m_RealPosition.y;
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
                SDL_FRect r{ m_RealPosition.x + x, m_RealPosition.y + y, destW, destH };
                // If canvas is specified then we draw within the canvas without offsetting
                if (m_Canvas)
                {
                    r.x -= m_RealPosition.x;
                    r.y -= m_RealPosition.y;
                }
                if (RoundCoords)
                {
                    SDL_Rect ir = FloorFRect(&r);
                    SDL_RectToFRect(&ir, &r);
                }
                SDL_RenderTexture(GetRenderer(), Src, nullptr, &r);
            }
            inline void RenderTexture(SDL_Texture* Src, SDL_FRect* SrcRect, float x, float y, float destW, float destH, bool RoundCoords = true)
            {
                SDL_FRect r{ m_RealPosition.x + x, m_RealPosition.y + y, destW, destH };
                // If canvas is specified then we draw within the canvas without offsetting
                if (m_Canvas)
                {
                    r.x -= m_RealPosition.x;
                    r.y -= m_RealPosition.y;
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
            virtual void SetText(const Tilc::TExtString& Text)
            {
                m_Text = Text;
                Invalidate();
            }
            bool PointIn(float x, float y);
            void Play(bool forward = true);
            virtual void SetPosition(float x, float y)
            {
                m_Position.x = x;
                m_Position.y = y;
            }
            virtual void SetSize(float width, float height);
            void SetMaxAvailableSizeOfScrollBars();
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
            // Draw All children default without vertical scrollbar and horizontal scrollbar id both present
            virtual void DrawChildren(bool SkipVerticalAndHorizontalScrollBars = true);
            virtual void DrawVerticalAndHorizontalScrollBars();
            virtual void DrawChildWindows();
            virtual void InvalidateAllChildren();

            // zwraca okno wewnątrz którego znajduje się ta kontrolka.
            TStyledWindow* GetParentWindow();

            // Zwraca sprite'a o podanej nazwie
            TGuiControl* GetChildByName(const Tilc::TExtString& name);

            // Funkcja ustawiająca focus na kontrolce - możliwość customizacji we właściwej kontrolce
            virtual void Focus();
            // Funkcja odbierająca kontrolce focus - możliwość customizacji we właściwej kontrolce
            virtual void LooseFocus();
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
            virtual bool OnKeyDown(const SDL_Event& event);

            // Funkcja wywoływana w momencie wprowadzenia tekstu.
            virtual bool OnTextInput(const SDL_Event& event)
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

            virtual void AddScrollBars(bool vertical, bool horizontal, int vmin, int vmax, int hmin, int hmax, bool IsStandardSizedScrollBar = true);
            virtual void AddVerticalScrollBar(int min, int max, int size = 0, bool IsStandardSizedScrollBar = true);
            virtual bool RemoveVerticalScrollBar();
            virtual void AddHorizontalScrollBar(int min, int max, int size = 0, bool IsStandardSizedScrollBar = true);
            virtual bool RemoveHorizontalScrollBar();
            virtual void AddEditor();
            virtual bool RemoveEditor();

            virtual Tilc::TExtString GetValue() { return {}; }
            virtual void SetValue(const Tilc::TExtString& value, bool redraw = true) {}
            virtual void Edit();
            virtual void OnEditorShow() {};
            virtual void EndEdit(bool acceptChanges);
            // Do not delete value argument inside below method
            virtual void OnApplyEditorChanges(const Tilc::TExtString& value) {};

            virtual void ResetEditor();
            virtual void SetEditorValue(const Tilc::TExtString& value);

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
            inline static Tilc::Gui::TGuiControl* GetLastClickedControl() { return m_LastClickedControl; }

            // Reset control and all it's children to default state
            void ResetToDefaultState();

            virtual void MoveTabStopForward();
            virtual void MoveTabStopBackward();
            virtual bool SetActiveControl(TGuiControl* Control);
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

            SDL_Texture* m_Bg{};
            void LoadBackground(const Tilc::TExtString& Filename);

            float m_Alpha = 1.0f;
            void inline SetAlpha(float Alpha)
            {
                m_Alpha = Alpha;
                Invalidate();
            }

            // Two function below mark and unmark control as in updating state. If control is in updating state, not processes Invalidate function.
            void inline BeginUpdate() { m_BeginUpdate = true; }
            void inline EndUpdate()
            {
                m_BeginUpdate = false;
                Invalidate();
            }

            void DrawCommon(
                const SDL_FRect& Position,
                const SDL_FRect& rc,
                const SDL_FRect& disabled_rc,
                const SDL_FRect& focused_rc,
                const SDL_FRect& hover_focused_rc,
                const SDL_FRect& pushed_focused_rc,
                const SDL_FRect& hover_rc,
                const SDL_FRect& pushed_rc
            );

            void DrawCommon(
                const SDL_FRect& Position,
                const SDL_FRect& left_rc, const SDL_FRect& middle_rc, const SDL_FRect& right_rc,
                const SDL_FRect& left_disabled_rc, const SDL_FRect& middle_disabled_rc, const SDL_FRect& right_disabled_rc,
                const SDL_FRect& left_focused_rc, const SDL_FRect& middle_focused_rc, const SDL_FRect& right_focused_rc,
                const SDL_FRect& left_hover_focused_rc, const SDL_FRect& middle_hover_focused_rc, const SDL_FRect& right_hover_focused_rc,
                const SDL_FRect& left_pushed_focused_rc, const SDL_FRect& middle_pushed_focused_rc, const SDL_FRect& right_pushed_focused_rc,
                const SDL_FRect& left_hover_rc, const SDL_FRect& middle_hover_rc, const SDL_FRect& right_hover_rc,
                const SDL_FRect& left_pushed_rc, const SDL_FRect& middle_pushed_rc, const SDL_FRect& right_pushed_rc
            );

            void DrawCommonComplex(
                const SDL_FRect& Position,
                const SDL_FRect& top_left_rc, const SDL_FRect& top_middle_rc, const SDL_FRect& top_right_rc, const SDL_FRect& inner_left_rc, const SDL_FRect& inner_right_rc, const SDL_FRect& bottom_left_rc, const SDL_FRect& bottom_middle_rc, const SDL_FRect& bottom_right_rc,
                const SDL_FRect& top_left_disabled_rc, const SDL_FRect& top_middle_disabled_rc, const SDL_FRect& top_right_disabled_rc, const SDL_FRect& inner_left_disabled_rc, const SDL_FRect& inner_right_disabled_rc, const SDL_FRect& bottom_left_disabled_rc, const SDL_FRect& bottom_middle_disabled_rc, const SDL_FRect& bottom_right_disabled_rc,
                const SDL_FRect& top_left_focused_rc, const SDL_FRect& top_middle_focused_rc, const SDL_FRect& top_right_focused_rc, const SDL_FRect& inner_left_focused_rc, const SDL_FRect& inner_right_focused_rc, const SDL_FRect& bottom_left_focused_rc, const SDL_FRect& bottom_middle_focused_rc, const SDL_FRect& bottom_right_focused_rc,
                const SDL_FRect& top_left_hover_focused_rc, const SDL_FRect& top_middle_hover_focused_rc, const SDL_FRect& top_right_hover_focused_rc, const SDL_FRect& inner_left_hover_focused_rc, const SDL_FRect& inner_right_hover_focused_rc, const SDL_FRect& bottom_left_hover_focused_rc, const SDL_FRect& bottom_middle_hover_focused_rc, const SDL_FRect& bottom_right_hover_focused_rc,
                const SDL_FRect& top_left_pushed_focused_rc, const SDL_FRect& top_middle_pushed_focused_rc, const SDL_FRect& top_right_pushed_focused_rc, const SDL_FRect& inner_left_pushed_focused_rc, const SDL_FRect& inner_right_pushed_focused_rc, const SDL_FRect& bottom_left_pushed_focused_rc, const SDL_FRect& bottom_middle_pushed_focused_rc, const SDL_FRect& bottom_right_pushed_focused_rc,
                const SDL_FRect& top_left_hover_rc, const SDL_FRect& top_middle_hover_rc, const SDL_FRect& top_right_hover_rc, const SDL_FRect& inner_left_hover_rc, const SDL_FRect& inner_right_hover_rc, const SDL_FRect& bottom_left_hover_rc, const SDL_FRect& bottom_middle_hover_rc, const SDL_FRect& bottom_right_hover_rc,
                const SDL_FRect& top_left_pushed_rc, const SDL_FRect& top_middle_pushed_rc, const SDL_FRect& top_right_pushed_rc, const SDL_FRect& inner_left_pushed_rc, const SDL_FRect& inner_right_pushed_rc, const SDL_FRect& bottom_left_pushed_rc, const SDL_FRect& bottom_middle_pushed_rc, const SDL_FRect& bottom_right_pushed_rc
            );

            bool IsCaretMovingKey(unsigned int virtualCode);

            void SetTickable(bool IsTickable);
            bool IsTickble() const {
                return m_IsTickable;
            };

            // Lista kontrolek, dla których wywoływana jest metda update
            inline static std::list<Tilc::Gui::TGuiControl*> m_TickableControls{};
            // domyślnie kontrolka nie jest Tickable, czyli ni jest dodawana do powyższej listy.
            bool m_IsTickable{};
            // Lista kontrolek rysowanych po narysowaniu wszystkich innych kontrolek. Używana jest do tego, żeby:
            // 1. DropDowny combobox-ów zawsze rysowały się na wierzchu.
            // Kontrolki te mają także prorytet podczas przetwarzania zdarzeń. Czyli jeśli jedna z nich przesłania inną kontrolkę, to przechwyci ona zdarzenia nawet jeśli ta
            // zasłonieta kontrolka normalnie miałaby priorytet.
            inline static std::list<Tilc::Gui::TGuiControl*> m_HighPrivilegedControls;
            // Poniższa lista słży do usuwania kontrolek z listy wyżej: m_HighPrivilegedControls. Jest to zrobione tak dlatego, że kontrolki priorytetowe mogą być usuwane
            // podczas przetwarzania zdarzenia myszy np. kliknięcia. I usunięcie tej kontrolki z listy, gdy ta jest przetwarzana powodowało crash. Więc kontrolkę, którą
            // chcemy usunąć dodajemy do tej listy m_HighPrivilegedControlsToRemove i są one następnie usuwane z m_HighPrivilegedControls po zakończeniu przetwarzania
            // zdarzeń.
            inline static std::list<Tilc::Gui::TGuiControl*> m_HighPrivilegedControlsToRemove;

            virtual void SetScrollBars() {};
            float GetControlHeightFromTheme(Tilc::Gui::EControlType ControlType);

        protected:
            SDL_Texture* m_Canvas{};
            TStyledWindow* m_ParentWindow{};
            TGuiControl* m_Parent{};
            Tilc::Game2D::Sprite::TDirectionalAnimation* m_Animation = nullptr;
            SDL_FPoint m_ClientPos;
            Tilc::TExtString m_Name;
            Tilc::TExtString m_Text;
            // Aktywna kontrolka - ta która aktualnie ma focus
            TGuiControl* m_ActiveControl{};
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
            // if control has editor. Editor is a TextField that appear inside control to edit holded text data in cells/columns etc. By default control has no editor.
            bool m_HasEditor{};
            // if we are editor control
            bool m_IsEditor{};
            // if caret position must be updated
            bool m_UpdateCaretPos{};

            // Zmienne poniżej kontrolują przewijanie zawartości okienka w zależności od wartości suwaków pionowego i poziomego
            float m_OffsetX{};
            float m_OffsetY{};

            // List of child controls
            std::list<TGuiControl*> m_Children;

            bool m_BeginUpdate{};

            // Wskaźnik na sprite'a, na którym zarezerwowano zdarzenia WM_MOUSE (w wyniku kliknięcia na nim).
            // Jeśli jest różny od NULL, to inne Sprite'y powinny ignorować zdarzenia myszki.
            static TGuiControl* m_ControlThatCapturedMouse;
            // Wskazuje kontrolkę, która została ostatnio kliknięta. Zmienna ta nigdy nie jest zerowana i zawsze jest ustawiana w funckji MouseDown klasy TGuiControl.
            inline static TGuiControl* m_LastClickedControl{};

            void CommonInit(bool editable);
            void DestroyCanvasIfNeedDestroy();

            void MoveAllSubWindowsToTheEndOfGlobalWindowsOrder();
        };
    }
}
