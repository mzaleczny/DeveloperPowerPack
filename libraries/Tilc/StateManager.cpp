#include "Tilc/StateManager.h"
#include "Tilc/EventManager.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/Theme.h"

Tilc::TBaseState::TBaseState(TStateManager* StateManager, bool DraggingTurnedOn)
    : m_StateManager(StateManager), m_Transparent(false), m_Transcendent(false), m_Elapsed(0.0f), HasDraggingTurnedOn(DraggingTurnedOn)
{
    if (Tilc::GameObject)
    {
        m_Window = Tilc::GameObject->GetContext()->m_Window;
        m_EventManager = Tilc::GameObject->GetContext()->m_EventManager;
        if (HasDraggingTurnedOn)
        {
            SetWindowDraggingHandler();
        }
    }
}

Tilc::TStateManager::TStateManager()
{
}

Tilc::TStateManager::~TStateManager()
{
	for (auto& itr : m_States)
	{
		itr.second->OnDestroy();
		delete itr.second;
	}
}

void Tilc::TStateManager::Update(float DeltaTime)
{
	if (m_States.empty())
	{
		return;
	}

	if (m_States.back().second->IsTranscendent() && m_States.size() > 1)
	{
		auto itr = m_States.end();
		while (itr != m_States.begin())
		{
			if (itr != m_States.end())
			{
				if (!itr->second->IsTranscendent())
                {
					break;
                }
			}
			--itr;
		}
		for (; itr != m_States.end(); ++itr)
		{
			itr->second->Update(DeltaTime);
		}
	}
	else
	{
		m_States.back().second->Update(DeltaTime);
	}
}

void Tilc::TStateManager::Draw()
{
	if (m_States.empty())
	{
		return;
	}

	if (m_States.back().second->IsTransparent() && m_States.size() > 1)
	{
		auto itr = m_States.end();
		while (itr != m_States.begin())
		{
			if (itr != m_States.end())
			{
				if (!itr->second->IsTransparent())
				{
					break;
				}
			}
			--itr;
		}
		for (; itr != m_States.end(); ++itr)
		{
			itr->second->Draw();
		}
	}
	else
	{
		m_States.back().second->Draw();
	}
}

void Tilc::TStateManager::ProcessRequests()
{
	while (m_ToRemove.begin() != m_ToRemove.end())
	{
		RemoveState(*m_ToRemove.begin());
		m_ToRemove.erase(m_ToRemove.begin());
	}
}

bool Tilc::TStateManager::HasState(const EStateType& StateType)
{
	for (auto itr = m_States.begin(); itr != m_States.end(); ++itr)
	{
		if (itr->first == StateType) {
			auto Removed = std::find(m_ToRemove.begin(), m_ToRemove.end(), StateType);
			if (Removed == m_ToRemove.end())
			{
				return true;
			}
			return false;
		}
	}
	return false;
}

void Tilc::TStateManager::SwitchTo(const EStateType& StateType)
{
	TSharedContext* SharedContext = nullptr;
	if (Tilc::GameObject)
	{
		SharedContext = Tilc::GameObject->GetContext();
	}
	if (SharedContext && SharedContext->m_EventManager)
	{
		SharedContext->m_EventManager->SetCurrentState(StateType);
		for (auto itr = m_States.begin(); itr != m_States.end(); ++itr)
		{
			if (itr->first == StateType)
			{
				m_States.back().second->Deactivate();
				EStateType TmpType = itr->first;
				TBaseState* TmpState = itr->second;
				m_States.erase(itr);
				m_States.emplace_back(TmpType, TmpState);
				TmpState->Activate();
				return;
			}
		}

		// State with StateType wasn't found.
		if (!m_States.empty())
		{
			m_States.back().second->Deactivate();
		}
		CreateState(StateType);
		m_States.back().second->Activate();
	}
}

void Tilc::TStateManager::RemoveAllImmediately()
{
	auto itr = m_States.begin();
	while (itr != m_States.end())
	{
		// Function below calls OnDestroy of the state and frees its memory and removes the state from the list
		RemoveState(itr->first);
		// so we have to get pointer to the first state after cleanup above
		itr = m_States.begin();
	}
}

Tilc::TBaseState* Tilc::TStateManager::GetState(const EStateType& StateType) const
{
	for (auto itr = m_States.begin(); itr != m_States.end(); ++itr)
	{
		if (itr->first == StateType) {
			auto Removed = std::find(m_ToRemove.begin(), m_ToRemove.end(), StateType);
			if (Removed == m_ToRemove.end())
			{
				return itr->second;
			}
			return nullptr;
		}
	}
	return nullptr;
}

void Tilc::TBaseState::MouseDownLeft(Tilc::TEventDetails* Details)
{
    // Check if main window is dragged
    if (!DraggingWindow && Details && Details->m_Mouse.Coord[1] < Tilc::GameObject->GetContext()->m_Theme->wnd_caption_middle_rc.h)
    {
        DraggingWindow = true;
        Tilc::GameObject->GetContext()->m_Window->m_DraggedWindow = Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow;
        SDL_GetGlobalMouseState(&DragStartMouseX, &DragStartMouseY);
        SDL_GetWindowPosition(Tilc::GameObject->GetContext()->m_Window->GetRenderWindow(), &DragStartWindowX, &DragStartWindowY);
        //std::cout << "Dragging topwindow started" << std::endl;
    }
}

void Tilc::TBaseState::MouseUpLeft(Tilc::TEventDetails* Details)
{
    DraggingWindow = false;
}

void Tilc::TBaseState::MouseMotion(Tilc::TEventDetails* Details)
{
    if (DraggingWindow)
    {
        float mx, my;
        int dx, dy;
        SDL_GetGlobalMouseState(&mx, &my);
        dx = mx - DragStartMouseX;
        dy = my - DragStartMouseY;
        Tilc::Gui::TStyledWindow* DraggedWindow = Tilc::GameObject->GetContext()->m_Window->m_DraggedWindow;
        if (DraggedWindow)
        {
            if (DraggedWindow == Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow)
            {
                SDL_SetWindowPosition(Tilc::GameObject->GetContext()->m_Window->GetRenderWindow(), DragStartWindowX + dx, DragStartWindowY + dy);
            }
            else
            {
                DraggedWindow->SetPosition(DragStartWindowX + dx, DragStartWindowY + dy);
                // We have to update DraggedWindow->m_OriginalPosition also
                DraggedWindow->m_OriginalPosition = DraggedWindow->m_Position;
                DraggedWindow->m_OriginalPosition.x += DraggedWindow->GetParent()->GetOffsetX();
                DraggedWindow->m_OriginalPosition.y += DraggedWindow->GetParent()->GetOffsetY();

                /*
                std::cout << "mx: " << mx << "  my: " << my << "  DragStartMouseX: " << DragStartMouseX << "  DragStartMouseY: " << DragStartMouseY << "  dx: " << dx << "  dy: " << dy
                    << "  x: " << DraggedWindow->m_OriginalPosition.x
                    << "  y: " << DraggedWindow->m_OriginalPosition.y
                    << std::endl;
                */
                DraggedWindow->Invalidate();
            }
        }
    }
}

void Tilc::TBaseState::SetWindowDraggingHandler()
{
    Tilc::TEventManager* evMgr = Tilc::GameObject->GetContext()->m_EventManager;
    if (evMgr)
    {
        evMgr->AddCallback(Tilc::EStateType::Game, "MouseDown_Left", &TBaseState::MouseDownLeft, this);
        evMgr->AddCallback(Tilc::EStateType::Game, "MouseUp_Left", &TBaseState::MouseUpLeft, this);
        evMgr->AddCallback(Tilc::EStateType::Game, "MouseMotion", &TBaseState::MouseMotion, this);
    }
}

void Tilc::TBaseState::RemoveWindowDraggingHandler()
{
    Tilc::TEventManager* evMgr = Tilc::GameObject->GetContext()->m_EventManager;
    if (evMgr)
    {
        evMgr->RemoveCallback(Tilc::EStateType::Game, "MouseMotion");
        evMgr->RemoveCallback(Tilc::EStateType::Game, "MouseUp_Left");
        evMgr->RemoveCallback(Tilc::EStateType::Game, "MouseDown_Left");
    }
}

void Tilc::TStateManager::CreateState(const EStateType& StateType)
{
	auto NewState = m_StateFactory.find(StateType);
	if (NewState == m_StateFactory.end())
	{
		return;
	}
	TBaseState* State = NewState->second();
	m_States.emplace_back(StateType, State);
	State->OnCreate();
}

void Tilc::TStateManager::RemoveState(const EStateType& StateType)
{
	for (auto itr = m_States.begin(); itr != m_States.end(); ++itr)
	{
		if (itr->first == StateType)
		{
			itr->second->OnDestroy();
			delete itr->second;
			m_States.erase(itr);
			return;
		}
	}
}
