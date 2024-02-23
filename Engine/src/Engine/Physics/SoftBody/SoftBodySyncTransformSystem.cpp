#include "pch.h"

#include "SoftBodySyncTransformSystem.h"

#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/Components.h"

#include "Engine/Utils/Math.h"
#include "Engine/Utils/TransformUtils.h"

namespace MyEngine
{
    void SoftBodySyncTransformSystem::Init()
    {
    }

    void SoftBodySyncTransformSystem::Start(Scene* pScene)
    {
        return;
    }

    void SoftBodySyncTransformSystem::Update(Scene* pScene, float deltaTime)
    {
        // TODO: Maybe separate components for this whole wireframe/internalsprings?
        // Update transform position and rotation based on softbody "wireframe" particles
        for (Entity entityId : SceneView<TransformComponent, SoftBodyComponent>(*pScene))
        {
            TransformComponent* pTransform = pScene->Get<TransformComponent>(entityId);
            SoftBodyComponent* pSoftBody = pScene->Get<SoftBodyComponent>(entityId);

            if (pSoftBody->isWireframe)
            {
                for (int i = 0; i < 5; i++)
                {
                    UpdateSpringConstraint(pSoftBody->position, pTransform->position,
                        0.0f, 1.0f, true);
                }

                for (int i = 0; i < pSoftBody->vecWireframeParticles.size(); i++)
                {
                    SoftBodyParticle* pParticle = pSoftBody->vecWireframeParticles[i];

                    pParticle->worldPosition = TransformUtils::LocalToWorldPoint(pParticle->position,
                        pSoftBody->position);
                }
            }

            if (pSoftBody->isInternalSprings)
            {
                glm::vec3 weightedSum = glm::vec3(0.0f);
                glm::vec3 weightedDirection = glm::vec3(0.0f);
                float totalWeight = 0.0f;
                size_t vecSize = pSoftBody->vecParticles.size();
                for (size_t i = 0; i < vecSize; i++)
                {
                    SoftBodyParticle* pParticle = pSoftBody->vecParticles[i];

                    // HACK: Updating transform based on particles average position, this should not be here

                    // Calculate weighted sum of particle positions
                    float distanceSquared = glm::length(pParticle->position - pTransform->worldPosition);
                    float weight = 1.0f / (distanceSquared + 1.0f); // Adding 1 to avoid division by zero
                    weightedSum += pParticle->position * weight;
                    weightedDirection += glm::normalize(pParticle->position - pTransform->worldPosition) * weight;
                    totalWeight += weight;
                }

                // Update transform position to weighted average position of particles
                if (totalWeight > 0.0f) {
                    glm::vec3 averagePosition = weightedSum / totalWeight;
                    pTransform->position = averagePosition;
                    pSoftBody->position = averagePosition;

                    // Calculate rotation to align with the weighted direction
                    glm::quat rotation = glm::quatLookAt(glm::normalize(weightedDirection), glm::vec3(UP_VECTOR));
                    pTransform->orientation = rotation;
                    pSoftBody->orientation = rotation;
                }
            }
        }
    }

    void SoftBodySyncTransformSystem::Render(Scene* pScene)
    {
    }

    void SoftBodySyncTransformSystem::End(Scene* pScene)
    {
    }

    void SoftBodySyncTransformSystem::Shutdown()
    {
    }
}
