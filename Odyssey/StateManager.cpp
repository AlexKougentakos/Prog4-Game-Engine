#include "StateManager.h"
#include "State.h"

namespace ody
{
    void StateManager::ChangeState(std::unique_ptr<State> newState)
    {
        if (!m_States.empty())
        {
            // Check if the new state is the same type as the current state
            if (m_States.top()->GetStateName() == newState->GetStateName()) {
                return;  // If it is, do nothing
            }
            m_States.top()->Exit();
            m_States.pop();
        }
        m_States.push(std::move(newState));
        m_States.top()->Enter();
    }

    void StateManager::PushState(std::unique_ptr<State> newState)
    {
        if (!m_States.empty())
            m_States.top()->Exit();

        m_States.push(std::move(newState));
        m_States.top()->Enter();
    }

    void StateManager::PopState()
    {
        if (!m_States.empty()) 
        {
            m_States.top()->Exit();
            m_States.pop();
        }
        if (!m_States.empty())
            m_States.top()->Enter();
    }

	void StateManager::Update()
    {
        if (!m_States.empty())
            m_States.top()->Update();
    }
}
