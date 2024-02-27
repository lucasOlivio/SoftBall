#include "pch.h"

#include "SotBodyConstraintsSystem.h"

#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/Components.h"
#include "Engine/ECS/SingletonComponents/DebugLocator.h"

#include "Engine/Graphics/Renderer/RendererManagerLocator.h"

#include "Engine/Utils/Math.h"
#include "Engine/Utils/Random.h"
#include "Engine/Utils/SoftBodyUtils.h"

namespace MyEngine
{
    void SotBodyConstraintsSystem::Init()
    {
    }

    void SotBodyConstraintsSystem::Start(Scene* pScene)
    {
        // Create all particles and springs for the soft body
        for (Entity entityId : SceneView<TransformComponent, SoftBodyComponent>(*pScene))
        {
            TransformComponent* pTransform = pScene->Get<TransformComponent>(entityId);
            SoftBodyComponent* pSoftBody = pScene->Get<SoftBodyComponent>(entityId);

            SoftBodyUtils::CreateParticlesAndSprings(entityId, pTransform, pSoftBody);
        }
    }

    void SotBodyConstraintsSystem::Update(Scene* pScene, float deltaTime)
    {
        // Update velocity and position
        for (Entity entityId : SceneView<SoftBodyComponent>(*pScene))
        {
            SoftBodyComponent* pSoftBody = pScene->Get<SoftBodyComponent>(entityId);

            for (SoftBodySpring* pSpring : pSoftBody->vecSprings)
            {
                SoftBodyParticle* particleA = pSpring->particleA;
                SoftBodyParticle* particleB = pSpring->particleB;

                SoftBodyUtils::UpdateSpringConstraint(particleA->position, particleB->position,
                                       pSpring->restLength, pSoftBody->springStrength,
                                        pSoftBody->iterations);
            }

            for (SoftBodySpring* pSpring : pSoftBody->vecWireframeSprings)
            {
                SoftBodyParticle* particleA = pSpring->particleA;
                SoftBodyParticle* particleB = pSpring->particleB;

                SoftBodyUtils::UpdateSpringConstraint(particleA->position, particleB->worldPosition,
                    pSpring->restLength, pSoftBody->wireframeStrength,
                    pSoftBody->iterations, false);
            }
        }
    }

    void SotBodyConstraintsSystem::Render(Scene* pScene)
    {
    }

    void SotBodyConstraintsSystem::End(Scene* pScene)
    {
        // Update velocity and position
        for (Entity entityId : SceneView<SoftBodyComponent>(*pScene))
        {
            SoftBodyComponent* pSoftBody = pScene->Get<SoftBodyComponent>(entityId);

            SoftBodyUtils::ClearSoftBody(pSoftBody);
        }
    }

    void SotBodyConstraintsSystem::Shutdown()
    {
    }
}
