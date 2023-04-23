#pragma once
#include "Observer.h"
#include "Subject.h"
#include "Structs.h"
#include <memory>

class AchievementTest : public ody::IObserver
{
public:
	explicit AchievementTest(std::shared_ptr<ody::Subject> subject)
		: m_pSubject{ subject }
	{
		m_pSubject->AddObserver(this);
	}

	~AchievementTest()
	{
		m_pSubject->RemoveObserver(this);
	}

	void OnNotify(ody::GameEvent event) override
	{
		if (event == ody::GameEvent::TEST_EVENT)
		{
			int i{};
			++i;
		}
	}

private:
	std::shared_ptr<ody::Subject> m_pSubject{};
};

