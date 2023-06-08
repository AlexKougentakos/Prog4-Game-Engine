#pragma once
#include "glm/glm.hpp"
namespace ody
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
		MoveCommand(ody::GameObject* pActor, float moveSpeed, const glm::vec2* moveDir) : m_pActor{ pActor }, m_MoveSpeed{moveSpeed}
			, m_MoveDirectionRef{ moveDir }, m_UseRef(true) {}
		MoveCommand(ody::GameObject* pActor, float moveSpeed, const glm::vec2& moveDir) : m_pActor{ pActor }, m_MoveSpeed{ moveSpeed }
			, m_MoveDirection{ moveDir } {}

		virtual ~MoveCommand() override = default;
		MoveCommand(const MoveCommand& other) = delete;
		MoveCommand(MoveCommand&& other) = delete;
		MoveCommand& operator=(const MoveCommand& other) = delete;
		MoveCommand& operator=(MoveCommand&& other) = delete;
		
		virtual void Execute() override;
	private:
		ody::GameObject* m_pActor{};
		const float m_MoveSpeed{};
		const glm::vec2* m_MoveDirectionRef{};
		const glm::vec2 m_MoveDirection{};

		bool m_UseRef{ false };
	};
}
