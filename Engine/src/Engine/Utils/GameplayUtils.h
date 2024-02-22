#pragma once

#include <glm/glm.hpp>
#include <functional>

namespace MyEngine
{
	using StopConditionFunction = std::function<bool(float, float)>;

	class GameplayUtils
	{
	public:
		static void CalculateSteeringDirections(const glm::vec3& myPosition, const glm::vec3& targetPosition,
												glm::quat& myOrientation, glm::vec3& myVelocity,
												const glm::vec3& targetVelocity, bool isFleeing,
												float speed, float maxDistance, float futureTime,
												StopConditionFunction stopCondition);
	};
}
