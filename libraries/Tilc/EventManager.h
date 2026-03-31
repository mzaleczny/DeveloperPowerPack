#pragma once
#include <SDL3/SDL.h>
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include <vector>
#include <unordered_map>
#include <functional>
#include "Tilc/Utils/Vector.h"

constexpr const Uint32 EVENT_KEYBOARD	= SDL_EVENT_USER + 1;
constexpr const Uint32 EVENT_MOUSE		= SDL_EVENT_USER + 2;
constexpr const Uint32 EVENT_JOYSTICK	= SDL_EVENT_USER + 3;


namespace Tilc
{
	using EEventType = Uint32;

	struct DECLSPEC TEventInfo
	{
		TEventInfo()
		{
			m_Code = 0;
		}
		TEventInfo(int EventCode)
		{
			m_Code = EventCode;
		}
		union
		{
			int m_Code;
		};
	};

	using TEvents = std::vector<std::pair<EEventType, TEventInfo>>;

	struct DECLSPEC TEventDetails
	{
		Tilc::TExtString m_Name;
		Tilc::TVector2D<int> m_Size;
		uint32_t m_TextEntered;
		Tilc::TVector2D<int> m_Mouse;
		float m_MouseWheelDelta;
		int m_KeyCode;
        bool m_KeyDown;
        bool m_KeyUp;

		TEventDetails(const Tilc::TExtString& BindName) : m_Name(BindName)
		{
			Clear();
		}

		void Clear()
		{
			m_Size = Tilc::TVector2D<int>(0, 0);
			m_TextEntered = 0;
			m_Mouse = Tilc::TVector2D<int>(0, 0);
			m_MouseWheelDelta = 0;
			m_KeyCode = -1;
		}
	};

	struct DECLSPEC TBinding
	{
		TEvents m_Events;
		Tilc::TExtString m_Name;
		int m_Count; // Count of events that are "happening".
		TEventDetails m_Details;

		TBinding(const Tilc::TExtString& Name) : m_Name(Name), m_Details(Name), m_Count(0)
		{
		}

		void BindEvent(EEventType EventType, TEventInfo Info = TEventInfo())
		{
			m_Events.emplace_back(EventType, Info);
		}
	};

	using TBindings = std::unordered_map<Tilc::TExtString, TBinding*>; // BindingName => TBinding*
	using TCallbackContainer = std::unordered_map<Tilc::TExtString, std::function<void(TEventDetails*)>>; // BindingName => CallbackFunction(TEventDetails*)
	enum class EStateType;
	using TCallbacks = std::unordered_map<EStateType, TCallbackContainer>; // StateType => CallbackContainer



	class DECLSPEC TEventManager
	{
	public:
		TEventManager() : m_HasFocus(true)
		{
			LoadBindings();
		}
		~TEventManager();
		bool AddBinding(TBinding* Binding);
		bool RemoveBinding(Tilc::TExtString Name);
		void SetFocus(bool Focus)
		{
			m_HasFocus = Focus;
		}
		EStateType GetCurrentState() const
		{
			return m_CurrentState;
		}
		void SetCurrentState(EStateType State)
		{
			m_CurrentState = State;
		}

		// Needs to be defined in the header!
		template<typename T>
		bool AddCallback(EStateType StateType, const Tilc::TExtString& Name, void(T::* Func)(TEventDetails*), T* Instance)
		{
			auto itr = m_Callbacks.emplace(StateType, TCallbackContainer()).first;
			auto Temp = std::bind(Func, Instance, std::placeholders::_1);
			return itr->second.emplace(Name, Temp).second;
		}
		bool RemoveCallback(EStateType StateType, const Tilc::TExtString& Name)
		{
			auto itr = m_Callbacks.find(StateType);
			if (itr == m_Callbacks.end())
			{
				return false;
			}
			auto itr2 = itr->second.find(Name);
			if (itr2 == itr->second.end())
			{
				return false;
			}
			itr->second.erase(Name);
			return true;
		}
		// Default event handler. It is called regardless of event configuration file defines handled events. For example it drives default current camera
		// movement in 3d games.
		SDL_AppResult DefaultHandleEvent(const SDL_Event* Event);
		// Checks if Event is in m_Bindings map and if true, then increases events count for the binding
		SDL_AppResult HandleEvent(const SDL_Event* Event);

        // Call EventHandlers switched in game level or state to handle all receiving SDL_Events and not skipping any of them
		void Update();

		SDL_MouseButtonFlags GetMousePos(float* x, float* y, bool Global = true)
		{
			if (Global)
			{
				return SDL_GetGlobalMouseState(x, y);
			}
			return SDL_GetMouseState(x, y);
		}
	private:
		void LoadBindings();
		TBindings m_Bindings; // BindingName => TBinding*
		TCallbacks m_Callbacks; // StateType => CallbackContainer
		EStateType m_CurrentState;
		bool m_HasFocus;
	};

}
