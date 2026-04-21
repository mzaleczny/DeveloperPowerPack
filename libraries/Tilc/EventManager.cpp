#include "EventManager.h"
#include "StateManager.h"
#include "Tilc/Graphics/Camera.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/ScrollBar.h"
#include "Game.h"
#include <iostream>
#include <fstream>
#include <sstream>

Tilc::TEventManager::~TEventManager()
{
	for (auto& it : m_Bindings)
	{
		delete it.second;
		it.second = nullptr;
	}
}

bool Tilc::TEventManager::AddBinding(TBinding* Binding)
{
	if (m_Bindings.find(Binding->m_Name) != m_Bindings.end())
	{
		return false;
	}
	return m_Bindings.emplace(Binding->m_Name, Binding).second;
}

bool Tilc::TEventManager::RemoveBinding(Tilc::TExtString Name)
{
	auto it = m_Bindings.find(Name);
	if (it == m_Bindings.end()) 
	{
		return false;
	}
	delete it->second;
	m_Bindings.erase(it);
	return true;
}

SDL_AppResult Tilc::TEventManager::DefaultHandleEvent(const SDL_Event* Event)
{
	static Tilc::TVector2D<float> LastPos(-1000.0f, -1000.0f);
	static bool MouseDown = false;
	Tilc::Graphics::TCamera* Camera = Tilc::GameObject->GetCurrentCamera();
	if (Event->type == SDL_EVENT_QUIT || Tilc::GameObject->m_Window->IsDone())
	{
		return SDL_APP_SUCCESS;
	}
	else if (Event->type == SDL_EVENT_KEY_DOWN)
	{
		switch (Event->key.key)
		{
		case SDLK_Q:
			Tilc::GameObject->m_Window->Close();
			break;
		case SDLK_UP:
			// Camera Forward
			if (Camera)
			{
				Camera->Move(Tilc::Graphics::ECameraDirection::Forward, Tilc::GameObject->m_LastFrameDeltaTime);
			}
			break;
		case SDLK_DOWN:
			// Camera Backward
			if (Camera)
			{
				Tilc::GameObject->GetCurrentCamera()->Move(Tilc::Graphics::ECameraDirection::Backward, Tilc::GameObject->m_LastFrameDeltaTime);
			}
			break;
		case SDLK_LEFT:
			// Camera Left
			if (Camera)
			{
				Tilc::GameObject->GetCurrentCamera()->Move(Tilc::Graphics::ECameraDirection::Left, Tilc::GameObject->m_LastFrameDeltaTime);
			}
			break;
		case SDLK_RIGHT:
			// Camera Right
			if (Camera)
			{
				Tilc::GameObject->GetCurrentCamera()->Move(Tilc::Graphics::ECameraDirection::Right, Tilc::GameObject->m_LastFrameDeltaTime);
			}
			break;
		}
	}
	else if (Event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
	{
		MouseDown = true;
    }
	else if (Event->type == SDL_EVENT_MOUSE_BUTTON_UP)
	{
		MouseDown = false;
    }
    else if (Event->type == SDL_EVENT_MOUSE_MOTION)
    {
        Tilc::TVector2D<float> Pos;
        SDL_GetGlobalMouseState(&Pos.x, &Pos.y);
        // Absolute values below prevents cube displacement in situations when for example cursor moves to the left of the window and enters to the right of
        // the window again. The same in y axis top/down
        if (LastPos.x < -999.0f || fabs(Pos.x - LastPos.x) > 150.0f || fabs(Pos.y - LastPos.y) > 150.0f)
        {
            LastPos = Pos;
        }
        Tilc::TVector2D DeltaPos = Pos - LastPos;
        LastPos = Pos;
        if (Camera)
        {
            Camera->Rotate(DeltaPos.x, DeltaPos.y);
        }
        // If Styled window is created and theme is created
        if (Tilc::GameObject->m_Window->m_TopmostWindow && Tilc::GameObject->GetContext()->m_Theme)
        {
            // if this is topmost StyledWindow then make dragging whole app window
            int wx, wy;
            SDL_GetWindowPosition(Tilc::GameObject->m_Window->GetRenderWindow(), &wx, &wy);

            // then check if mouse is in the area of window buttons
            bool AllowRedraw = (Pos.y - wy >= 0.0f) && (Pos.y - wy <= 3 * Tilc::GameObject->GetContext()->m_Theme->wnd_caption_middle_rc.h);
            if (AllowRedraw && (Pos.x - wx > Tilc::GameObject->m_Window->m_TopmostWindow->m_Position.w - 3 * (Tilc::GameObject->GetContext()->m_Theme->wnd_close_button_rc.w + 10)))
            {
                // if so, then invalidate window to force redrawing only capytion buttons
                Tilc::GameObject->m_Window->m_TopmostWindow->Invalidate(Tilc::Gui::ENeedUpdate::ENU_Caption);
            }
        }
    }
    else if (Event->type == SDL_EVENT_WINDOW_FOCUS_GAINED)
    {
        Tilc::GameObject->m_Window->SetIsFocused(true);
    }
    else if (Event->type == SDL_EVENT_WINDOW_FOCUS_LOST)
    {
        Tilc::GameObject->m_Window->SetIsFocused(false);
    }
    else if (Event->type == SDL_EVENT_WINDOW_MAXIMIZED)
    {
        Tilc::GameObject->m_Window->Maximize();
    }
    else if (Event->type == SDL_EVENT_WINDOW_RESTORED)
    {
        Tilc::GameObject->m_Window->Restore();
    }

    // If Styled window is created then pass event to it
    if (Tilc::GameObject->m_Window->m_TopmostWindow && !Tilc::GameObject->m_Window->IsMinimized())
    {
        SDL_FPoint pt;
        SDL_GetMouseState(&pt.x, &pt.y);
        Tilc::Gui::TStyledWindow* wnd = Tilc::GameObject->m_Window->m_TopmostWindow;
        // Find which styled window is under the mouse cursor. If more then one, then take the topmost one.
        for (auto it = Tilc::GameObject->m_Window->m_AllWindows.rbegin(); it != Tilc::GameObject->m_Window->m_AllWindows.rend(); ++it)
        {
            SDL_FRect Position{ (*it)->GetRealPosition() };
            if (SDL_PointInRectFloat(&pt, &Position))
            {
                Tilc::Gui::TStyledWindow* FoundWnd = *it;
                // By default we take window under cursor into account
                bool WindowOK = true;
                if (FoundWnd->GetParent())
                {
                    // But if we are on the parent window vertical scrollbar, then mark not to take this window into account
                    if (WindowOK && FoundWnd->GetParent()->m_VScrollBar)
                    {
                        SDL_FRect Position{ FoundWnd->GetParent()->m_VScrollBar->GetRealPosition() };
                        if (SDL_PointInRectFloat(&pt, &Position))
                        {
                            WindowOK = false;
                        }
                    }
                    // And if we are on the parent window horizontal scrollbar, then mark not to take this window into account either.
                    if (WindowOK && FoundWnd->GetParent()->m_HScrollBar)
                    {
                        SDL_FRect Position{ FoundWnd->GetParent()->m_HScrollBar->GetRealPosition() };
                        if (SDL_PointInRectFloat(&pt, &Position))
                        {
                            WindowOK = false;
                        }
                    }
                }
                if (WindowOK)
                {
                    wnd = (*it);
                    break;
                }
            }
        }

        // Poniższy if odpowiada za usunięcie stanu hover z kontrolek okna po wyjechaniu z niego do innego okna
        if (Tilc::GameObject->m_Window->m_LastProcessedWindow != wnd)
        {
            if (Tilc::GameObject->m_Window->m_LastProcessedWindow)
            {
                if (Tilc::GameObject->m_Window->m_LastProcessedWindow->ResetControlsState(CONTROL_STATE_HOVER | CONTROL_STATE_PUSHED, false))
                {
                    Tilc::GameObject->m_Window->m_LastProcessedWindow->Invalidate();
                }
            }
            Tilc::GameObject->m_Window->m_LastProcessedWindow = wnd;
        }
        // Pass event to the found styled window
        //std::cout << "ProcessChildEvents for: " << wnd->GetName() << std::endl;
        wnd->ProcessChildEvent(*Event);
    }

	return SDL_APP_CONTINUE;
}

SDL_AppResult Tilc::TEventManager::HandleEvent(const SDL_Event* Event)
{
	// Handling events
	if (DefaultHandleEvent(Event) == SDL_APP_SUCCESS)
	{
		return SDL_APP_SUCCESS;
	}

	for (auto& b_itr : m_Bindings)
	{
		Tilc::TBinding* Bind = b_itr.second;
		for (auto& e_itr : Bind->m_Events)
		{
            if ((e_itr.first != Event->type) && (e_itr.first != EVENT_KEYBOARD && e_itr.first != EVENT_MOUSE && e_itr.first != EVENT_JOYSTICK))
            {
                continue;
            }

			if (Event->type == SDL_EVENT_KEY_DOWN || Event->type == SDL_EVENT_KEY_UP)
			{
                //std::cout << "Event->key.key: " << Event->key.key << ",  Type: " << Event->type << std::endl;
                if (e_itr.first == EVENT_KEYBOARD)
                {
                    Bind->m_Details.m_KeyCode = Event->key.key;
                    Bind->m_Details.m_KeyDown = (Event->type == SDL_EVENT_KEY_DOWN);
                    Bind->m_Details.m_KeyUp = (Event->type == SDL_EVENT_KEY_UP);
                    ++(Bind->m_Count);
                    //std::cout << "  Count: " << Bind->m_Count << std::endl;
                    break;
                }
				else if (e_itr.second.m_Code == Event->key.key)
				{
					// Matching event/keystroke.
					// Increase count.
					if (Bind->m_Details.m_KeyCode != -1)
					{
						Bind->m_Details.m_KeyCode = e_itr.second.m_Code;
                        Bind->m_Details.m_KeyDown = (Event->type == SDL_EVENT_KEY_DOWN);
                        Bind->m_Details.m_KeyUp = (Event->type == SDL_EVENT_KEY_UP);
                    }
					++(Bind->m_Count);
					break;
				}
			}
			else if (Event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || Event->type == SDL_EVENT_MOUSE_BUTTON_UP)
			{
				if (e_itr.second.m_Code == Event->button.button)
				{
					// Matching event/keystroke.
					// Increase count.
					Bind->m_Details.m_Mouse.x = static_cast<int>(Event->button.x);
					Bind->m_Details.m_Mouse.y = static_cast<int>(Event->button.y);
					Bind->m_Details.m_KeyCode = e_itr.second.m_Code;
					++(Bind->m_Count);
					break;
				}
			}
			else
			{
                if (e_itr.first == EVENT_MOUSE || e_itr.first == SDL_EVENT_MOUSE_MOTION || e_itr.first == SDL_EVENT_MOUSE_WHEEL)
                {
                    if (Event->type == SDL_EVENT_MOUSE_MOTION)
                    {
                        Bind->m_Details.m_Mouse.x = static_cast<int>(Event->button.x);
                        Bind->m_Details.m_Mouse.y = static_cast<int>(Event->button.y);
                        ++(Bind->m_Count);
                        break;
                    }
                    else if (Event->type == SDL_EVENT_MOUSE_WHEEL)
                    {
                        Bind->m_Details.m_MouseWheelDelta = Event->wheel.y;
                        ++(Bind->m_Count);
                        break;
                    }
                }
                else if (e_itr.first == EVENT_KEYBOARD)
                {
                    // Some stuff
                    break;
                }
				else if (Event->type == SDL_EVENT_WINDOW_RESIZED)
				{
					Bind->m_Details.m_Size.x = Event->window.data1;
					Bind->m_Details.m_Size.y = Event->window.data2;
                    ++(Bind->m_Count);
                    break;
                }
			}
		}
	}

    // Now we must call Update method because sometimes HandleEvent will be called for several SDL_Events in loop what causes Bind->m_Count variable be
    // incremented several times and that causes not to call EventHandler in game for some events
    Update();

	return SDL_APP_CONTINUE;
}

void Tilc::TEventManager::Update()
{
	if (!m_HasFocus)
	{
		return;
	}

	for (auto& b_itr : m_Bindings)
	{
		TBinding* Bind = b_itr.second;

        if (Bind->m_Events.size() == Bind->m_Count)
		{
			// These are current state callbacks called only for the state.
			auto StateCallbacks = m_Callbacks.find(m_CurrentState);
			// These are global callbacks that are called above the current state. They are called always regardless of current state.
			auto AllStatesCallbacks = m_Callbacks.find(Tilc::EStateType::All);

			if (StateCallbacks != m_Callbacks.end())
			{
				auto CallItr = StateCallbacks->second.find(Bind->m_Name);
				if (CallItr != StateCallbacks->second.end())
				{
					// Pass in information about events.
					CallItr->second(&Bind->m_Details);
				}
			}

			// These are global callbacks that are called above the current state. They are called always regardless of current state.
			if (AllStatesCallbacks != m_Callbacks.end())
			{
				auto CallItr = AllStatesCallbacks->second.find(Bind->m_Name);
				if (CallItr != AllStatesCallbacks->second.end())
				{
					// Pass in information about events.
					CallItr->second(&Bind->m_Details);
				}
			}
		}
		Bind->m_Count = 0;
		Bind->m_Details.Clear();
	}
}

void Tilc::TEventManager::LoadBindings()
{
	Tilc::TExtString Delimiter = ":";
	std::ifstream BindingsFile;
	BindingsFile.open("data/Keys.cfg");
	if (!BindingsFile.is_open())
	{
		std::cout << "File data/Keys.cfg not exists. Skipping..." << std::endl;
		return;
	}

	Tilc::TExtString Line;
	while (std::getline(BindingsFile, Line))
	{
		std::stringstream Keystream(Line);
		Tilc::TExtString CallbackName;
		Keystream >> CallbackName;
		TBinding* Bind = new TBinding(CallbackName);
		while (!Keystream.eof())
		{
			Tilc::TExtString KeyVal;
			Tilc::TExtString EventTypeName;
			Keystream >> KeyVal;
			size_t Start = 0;
			size_t End = KeyVal.find(Delimiter);
			if (End == Tilc::TExtString::npos)
			{
				delete Bind;
				Bind = nullptr;
				break;
			}

			EventTypeName = KeyVal.substr(Start, End - Start);
			EEventType EventType = 0;
			if (EventTypeName == "Closed")
			{
				EventType = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
			}
			else if (EventTypeName == "Resized")
			{
				EventType = SDL_EVENT_WINDOW_RESIZED;
			}
			else if (EventTypeName == "FocusLost")
			{
				EventType = SDL_EVENT_WINDOW_FOCUS_LOST;
			}
			else if (EventTypeName == "FocusGained")
			{
				EventType = SDL_EVENT_WINDOW_FOCUS_GAINED;
			}
			else if (EventTypeName == "KeyPressed")
			{
				EventType = SDL_EVENT_KEY_DOWN;
			}
			else if (EventTypeName == "KeyReleased")
			{
				EventType = SDL_EVENT_KEY_UP;
			}
			else if (EventTypeName == "MouseWheelScrolled")
			{
				EventType = SDL_EVENT_MOUSE_WHEEL;
			}
			else if (EventTypeName == "MouseButtonPressed")
			{
				EventType = SDL_EVENT_MOUSE_BUTTON_DOWN;
			}
			else if (EventTypeName == "MouseButtonReleased")
			{
				EventType = SDL_EVENT_MOUSE_BUTTON_UP;
			}
			else if (EventTypeName == "MouseMoved" || EventTypeName == "MouseMotion")
			{
				EventType = SDL_EVENT_MOUSE_MOTION;
			}
			else if (EventTypeName == "JoystickButtonPressed")
			{
				EventType = SDL_EVENT_JOYSTICK_BUTTON_DOWN;
			}
			else if (EventTypeName == "JoystickButtonReleased")
			{
				EventType = SDL_EVENT_JOYSTICK_BUTTON_UP;
			}
			else if (EventTypeName == "JoystickMoved")
			{
				EventType = SDL_EVENT_JOYSTICK_AXIS_MOTION;
			}
			else if (EventTypeName == "JoystickConnected")
			{
				EventType = SDL_EVENT_JOYSTICK_ADDED;
			}
			else if (EventTypeName == "JoystickDisconnected")
			{
				EventType = SDL_EVENT_JOYSTICK_REMOVED;
			}
			else if (EventTypeName == "TouchBegan")
			{
				EventType = SDL_EVENT_FINGER_DOWN;
			}
			else if (EventTypeName == "TouchMoved")
			{
				EventType = SDL_EVENT_FINGER_MOTION;
			}
			else if (EventTypeName == "TouchEnded")
			{
				EventType = SDL_EVENT_FINGER_UP;
			}
			else if (EventTypeName == "SensorChanged")
			{
				EventType = SDL_EVENT_SENSOR_UPDATE;
			}
			else if (EventTypeName == "Keyboard")
			{
				EventType = EVENT_KEYBOARD;
			}
			else if (EventTypeName == "Mouse")
			{
				EventType = EVENT_MOUSE;
			}
			else if (EventTypeName == "Joystick")
			{
				EventType = EVENT_JOYSTICK;
			}
			Tilc::TExtString Code = KeyVal.substr(End + Delimiter.length());
			TEventInfo EventInfo;
			EventInfo.m_Code = 0;

			// mouse buttons
			if (EventType == EVENT_KEYBOARD || EventType == SDL_EVENT_KEY_DOWN || EventType == SDL_EVENT_KEY_UP)
			{
				if (Code == "Key_A")
				{
					EventInfo.m_Code = SDLK_A;
				}
				else if (Code == "Key_B")
				{
					EventInfo.m_Code = SDLK_B;
				}
				else if (Code == "Key_C")
				{
					EventInfo.m_Code = SDLK_C;
				}
				else if (Code == "Key_D")
				{
					EventInfo.m_Code = SDLK_D;
				}
				else if (Code == "Key_E")
				{
					EventInfo.m_Code = SDLK_E;
				}
				else if (Code == "Key_F")
				{
					EventInfo.m_Code = SDLK_F;
				}
				else if (Code == "Key_G")
				{
					EventInfo.m_Code = SDLK_G;
				}
				else if (Code == "Key_H")
				{
					EventInfo.m_Code = SDLK_H;
				}
				else if (Code == "Key_I")
				{
					EventInfo.m_Code = SDLK_I;
				}
				else if (Code == "Key_J")
				{
					EventInfo.m_Code = SDLK_J;
				}
				else if (Code == "Key_K")
				{
					EventInfo.m_Code = SDLK_K;
				}
				else if (Code == "Key_L")
				{
					EventInfo.m_Code = SDLK_L;
				}
				else if (Code == "Key_M")
				{
					EventInfo.m_Code = SDLK_M;
				}
				else if (Code == "Key_N")
				{
					EventInfo.m_Code = SDLK_N;
				}
				else if (Code == "Key_O")
				{
					EventInfo.m_Code = SDLK_O;
				}
				else if (Code == "Key_P")
				{
					EventInfo.m_Code = SDLK_P;
				}
				else if (Code == "Key_Q")
				{
					EventInfo.m_Code = SDLK_Q;
				}
				else if (Code == "Key_R")
				{
					EventInfo.m_Code = SDLK_R;
				}
				else if (Code == "Key_S")
				{
					EventInfo.m_Code = SDLK_S;
				}
				else if (Code == "Key_T")
				{
					EventInfo.m_Code = SDLK_T;
				}
				else if (Code == "Key_U")
				{
					EventInfo.m_Code = SDLK_U;
				}
				else if (Code == "Key_V")
				{
					EventInfo.m_Code = SDLK_V;
				}
				else if (Code == "Key_W")
				{
					EventInfo.m_Code = SDLK_W;
				}
				else if (Code == "Key_X")
				{
					EventInfo.m_Code = SDLK_X;
				}
				else if (Code == "Key_Y")
				{
					EventInfo.m_Code = SDLK_Y;
				}
				else if (Code == "Key_Z")
				{
					EventInfo.m_Code = SDLK_Z;
				}
				else if (Code == "Key_Num0")
				{
					EventInfo.m_Code = SDLK_0;
				}
				else if (Code == "Key_Num1")
				{
					EventInfo.m_Code = SDLK_1;
				}
				else if (Code == "Key_Num2")
				{
					EventInfo.m_Code = SDLK_2;
				}
				else if (Code == "Key_Num3")
				{
					EventInfo.m_Code = SDLK_3;
				}
				else if (Code == "Key_Num4")
				{
					EventInfo.m_Code = SDLK_4;
				}
				else if (Code == "Key_Num5")
				{
					EventInfo.m_Code = SDLK_5;
				}
				else if (Code == "Key_Num6")
				{
					EventInfo.m_Code = SDLK_6;
				}
				else if (Code == "Key_Num7")
				{
					EventInfo.m_Code = SDLK_7;
				}
				else if (Code == "Key_Num8")
				{
					EventInfo.m_Code = SDLK_8;
				}
				else if (Code == "Key_Num9")
				{
					EventInfo.m_Code = SDLK_9;
				}
				else if (Code == "Key_Escape")
				{
					EventInfo.m_Code = SDLK_ESCAPE;
				}
				else if (Code == "Key_LControl")
				{
					EventInfo.m_Code = SDL_KMOD_LCTRL;
				}
				else if (Code == "Key_LShift")
				{
					EventInfo.m_Code = SDL_KMOD_LSHIFT;
				}
				else if (Code == "Key_LAlt")
				{
					EventInfo.m_Code = SDL_KMOD_LALT;
				}
				else if (Code == "Key_LGui")
				{
					EventInfo.m_Code = SDL_KMOD_LGUI;
				}
				else if (Code == "Key_RControl")
				{
					EventInfo.m_Code = SDLK_RCTRL;
				}
				else if (Code == "Key_RShift")
				{
					EventInfo.m_Code = SDLK_RSHIFT;
				}
				else if (Code == "Key_RAlt")
				{
					EventInfo.m_Code = SDLK_RALT;
				}
				else if (Code == "Key_RGui")
				{
					EventInfo.m_Code = SDLK_RGUI;
				}
				else if (Code == "Key_Menu")
				{
					EventInfo.m_Code = SDLK_MENU;
				}
				else if (Code == "Key_LBracket") // [
				{
					EventInfo.m_Code = SDLK_LEFTBRACKET;
				}
				else if (Code == "Key_RBracket") // ]
				{
					EventInfo.m_Code = SDLK_RIGHTBRACKET;
				}
				else if (Code == "Key_Semicolon") // ;
				{
					EventInfo.m_Code = SDLK_SEMICOLON;
				}
				else if (Code == "Key_Comma") // ,
				{
					EventInfo.m_Code = SDLK_COMMA;
				}
				else if (Code == "Key_Period") // .
				{
					EventInfo.m_Code = SDLK_PERIOD;
				}
				else if (Code == "Key_Apostrophe") // '
				{
					EventInfo.m_Code = SDLK_APOSTROPHE;
				}
				else if (Code == "Key_Slash") // /
				{
					EventInfo.m_Code = SDLK_SLASH;
				}
				else if (Code == "Key_Backslash")
				{
					EventInfo.m_Code = SDLK_BACKSLASH;
				}
				else if (Code == "Key_Grave") // `
				{
					EventInfo.m_Code = SDLK_GRAVE;
				}
				else if (Code == "Key_Equals") // =
				{
					EventInfo.m_Code = SDLK_EQUALS;
				}
				else if (Code == "Key_Underscore") // _
				{
					EventInfo.m_Code = SDLK_UNDERSCORE;
				}
				else if (Code == "Key_Space") // ' '
				{
					EventInfo.m_Code = SDLK_SPACE;
				}
				else if (Code == "Key_Enter" || Code == "Key_Return")
				{
					EventInfo.m_Code = SDLK_RETURN;
				}
				else if (Code == "Key_Backspace")
				{
					EventInfo.m_Code = SDLK_BACKSPACE;
				}
				else if (Code == "Key_Tab")
				{
					EventInfo.m_Code = SDLK_TAB;
				}
				else if (Code == "Key_PageUp")
				{
					EventInfo.m_Code = SDLK_PAGEUP;
				}
				else if (Code == "Key_PageDown")
				{
					EventInfo.m_Code = SDLK_PAGEDOWN;
				}
				else if (Code == "Key_End")
				{
					EventInfo.m_Code = SDLK_END;
				}
				else if (Code == "Key_Home")
				{
					EventInfo.m_Code = SDLK_HOME;
				}
				else if (Code == "Key_Insert")
				{
					EventInfo.m_Code = SDLK_INSERT;
				}
				else if (Code == "Key_Delete")
				{
					EventInfo.m_Code = SDLK_DELETE;
				}
				else if (Code == "Key_Plus") // +
				{
					EventInfo.m_Code = SDLK_PLUS;
				}
				else if (Code == "Key_Minus") // -
				{
					EventInfo.m_Code = SDLK_MINUS;
				}
				else if (Code == "Key_Asterisk")
				{
					EventInfo.m_Code = SDLK_ASTERISK;
				}
				else if (Code == "Key_Left")
				{
					EventInfo.m_Code = SDLK_LEFT;
				}
				else if (Code == "Key_Right")
				{
					EventInfo.m_Code = SDLK_RIGHT;
				}
				else if (Code == "Key_Up")
				{
					EventInfo.m_Code = SDLK_UP;
				}
				else if (Code == "Key_Down")
				{
					EventInfo.m_Code = SDLK_DOWN;
				}
				else if (Code == "Key_F1")
				{
					EventInfo.m_Code = SDLK_F1;
				}
				else if (Code == "Key_F2")
				{
					EventInfo.m_Code = SDLK_F2;
				}
				else if (Code == "Key_F3")
				{
					EventInfo.m_Code = SDLK_F3;
				}
				else if (Code == "Key_F4")
				{
					EventInfo.m_Code = SDLK_F4;
				}
				else if (Code == "Key_F5")
				{
					EventInfo.m_Code = SDLK_F5;
				}
				else if (Code == "Key_F6")
				{
					EventInfo.m_Code = SDLK_F6;
				}
				else if (Code == "Key_F7")
				{
					EventInfo.m_Code = SDLK_F7;
				}
				else if (Code == "Key_F8")
				{
					EventInfo.m_Code = SDLK_F8;
				}
				else if (Code == "Key_F9")
				{
					EventInfo.m_Code = SDLK_F9;
				}
				else if (Code == "Key_F10")
				{
					EventInfo.m_Code = SDLK_F10;
				}
				else if (Code == "Key_F11")
				{
					EventInfo.m_Code = SDLK_F11;
				}
				else if (Code == "Key_F12")
				{
					EventInfo.m_Code = SDLK_F12;
				}
				else if (Code == "Key_F13")
				{
					EventInfo.m_Code = SDLK_F13;
				}
				else if (Code == "Key_F14")
				{
					EventInfo.m_Code = SDLK_F14;
				}
				else if (Code == "Key_F15")
				{
					EventInfo.m_Code = SDLK_F15;
				}
				else if (Code == "Key_Pause")
				{
					EventInfo.m_Code = SDLK_PAUSE;
				}
			}
			else if (EventType == EVENT_MOUSE || EventType == SDL_EVENT_MOUSE_BUTTON_DOWN || SDL_EVENT_MOUSE_BUTTON_UP || SDL_EVENT_MOUSE_MOTION)
			{
				if (Code == "Left")
				{
					EventInfo.m_Code = SDL_BUTTON_LEFT;
				}
				else if (Code == "Right")
				{
					EventInfo.m_Code = SDL_BUTTON_RIGHT;
				}
				else if (Code == "Middle")
				{
					EventInfo.m_Code = SDL_BUTTON_MIDDLE;
				}
				else if (Code == "Extra1")
				{
					EventInfo.m_Code = SDL_BUTTON_X1;
				}
				else if (Code == "Extra2")
				{
					EventInfo.m_Code = SDL_BUTTON_X2;
				}
                else
                {
                    EventInfo.m_Code = std::atoi(Code.c_str());
                }
			}
			else
			{
				EventInfo.m_Code = std::stoi(Code);
			}
			Bind->BindEvent(EventType, EventInfo);
		}
		if (!AddBinding(Bind))
		{
			delete Bind;
		}
		Bind = nullptr;
	}
	BindingsFile.close();
}
