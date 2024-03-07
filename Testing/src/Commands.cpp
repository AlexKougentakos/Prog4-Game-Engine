#include "Commands.h"

#include "ServiceLocator.h"

void PlaySound::Execute()
{
	ody::ServiceLocator::GetSoundSystem().PlaySound(1);
}
