#pragma once

#include "Engine/ECS/Components.h"

namespace MyEngine
{
	class SoftBodyUtils
	{
	public:
		static void UpdateSpringConstraint(glm::vec3& positionA, glm::vec3& positionB,
									const float& restLength, const float& springStregth,
									int iterations, bool moveB = true);

		static void ApplyVerlet(glm::vec3& position, glm::vec3& oldPosition,
										const glm::vec3& acceleration, float deltaTime);
		
		static void CreateParticlesAndSprings(Entity entityId, 
											  TransformComponent* pTransform, 
											  SoftBodyComponent* pSoftBody);

		static void ClearSoftBody(SoftBodyComponent* pSoftBody);
	};
}
