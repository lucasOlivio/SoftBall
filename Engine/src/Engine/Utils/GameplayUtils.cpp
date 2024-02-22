#include "pch.h"

#include "GameplayUtils.h"

#include "Engine/Utils/TransformUtils.h"
#include "Engine/Utils/Math.h"

namespace MyEngine
{
	void GameplayUtils::CalculateSteeringDirections(const glm::vec3& myPosition, const glm::vec3& targetPosition, 
                                                   glm::quat& myOrientation, glm::vec3& myVelocity, 
                                                   const glm::vec3& targetVelocity, bool isFleeing, 
                                                   float speed, float maxDistance, 
                                                   float futureTime, StopConditionFunction stopCondition)
    {
        // Calculate direction to/from target
        glm::vec3 futurePosition = targetPosition + (targetVelocity * futureTime);
        glm::vec3 direction = futurePosition - myPosition;
        if (isFleeing)
        {
            direction = -direction;
        }

        // Normalize direction
        glm::vec3 lockedAxis = glm::vec3(1.0f, 0.0f, 1.0f);
        glm::vec3 directionNormal = glm::normalize(direction * lockedAxis);

        // Update orientation
        glm::vec3 currForward = TransformUtils::GetForwardVector(myOrientation);
        float angle = Angle(currForward, direction, UP_VECTOR);
        glm::vec3 yaw = angle * UP_VECTOR;
        myOrientation = TransformUtils::AdjustOrientation(myOrientation, yaw);

        // Update movement direction
        float distToTarget = glm::distance(futurePosition, myPosition);
        if (stopCondition(distToTarget, maxDistance))
        {
            myVelocity = glm::vec3(0.0f);

            return;
        }

        // HACK: Additional condition from m_ApproachTarget
        if (!isFleeing && distToTarget < maxDistance)
        {
            directionNormal = -directionNormal;
        }

        myVelocity = directionNormal * speed;
    }
}
