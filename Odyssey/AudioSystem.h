#pragma once
#include <map>

#include "IAudio.h"
#include <memory>

namespace ody
{
	class AudioSystem : public IAudio
	{
	private:
		class AudioSystemImpl;
		AudioSystemImpl* pImpl{};

		std::map<unsigned int, std::pair<std::string, bool>> effectLocationMap{};
	public:
		virtual void PlaySound(unsigned int effectID) override;
		virtual void PauseSound() override;
		virtual void StopSound() override;
		virtual void StopAllSounds() override;

		explicit AudioSystem(const std::map<unsigned int, std::pair<std::string, bool>>& effectLocationMap);
		virtual ~AudioSystem() override;
	};
}