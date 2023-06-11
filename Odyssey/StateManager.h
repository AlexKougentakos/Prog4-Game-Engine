#pragma once
#include <algorithm>
#include <memory>
#include <stack>
#include "State.h"

namespace ody
{
	class State;

	class StateManager final
	{
	public:
		void ChangeState(std::unique_ptr<State> newState);

		void PushState(std::unique_ptr<State> newState);

		void PopState();

		void Update();
	private:
		std::stack<std::unique_ptr<State>> m_States{};

	};
}
