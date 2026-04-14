#pragma once
#include <vector>
#include <unordered_map>
#include <functional>
#include "Tilc/DllGlobals.h"
#include "SDL3/SDL.h"

namespace Tilc
{
	class TStateManager;
    class TEventDetails;
    class TEventManager;
    class TWindow;
    class TLevel;

	enum class EStateType
	{
		All = 0, Intro = 1, MainMenu, Game, Paused, GameOver, Credits
	};


	class DECLSPEC TBaseState
	{
		friend class TStateManager;
	public:
        TBaseState(TStateManager* StateManager, bool DraggingTurnedOn = false);
		virtual ~TBaseState()
        {
            if (HasDraggingTurnedOn)
            {
                RemoveWindowDraggingHandler();
            }
        }
		virtual void OnCreate() = 0;
		virtual void OnDestroy() = 0;
		virtual void Activate() = 0;
		virtual void Deactivate() = 0;
		virtual void Update(float DeltaTime) = 0; // DeltaTime czas w sekundach
		virtual void Draw() = 0;
		void SetTransparent(const bool Transparent)
		{
			m_Transparent = Transparent;
		}
		bool IsTransparent() const
		{
			return m_Transparent;
		}
		void SetTranscendent(const bool Transcendence)
		{
			m_Transcendent = Transcendence;
		}
		bool IsTranscendent() const
		{
			return m_Transcendent;
		}
		TStateManager* GetStateManager()
		{
			return m_StateManager;
		}

        TStateManager* m_StateManager;
        TWindow* m_Window;
        TEventManager* m_EventManager;
        TLevel* m_Level = nullptr;
        bool m_Transparent;
		bool m_Transcendent;
        float m_Elapsed;

        // Cross platform handling dragging window by custom caption borderless window
        virtual void MouseDownLeft(TEventDetails* Details);
        virtual void MouseUpLeft(TEventDetails* Details);
        virtual void MouseMotion(TEventDetails* Details);

        float DragStartMouseX, DragStartMouseY;
        int DragStartWindowX, DragStartWindowY;
        bool DraggingWindow{ false };
        bool HasDraggingTurnedOn{ false };
        void SetWindowDraggingHandler();
        void RemoveWindowDraggingHandler();
    };


	using TStateContainer = std::vector<std::pair<EStateType, TBaseState*>>;
	using TStateTypeContainer = std::vector<EStateType>;
	using TStateFactory = std::unordered_map<EStateType, std::function<TBaseState* (void)>>;


	class DECLSPEC TStateManager
	{
	public:
		TStateManager();
		~TStateManager();
		void Update(float DeltaTime); // DeltaTime czas w sekundach
		void Draw();
		void ProcessRequests();
		bool HasState(const EStateType& StateType);
		void SwitchTo(const EStateType& StateType);
		void RemoveAllImmediately();
		void Remove(const EStateType& StateType)
		{
			m_ToRemove.push_back(StateType);
		}
		template<typename T>
		void RegisterState(const EStateType& StateType)
		{
			m_StateFactory[StateType] = [this]() -> TBaseState*
				{
					return new T(this);
				};
		}
		TBaseState* GetState(const EStateType& StateType) const;
	private:
		// Methods.
		void CreateState(const EStateType& StateType);
		void RemoveState(const EStateType& StateType);
		// Members.
		TStateContainer m_States;
		TStateTypeContainer m_ToRemove;
		TStateFactory m_StateFactory;
	};

}
