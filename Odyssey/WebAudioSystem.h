#pragma once
#ifdef __EMSCRIPTEN__
#include <map>

#include "IAudio.h"
#include <memory>

namespace ody
{
	class WebAudioSystem : public IAudio
	{
	private:
		class WebAudioSystemImpl;
		WebAudioSystemImpl* pImpl{};

		std::map<unsigned int, std::pair<std::string, bool>> effectLocationMap{};
	public:
		virtual void PlaySound(unsigned int effectID) override;
		virtual void PauseSound() override;
		virtual void StopSound() override;
		virtual void StopAllSounds() override;

		explicit WebAudioSystem(const std::map<unsigned int, std::pair<std::string, bool>>& effectLocationMap);
		virtual ~WebAudioSystem() override;

	};
}
#endif