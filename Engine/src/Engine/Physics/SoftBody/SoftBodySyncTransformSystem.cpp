#include "pch.h"

#include "SoftBodySyncTransformSystem.h"

#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/Components.h"

#include "Engine/Utils/Math.h"
#include "Engine/Utils/TransformUtils.h"
#include "Engine/Utils/SoftBodyUtils.h"

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

            if (pSoftBody->vecParticles.size() == 0)
            {
                continue;
            }

            if (pSoftBody->isInternalSprings)
            {
                // HACK: Last two particles will always be the orientational particles
                std::vector<SoftBodyParticle*>::reverse_iterator it = pSoftBody->vecParticles.rbegin();
                SoftBodyParticle* pParticleUp = *it;
                SoftBodyParticle* pParticleForward = *(it + 1);
                glm::vec3 up = glm::normalize(TransformUtils::WorldToLocalPoint(pParticleUp->position,
                                                                                pTransform->worldPosition,
                                                                                pTransform->worldScale));
                glm::vec3 forward = glm::normalize(TransformUtils::WorldToLocalPoint(pParticleForward->position,
                                                                                    pTransform->worldPosition,
                                                                                    pTransform->worldScale));
                glm::quat rot = glm::quatLookAt(forward, up);

                pSoftBody->orientation = rot;

                glm::vec3 weightedSum = glm::vec3(0.0f);
                float totalWeight = 0.0f;
                size_t vecSize = pSoftBody->vecParticles.size();

                // HACK: The -2 is to avoid the "reference" particles for forward and up
                for (size_t i = 0; i < vecSize - 2; i++)
                {
                    SoftBodyParticle* pParticle = pSoftBody->vecParticles[i];

                    // Calculate weighted sum of particle positions
                    float distanceSquared = glm::length(pParticle->position - pTransform->position);
                    float weight = 1.0f / (distanceSquared + 1.0f); // Adding 1 to avoid division by zero

                    // HACK: Updating transform based on particles average position, this should not be here
                    weightedSum += pParticle->position * weight;
                    totalWeight += weight;
                }

                if (totalWeight <= 0.1f) 
                {
                    continue;
                }

                // Update transform position to weighted average position of particles
                glm::vec3 averagePosition = weightedSum / totalWeight;
                pTransform->position = averagePosition;
                pSoftBody->position = averagePosition;
            }

            if (pSoftBody->isWireframe)
            {
                if (!pSoftBody->isInternalSprings)
                {
                    SoftBodyUtils::UpdateSpringConstraint(pSoftBody->position, pTransform->position,
                                            0.0f, 1.0f, 5, true);
                }

                for (int i = 0; i < pSoftBody->vecWireframeParticles.size(); i++)
                {
                    SoftBodyParticle* pParticle = pSoftBody->vecWireframeParticles[i];

                    pParticle->worldPosition = TransformUtils::LocalToWorldPoint(pParticle->position,
                                                                                    pSoftBody->position,
                                                                                    pSoftBody->orientation,
                                                                                    pTransform->worldScale);
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
