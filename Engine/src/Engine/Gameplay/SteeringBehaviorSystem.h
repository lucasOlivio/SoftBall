#pragma once

#include "Engine/ECS/System/iSystem.h"

namespace MyEngine
{
	class SteeringBehaviorSystem : public iSystem
	{
	public:
		SteeringBehaviorSystem() = default;
		virtual ~SteeringBehaviorSystem() { };

		virtual std::string SystemName() { return "SteeringBehaviorSystem"; };

		virtual void Init();

		virtual void Start(Scene* pScene);

		virtual void Update(Scene* pScene, float deltaTime);

		virtual void Render(Scene* pScene);

		virtual void End(Scene* pScene);

		virtual void Shutdown();
	private:
		void m_SeekTarget(const glm::vec3& myPosition, const glm::vec3& targetPosition,
							glm::quat& myOrientation, glm::vec3& myVelocity,
							float speed, float maxDistance, float futureTime);
		
		void m_FleeFromTarget(const glm::vec3& myPosition, const glm::vec3& targetPosition,
							  glm::quat& myOrientation, glm::vec3& myVelocity,
							  float speed, float maxDistance, float futureTime);
		
		void m_PursueTarget(const glm::vec3& myPosition, const glm::vec3& targetPosition,
							glm::quat& myOrientation, glm::vec3& myVelocity, const glm::vec3& targetVelocity,
							float speed, float maxDistance, float futureTime);
		
		void m_EvadeTarget(const glm::vec3& myPosition, const glm::vec3& targetPosition,
							glm::quat& myOrientation, glm::vec3& myVelocity, const glm::vec3& targetVelocity,
							float speed, float maxDistance, float futureTime);
		
		void m_ApproachTarget(const glm::vec3& myPosition, const glm::vec3& targetPosition,
							  glm::quat& myOrientation, glm::vec3& myVelocity,
							  float speed, float maxDistance, float futureTime);
	};
}
