#include "pch.h"

#include "SoftBodyResponseSystem.h"

#include "Engine/ECS/Components.h"

#include "Engine/Events/CollisionEvent.h"
#include "Engine/Events/EventBusLocator.hpp"

#include "Engine/Utils/CollisionsUtils.h"


namespace MyEngine
{
    void SoftBodyResponseSystem::Init()
    {
    }

    void SoftBodyResponseSystem::Start(Scene* pScene)
    {
    }

    void SoftBodyResponseSystem::Update(Scene* pScene, float deltaTime)
    {
        const std::set<sCollisionParticleData>& currFrameColls = CollisionsUtils::CurrentFrameParticleCollisions();
        // Draw a wireframe blue sphere at the contact points
        for (const sCollisionParticleData& coll : currFrameColls)
        {
            glm::vec3 collNormal = coll.collisionNormalB;
            Entity entityAId = coll.entityA;

            Scene* pScene = coll.pScene;
            SoftBodyParticle* pParticle = coll.pParticle;
            RigidBodyComponent* pRigidBodyA = pScene->Get<RigidBodyComponent>(entityAId);
            TransformComponent* pTransformA = pScene->Get<TransformComponent>(entityAId);

            glm::vec3& particlePosition = pParticle->position;
            switch (pRigidBodyA->shapeType)
            {
            case eShape::SPHERE:
            {
                float radius = pScene->Get<SphereColliderComponent>(entityAId)->radius;

                // How much the particle penetrated
                float displacement = glm::length(particlePosition - pTransformA->position);
                float penetrationDepth = displacement - radius;

                glm::vec3 correction = collNormal * penetrationDepth * 20.0f;// HACK: Why just adjusting the penetration is not enough?

                // Move the particle outside the sphere along the collision normal
                particlePosition += correction;

                break;
            }
            default:
                break;
            }
        }
    }

    void SoftBodyResponseSystem::Render(Scene* pScene)
    {
    }

    void SoftBodyResponseSystem::End(Scene* pScene)
    {
    }

    void SoftBodyResponseSystem::Shutdown()
    {
    }
}
