#pragma once
#include <string>

namespace ody
{
	class State
{
public:
    State() = default;
    virtual ~State() = default;

    State(const State& other) = delete;
    State(State&& other) = delete;
    State& operator=(const State& other) = delete;
    State& operator=(State&& other) = delete;

    virtual void Enter() = 0;
    virtual void Update() = 0;
    virtual void Exit() = 0;

    virtual std::string GetStateName() = 0;
};

}
