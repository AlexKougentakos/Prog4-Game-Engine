#include "Utils.h"
#include "Constants.h"
#include <stdexcept>

#include <random>

namespace ody
{

	bool Utils::CheckChance(int chance)
	{
		if (chance < 0 || chance > 100) 
		{
			throw std::invalid_argument("percentage must be between 0 and 100");
		}

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(1, 100);

		const int roll = distr(gen);

		return roll <= chance;
	}

}
