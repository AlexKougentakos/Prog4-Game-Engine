#pragma once
#include "IAudio.h"

namespace ody
{
    class ServiceLocator final
    {
    public:
        static void Provide(IAudio* audio)
        {
            if (audio == nullptr) return;

            m_pAudioSystemInstance = audio;
        }

        static IAudio& GetSoundSystem()
        {
            return *m_pAudioSystemInstance;
        }

    private:
        static IAudio* m_pAudioSystemInstance;
        static NullSoundSystem m_pNullSoundSystemInstance;
    };

    //Initialize the null instances
    IAudio* ServiceLocator::m_pAudioSystemInstance{ &ServiceLocator::m_pNullSoundSystemInstance };
    NullSoundSystem ServiceLocator::m_pNullSoundSystemInstance;
    /*template <typename T>
    class ServiceLocator 
    {
    public:
        static void Provide(T* service) 
        {
            Instance() = service;
        }

        static T& Get() 
        {
            if (!Instance()) 
            {
                Instance() = new T();
            }
            return *Instance();
        }

    private:
        static T*& Instance() 
        {
            static T* service = nullptr;
            return service;
        }
    };*/
}