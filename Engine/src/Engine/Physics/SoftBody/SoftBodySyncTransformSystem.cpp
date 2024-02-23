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
        // Update transform position and rotation based on softbody "wireframe" particles
        for (Entity entityId : SceneView<TransformComponent, SoftBodyComponent>(*pScene))
        {
            TransformComponent* pTransform = pScene->Get<TransformComponent>(entityId);
            SoftBodyComponent* pSoftBody = pScene->Get<SoftBodyComponent>(entityId);
            
            // Adjust with transform center
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
