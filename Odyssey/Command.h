#pragma once
namespace ody
{
	class Command
	{
	public:
		virtual ~Command() = default;
		virtual void Execute() = 0;
		Command(const Command& other) = delete;
		Command(Command&& other) = delete;
		Command& operator=(const Command& other) = delete;
		Command& operator=(Command&& other) = delete;
	};

	class MoveCommand : public Command
	{
	public:
		void Execute() override;
	
	};
}
