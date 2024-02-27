#pragma once

#include "Engine/ECS/Scene/Scene.h"
#include "Engine/ECS/Components.h"

#include <glm/glm.hpp>
#include <functional>

namespace MyEngine
{
	using StopConditionFunction = std::function<bool(float, float)>;

	class GameplayUtils
	{
	public:
		static Entity GetPlayerId(Scene* pScene);

		static void CalculateSteeringDirections(const glm::vec3& myPosition, const glm::vec3& targetPosition,
												glm::quat& myOrientation, glm::vec3& myVelocity,
												const glm::vec3& targetVelocity, bool isFleeing,
												float speed, float maxDistance, float futureTime,
												StopConditionFunction stopCondition);

		// Pass the number of chains to create and the name of the mesh used as the chain node,
		// It creates a softbody entity that has the "nodeMesh" attached to each node
		// NOTE: The node mesh will be created initially with 0 rotation and in the middle of each particle.
		static Entity CreateSoftBodyChain(uint numOfNodes, float nodeSpacing,
										  int iterations, float springStrength,
										  const glm::vec3& position, 
										  const ModelComponent& modelInfo); // HACK: Just temp component to send all the needed parameters to create the node
	};
}
