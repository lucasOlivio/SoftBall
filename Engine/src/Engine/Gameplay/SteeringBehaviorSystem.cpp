#include "pch.h"

#include "SteeringBehaviorSystem.h"

#include "Engine/Gameplay/GameplayProperties.h"

#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/Components.h"

#include "Engine/Utils/Math.h"
#include "Engine/Utils/TransformUtils.h"
#include "Engine/Utils/GameplayUtils.h"

namespace MyEngine
{
    void SteeringBehaviorSystem::Init()
    {
    }

    void SteeringBehaviorSystem::Start(Scene* pScene)
    {
    }

    void SteeringBehaviorSystem::Update(Scene* pScene, float deltaTime)
    {
        // Update velocity and position
        for (Entity entityId : SceneView<TransformComponent, MovementComponent, SteeringBehaviorComponent>(*pScene))
        {
            TransformComponent* pTransform = pScene->Get<TransformComponent>(entityId);
            MovementComponent* pMovement = pScene->Get<MovementComponent>(entityId);
            SteeringBehaviorComponent* pSteeringBehavior = pScene->Get<SteeringBehaviorComponent>(entityId);

            TransformComponent* pTransformTarget = pScene->Get<TransformComponent>(pSteeringBehavior->targetId);
            if (!pTransformTarget)
            {
                continue;
            }

            switch (pSteeringBehavior->steeringType)
            {
            case eSteeringTypes::SEEK:
                m_SeekTarget(pTransform->worldPosition, pTransformTarget->worldPosition,
							    pTransform->orientation, pMovement->velocity,
                                pSteeringBehavior->speed, pSteeringBehavior->maxDistance, 
                                pSteeringBehavior->futureTime);
                break;
            case eSteeringTypes::FLEE:
                m_FleeFromTarget(pTransform->worldPosition, pTransformTarget->worldPosition,
							    pTransform->orientation, pMovement->velocity,
                                pSteeringBehavior->speed, pSteeringBehavior->maxDistance, 
                                pSteeringBehavior->futureTime);
                break;
            case eSteeringTypes::PURSUE:
            {
                glm::vec3 targetVelocity = glm::vec3(0.0f);
                MovementComponent* pMovementTarget = pScene->Get<MovementComponent>(pSteeringBehavior->targetId);
                if (pMovementTarget)
                {
                    targetVelocity = pMovementTarget->velocity;
                }

                m_PursueTarget(pTransform->worldPosition, pTransformTarget->worldPosition,
                    pTransform->orientation, pMovement->velocity, targetVelocity,
                    pSteeringBehavior->speed, pSteeringBehavior->maxDistance,
                    pSteeringBehavior->futureTime);
            }
                break;
            case eSteeringTypes::EVADE:
            {
                glm::vec3 targetVelocity = glm::vec3(0.0f);
                MovementComponent* pMovementTarget = pScene->Get<MovementComponent>(pSteeringBehavior->targetId);
                if (pMovementTarget)
                {
                    targetVelocity = pMovementTarget->velocity;
                }

                m_EvadeTarget(pTransform->worldPosition, pTransformTarget->worldPosition,
                    pTransform->orientation, pMovement->velocity, targetVelocity,
                    pSteeringBehavior->speed, pSteeringBehavior->maxDistance,
                    pSteeringBehavior->futureTime);
            }
            break;
            case eSteeringTypes::APPROACH:
                m_ApproachTarget(pTransform->worldPosition, pTransformTarget->worldPosition,
                    pTransform->orientation, pMovement->velocity,
                    pSteeringBehavior->speed, pSteeringBehavior->maxDistance,
                    pSteeringBehavior->futureTime);
            break;
            default:
                break;
            }
        }
    }

    void SteeringBehaviorSystem::Render(Scene* pScene)
    {
    }

    void SteeringBehaviorSystem::End(Scene* pScene)
    {
    }

    void SteeringBehaviorSystem::Shutdown()
    {
    }

    void SteeringBehaviorSystem::m_SeekTarget(const glm::vec3& myPosition, const glm::vec3& targetPosition,
        glm::quat& myOrientation, glm::vec3& myVelocity,
        float speed, float maxDistance, float futureTime)
    {
        GameplayUtils::CalculateSteeringDirections(myPosition, targetPosition, myOrientation, myVelocity,
            glm::vec3(0.0f), false, speed, maxDistance, futureTime,
            [](float distToTarget, float maxDistance) {
                return distToTarget < maxDistance;
            });
    }

    void SteeringBehaviorSystem::m_FleeFromTarget(const glm::vec3& myPosition, const glm::vec3& targetPosition,
        glm::quat& myOrientation, glm::vec3& myVelocity,
        float speed, float maxDistance, float futureTime)
    {
        GameplayUtils::CalculateSteeringDirections(myPosition, targetPosition, myOrientation, myVelocity,
            glm::vec3(0.0f), true, speed, maxDistance, futureTime,
            [](float distToTarget, float maxDistance) {
                return distToTarget > maxDistance;
            });
    }

    void SteeringBehaviorSystem::m_PursueTarget(const glm::vec3& myPosition, const glm::vec3& targetPosition,
        glm::quat& myOrientation, glm::vec3& myVelocity,
        const glm::vec3& targetVelocity, float speed, float maxDistance, float futureTime)
    {
        GameplayUtils::CalculateSteeringDirections(myPosition, targetPosition, myOrientation, myVelocity,
            targetVelocity, false, speed, maxDistance, futureTime,
            [](float distToTarget, float maxDistance) {
                return distToTarget < maxDistance;
            });
    }

    void SteeringBehaviorSystem::m_EvadeTarget(const glm::vec3& myPosition, const glm::vec3& targetPosition,
        glm::quat& myOrientation, glm::vec3& myVelocity,
        const glm::vec3& targetVelocity, float speed, float maxDistance, float futureTime)
    {
        GameplayUtils::CalculateSteeringDirections(myPosition, targetPosition, myOrientation, myVelocity,
            targetVelocity, true, speed, maxDistance, futureTime,
            [](float distToTarget, float maxDistance) {
                return distToTarget > maxDistance;
            });
    }

    void SteeringBehaviorSystem::m_ApproachTarget(const glm::vec3& myPosition, const glm::vec3& targetPosition,
        glm::quat& myOrientation, glm::vec3& myVelocity,
        float speed, float maxDistance, float futureTime)
    {
        GameplayUtils::CalculateSteeringDirections(myPosition, targetPosition, myOrientation, myVelocity,
                        glm::vec3(0.0f), false, speed, maxDistance, futureTime,
                        [](float distToTarget, float maxDistance) {
                            float distToMaxDist = maxDistance - distToTarget;
                            return (distToMaxDist < 1.0f && distToMaxDist > -1.0f);
                        });
    }
}
