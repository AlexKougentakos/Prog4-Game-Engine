#include "ServiceLocator.h"

namespace ody
{
    //Initialize the null instances
    IAudio* ServiceLocator::m_pAudioSystemInstance{ &ServiceLocator::m_pNullSoundSystemInstance };
    NullSoundSystem ServiceLocator::m_pNullSoundSystemInstance;

    void ServiceLocator::Provide(IAudio* audio)
    {
        if (audio == nullptr) return;

        m_pAudioSystemInstance = audio;
    }

    IAudio& ServiceLocator::GetSoundSystem()
    {
        return *m_pAudioSystemInstance;
    }
}
