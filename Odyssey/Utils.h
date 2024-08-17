#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


namespace ody
{

#pragma region Forward Declarations

#pragma endregion Forward Declarations

/**
 * \brief Here you can place any helper functions you might need
 */
class Utils final
{
public:
	static bool CheckChance(int chance);

	static bool AreEqual(float a, float b, float epsilon = 0.0001f);
	static bool AreEqual(const glm::vec2& a, const glm::vec2& b, float epsilon = 0.0001f);
	static bool AreEqual(const glm::vec3& a, const glm::vec3& b, float epsilon = 0.0001f);
	static bool AreEqual(const glm::vec4& a, const glm::vec4& b, float epsilon = 0.0001f);
};

}
