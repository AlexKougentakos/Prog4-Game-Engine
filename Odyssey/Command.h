#pragma once
#include "glm/glm.hpp"
namespace dae
{
	class GameObject;
}
namespace ody
{
	class Command
	{
	public:
		Command() = default;
		virtual ~Command() = default;
		virtual void Execute() = 0;
		Command(const Command& other) = delete;
		Command(Command&& other) = delete;
		Command& operator=(const Command& other) = delete;
		Command& operator=(Command&& other) = delete;
	};

	class MoveCommand final : public Command
	{
	public:
		MoveCommand(dae::GameObject* pActor, float moveSpeed, const glm::vec2& moveDir, float elapsedTime) : m_pActor{ pActor }, m_MoveSpeed{moveSpeed}
			, m_MoveDirection{ moveDir }, m_ElapsedTime{elapsedTime} {}
		virtual ~MoveCommand() override = default;
		
		virtual void Execute() override;
	private:
		dae::GameObject* m_pActor{};
		const float m_MoveSpeed{};
		glm::vec2 m_MoveDirection{};
		const float m_ElapsedTime{ };
	
	};
}
