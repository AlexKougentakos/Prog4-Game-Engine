#include "Utils.h"
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

	bool Utils::AreEqual(float a, float b, float epsilon)
	{
		return std::abs(a - b) <= epsilon;
	}

	bool Utils::AreEqual(const glm::vec2& a, const glm::vec2& b, float epsilon)
	{
		return AreEqual(a.x, b.x, epsilon) && AreEqual(a.y, b.y, epsilon);
	}

	bool Utils::AreEqual(const glm::vec3& a, const glm::vec3& b, float epsilon)
	{
		return AreEqual(a.x, b.x, epsilon) && AreEqual(a.y, b.y, epsilon) && AreEqual(a.z, b.z, epsilon);
	}

	bool Utils::AreEqual(const glm::vec4& a, const glm::vec4& b, float epsilon)
	{
		return AreEqual(a.x, b.x, epsilon) && AreEqual(a.y, b.y, epsilon) &&
			AreEqual(a.z, b.z, epsilon) && AreEqual(a.w, b.w, epsilon);
	}
}
