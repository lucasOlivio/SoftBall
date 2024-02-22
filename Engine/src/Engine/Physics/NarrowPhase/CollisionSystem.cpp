#include "pch.h"

#include "CollisionSystem.h"

#include "Engine/ECS/Components.h"
#include "Engine/ECS/SingletonComponents/PhysicsLocator.h"
#include "Engine/ECS/SingletonComponents/CoreLocator.h"

#include "Engine/Events/EventBusLocator.hpp"
#include "Engine/Events/CollisionEvent.h"

#include "Engine/Utils/TransformUtils.h"
#include "Engine/Utils/CollisionsUtils.h"
#include "Engine/Utils/Math.h"

namespace MyEngine
{
    void CollisionSystem::Init()
    {
    }

    void CollisionSystem::Start(Scene* pScene)
    {
    }

    void CollisionSystem::Update(Scene* pScene, float deltaTime)
    {
        // Clear frame collisions
        CollisionsUtils::CurrentFrameCollisions().clear();
        CollisionsUtils::CurrentFrameParticleCollisions().clear();

        NarrowPhaseTestsComponent* pTests = PhysicsLocator::GetNarrowPhaseTests();

        // The first layer is the grouping of objects to test
        for (int group = 0; group < pTests->activeEntitiesToTest.size(); group++)
        {
            std::vector<Entity>& activeGroup = pTests->activeEntitiesToTest[group];
            std::vector<SoftBodyParticle*>& particlesGroup = pTests->particlesToTest[group];
            std::vector<Entity>& passiveGroup = pTests->passiveEntitiesToTest[group];
            std::vector<Entity>& staticGroup = pTests->staticEntitiesToTest[group];
            std::vector<sTriangle*>& trianglesGroup = pTests->trianglesToTest[group];

            for (int i = 0; i < activeGroup.size(); i++)
            {
                Entity entityId = activeGroup[i];
                TransformComponent* pTransform = pScene->Get<TransformComponent>(entityId);
                RigidBodyComponent* pRigidBody = pScene->Get<RigidBodyComponent>(entityId);

                // Get the right collider to test against
                SphereColliderComponent* pSphere;
                AABBColliderComponent* pAABB;
                switch (pRigidBody->shapeType)
                {
                case eShape::SPHERE:
                    pSphere = pScene->Get<SphereColliderComponent>(entityId);
                    m_CheckSphereOverlaps(pScene, entityId, pTransform, pSphere, i,
                                          activeGroup,
                                          particlesGroup,
                                          passiveGroup,
                                          staticGroup,
                                          trianglesGroup);

                    break;
                case eShape::AABB:
                    pAABB = pScene->Get<AABBColliderComponent>(entityId);
                    m_CheckAABBOverlaps(pScene, entityId, pTransform, pAABB, i,
                                        activeGroup,
                                        particlesGroup,
                                        passiveGroup,
                                        staticGroup,
                                        trianglesGroup);
                    break;
                default:
                    LOG_WARNING("Unknown shape for entity " + std::to_string(entityId));
                    continue;
                }
            }
        }
    }

    void CollisionSystem::Render(Scene* pScene)
    {
    }

    void CollisionSystem::End(Scene* pScene)
    {
    }

    void CollisionSystem::Shutdown()
    {
    }

    bool CollisionSystem::m_RegisterFrameCollision(const sCollisionData& collData)
    {
        bool isNewCollision = CollisionsUtils::CurrentFrameCollisions().insert(collData).second;

        return isNewCollision;
    }

    bool CollisionSystem::m_RegisterFrameCollision(const sCollisionParticleData& collData)
    {
        bool isNewCollision = CollisionsUtils::CurrentFrameParticleCollisions().insert(collData).second;

        return isNewCollision;
    }

    void CollisionSystem::m_TriggerCollision(const sCollisionData& collData)
    {
        bool isNewCollision = m_RegisterFrameCollision(collData);
        if (!isNewCollision)
        {
            return;
        }

        iEventBus<eCollisionEvents, RigidBodyCollisionEvent>* pEventBus = EventBusLocator<eCollisionEvents, RigidBodyCollisionEvent>::Get();

        RigidBodyCollisionEvent collEvent = RigidBodyCollisionEvent();
        collEvent.collisionData = collData;
        pEventBus->Publish(collEvent);
    }

    void CollisionSystem::m_TriggerCollision(const sCollisionParticleData& collData)
    {
        bool isNewCollision = m_RegisterFrameCollision(collData);
        if (!isNewCollision)
        {
            return;
        }

        iEventBus<eCollisionEvents, SoftBodyCollisionEvent>* pEventBus = EventBusLocator<eCollisionEvents, SoftBodyCollisionEvent>::Get();

        SoftBodyCollisionEvent collEvent = SoftBodyCollisionEvent();
        collEvent.collisionData = collData;
        pEventBus->Publish(collEvent);
    }

    void CollisionSystem::m_CheckSphereOverlaps(Scene* pScene,
								                Entity entityIdA,
                                                TransformComponent* pTransformA,
                                                SphereColliderComponent* pSphereA,
                                                const int index,
                                                const std::vector<Entity>& activeEntities,
                                                const std::vector<SoftBodyParticle*>& particles,
                                                const std::vector<Entity>& passiveEntities,
                                                const std::vector<Entity>& staticEntities,
                                                const std::vector<sTriangle*>& triangles)
    {
        // Start from one entity ahead so we dont test repeated
        for (int i = index + 1; i < activeEntities.size(); i++)
        {
            Entity entityIdB = activeEntities[i];
            bool isCollision = m_CheckSphereEntityOverlap(pScene, entityIdA,
                                                          pTransformA,
                                                          pSphereA, 
                                                          entityIdB);
        }

        for (int i = 0; i < staticEntities.size(); i++)
        {
            Entity entityIdB = activeEntities[i];
            bool isCollision = m_CheckSphereEntityOverlap(pScene, entityIdA,
                                                          pTransformA,
                                                          pSphereA, 
                                                          entityIdB);
        }

        MovementComponent* pMovementA = pScene->Get<MovementComponent>(entityIdA);
        // Triangles (from mesh collision) collisions
        for (int i = 0; i < triangles.size(); i++)
        {
            bool isCollision = CollisionsUtils::SphereTriangle_Overlap(pSphereA->radius,
                                                                       pTransformA->worldPosition,
                                                                       triangles[i]->vertices[0],
                                                                       triangles[i]->vertices[1],
                                                                       triangles[i]->vertices[2]);

            if (isCollision)
            {
                sCollisionData collData = sCollisionData();
                collData.entityA = entityIdA;
                collData.entityB = -1; // TODO: Get entity id from the triangle
                collData.contactPoint = ClosestPtPointTriangle(pTransformA->worldPosition, 
                                                               triangles[i]->vertices[0],
                                                               triangles[i]->vertices[1],
                                                               triangles[i]->vertices[2]);
                collData.collisionNormalA = CollisionsUtils::SphereSphere_Normal(pTransformA->worldPosition, 
                                                                                 collData.contactPoint);
                collData.collisionNormalB = -collData.collisionNormalA;
                collData.velocityAtCollisionA = pMovementA->velocity;

                collData.pScene = pScene;

                m_TriggerCollision(collData);
            }
        }

        // Particles collisions
        for (int i = 0; i < particles.size(); i++)
        {
            glm::vec3 point = particles[i]->position;
            Entity particleEntityId = particles[i]->entityId;
            if (entityIdA == particleEntityId)
            {
                continue;
            }

            bool isCollision = CollisionsUtils::PointSphere_Overlap(pSphereA->radius,
                                                                    pTransformA->worldPosition,
                                                                    point);

            if (isCollision)
            {
                sCollisionParticleData collData = sCollisionParticleData();
                collData.entityA = entityIdA;
                collData.pParticle = particles[i];
                collData.contactPoint = point;
                collData.collisionNormalA = CollisionsUtils::SphereSphere_Normal(pTransformA->worldPosition, collData.contactPoint);
                collData.collisionNormalB = -collData.collisionNormalA;

                collData.pScene = pScene;

                m_TriggerCollision(collData);
            }
        }
    }

    bool CollisionSystem::m_CheckSphereEntityOverlap(Scene* pScene,
										             Entity entityIdA,
										             TransformComponent* pTransformA,
										             SphereColliderComponent* pSphereA,
										             Entity entityIdB)
    {
        TransformComponent* pTransformB = pScene->Get<TransformComponent>(entityIdB);
        RigidBodyComponent* pRigidBodyB = pScene->Get<RigidBodyComponent>(entityIdB);

        MovementComponent* pMovementA = pScene->Get<MovementComponent>(entityIdA);
        MovementComponent* pMovementB = pScene->Get<MovementComponent>(entityIdB);

        // Get the right collider to test against
        bool isCollision = false;
        switch (pRigidBodyB->shapeType)
        {
        case eShape::SPHERE:
        {
            SphereColliderComponent* pSphereB = pScene->Get<SphereColliderComponent>(entityIdB);
            isCollision = CollisionsUtils::SphereSphere_Overlap(pSphereA->radius,
                                                                pTransformA->worldPosition,
                                                                pSphereB->radius,
                                                                pTransformB->worldPosition);

            if (isCollision)
            {
                sCollisionData collData = sCollisionData();
                collData.entityA = entityIdA;
                collData.entityB = entityIdB;
                collData.collisionNormalA = CollisionsUtils::SphereSphere_Normal(pTransformA->worldPosition, 
                                                                                 pTransformB->worldPosition);
                collData.collisionNormalB = -collData.collisionNormalA;
                collData.contactPoint = CollisionsUtils::SphereSphere_CollisionPoint(pSphereA->radius,
                                                                                      pTransformA->worldPosition,
                                                                                      collData.collisionNormalA,
                                                                                      pSphereB->radius,
                                                                                      pTransformB->worldPosition);
                collData.velocityAtCollisionA = pMovementA->velocity;

                collData.pScene = pScene;

                if (pMovementB)
                {
                    collData.velocityAtCollisionB = pMovementB->velocity;
                }

                m_TriggerCollision(collData);
            }

            break;
        }
        case eShape::AABB:
        {
            AABBColliderComponent* pAABBB = pScene->Get<AABBColliderComponent>(entityIdB);

            glm::vec3 worldMinB = TransformUtils::LocalToWorldPoint(pAABBB->min, 
                                                                   pTransformB->worldPosition,
                                                                   pTransformB->worldScale);
            glm::vec3 worldMaxB = TransformUtils::LocalToWorldPoint(pAABBB->max, 
                                                                   pTransformB->worldPosition,
                                                                   pTransformB->worldScale);

            isCollision = CollisionsUtils::SphereAABB_Overlap(pTransformA->worldPosition,
                                                              pSphereA->radius,
                                                              worldMinB,
                                                              worldMaxB);
        
            if (isCollision)
            {
                sCollisionData collData = sCollisionData();
                collData.entityA = entityIdA;
                collData.entityB = entityIdB;
                collData.contactPoint = CollisionsUtils::SphereAABB_CollisionPoint(worldMinB, worldMaxB,
                                                                                   pTransformA->worldPosition,
                                                                                   pSphereA->radius);
                collData.collisionNormalB = CollisionsUtils::AABB_Normal(worldMinB, worldMaxB,
										                                 collData.contactPoint);
                collData.collisionNormalA = -collData.collisionNormalB;
                collData.velocityAtCollisionA = pMovementA->velocity;

                collData.pScene = pScene;
                
                if (pMovementB)
                {
                    collData.velocityAtCollisionB = pMovementB->velocity;
                }

                m_TriggerCollision(collData);
            }

            break;
        }
        default:
            LOG_WARNING("Unknown shape for entity " + std::to_string(entityIdB));
            return false;
        }

        return isCollision;
    }

    void CollisionSystem::m_CheckAABBOverlaps(Scene* pScene,
								              Entity entityIdA,
								              TransformComponent* pTransformA,
								              AABBColliderComponent* pAABBA,
								              const int index,
								              const std::vector<Entity>& activeEntities,
                                              const std::vector<SoftBodyParticle*>& particles,
								              const std::vector<Entity>& passiveEntities,
								              const std::vector<Entity>& staticEntities,
								              const std::vector<sTriangle*>& triangles)
    {
        // Start from one entity ahead so we dont test repeated
        for (int j = index + 1; j < activeEntities.size(); j++)
        {
            Entity entityIdB = activeEntities[j];
            bool isCollision = m_CheckAABBEntityOverlap(pScene, entityIdA,
                                                        pTransformA,
                                                        pAABBA, 
                                                        entityIdB);
        }

        for (int j = 0; j < passiveEntities.size(); j++)
        {
            Entity entityIdB = activeEntities[j];
            bool isCollision = m_CheckAABBEntityOverlap(pScene, entityIdA,
                                                        pTransformA,
                                                        pAABBA,
                                                        entityIdB);
        }

        for (int j = 0; j < staticEntities.size(); j++)
        {
            Entity entityIdB = activeEntities[j];
            bool isCollision = m_CheckAABBEntityOverlap(pScene, entityIdA,
                                                        pTransformA,
                                                        pAABBA, 
                                                        entityIdB);
        }

        glm::vec3 worldMinA = TransformUtils::LocalToWorldPoint(pAABBA->min,
                                                                pTransformA->worldPosition,
                                                                pTransformA->worldScale);
        glm::vec3 worldMaxA = TransformUtils::LocalToWorldPoint(pAABBA->max,
                                                                pTransformA->worldPosition,
                                                                pTransformA->worldScale);

        MovementComponent* pMovementA = pScene->Get<MovementComponent>(entityIdA);
        // Triangles (from mesh collision) collisions
        for (int j = 0; j < triangles.size(); j++)
        {

            bool isCollision = CollisionsUtils::AABBTriangle_Overlap(worldMinA, worldMaxA,
                                                                     triangles[j]->vertices[0],
                                                                     triangles[j]->vertices[1],
                                                                     triangles[j]->vertices[2]);

            if (isCollision)
            {
                sCollisionData collData = sCollisionData();
                collData.entityA = entityIdA;
                collData.entityB = -1; // TODO: Get entity id from the triangle
                collData.contactPoint = ClosestPtPointTriangle(pTransformA->worldPosition,
                                                               triangles[j]->vertices[0],
                                                               triangles[j]->vertices[1],
                                                               triangles[j]->vertices[2]);
                collData.collisionNormalA = CollisionsUtils::AABB_Normal(worldMinA, worldMaxA,
										                                 collData.contactPoint);
                collData.collisionNormalB = -collData.collisionNormalA;
                collData.velocityAtCollisionA = pMovementA->velocity;

                collData.pScene = pScene;

                m_TriggerCollision(collData);
            }
        }

        // Particles collisions
        for (int i = 0; i < particles.size(); i++)
        {
            glm::vec3 point = particles[i]->position;
            Entity particleEntityId = particles[i]->entityId;
            if (entityIdA == particleEntityId)
            {
                continue;
            }

            bool isCollision = CollisionsUtils::PointAABB_Overlap(pAABBA->min, pAABBA->max, point);

            if (isCollision)
            {
                sCollisionParticleData collData = sCollisionParticleData();
                collData.entityA = entityIdA;
                collData.pParticle = particles[i];
                collData.contactPoint = point;
                collData.collisionNormalA = CollisionsUtils::SphereSphere_Normal(pTransformA->worldPosition, collData.contactPoint);
                collData.collisionNormalB = -collData.collisionNormalA;

                collData.pScene = pScene;

                m_TriggerCollision(collData);
            }
        }
    }

    bool CollisionSystem::m_CheckAABBEntityOverlap(Scene* pScene,
									               Entity entityIdA,
									               TransformComponent* pTransformA,
									               AABBColliderComponent* pAABBA,
									               Entity entityIdB)
    {
        TransformComponent* pTransformB = pScene->Get<TransformComponent>(entityIdB);
        RigidBodyComponent* pRigidBodyB = pScene->Get<RigidBodyComponent>(entityIdB);
        
        glm::vec3 worldMinA = TransformUtils::LocalToWorldPoint(pAABBA->min,
                                                                pTransformA->worldPosition,
                                                                pTransformA->worldScale);
        glm::vec3 worldMaxA = TransformUtils::LocalToWorldPoint(pAABBA->max,
                                                                pTransformA->worldPosition,
                                                                pTransformA->worldScale);

        MovementComponent* pMovementA = pScene->Get<MovementComponent>(entityIdA);
        MovementComponent* pMovementB = pScene->Get<MovementComponent>(entityIdB);

        // Get the right collider to test against
        bool isCollision = false;
        switch (pRigidBodyB->shapeType)
        {
        case eShape::SPHERE:
        {
            SphereColliderComponent* pSphereB = pScene->Get<SphereColliderComponent>(entityIdB);
            isCollision = CollisionsUtils::SphereAABB_Overlap(pTransformB->worldPosition, 
                                                              pSphereB->radius,
                                                              pAABBA->min,
                                                              pAABBA->max);

            if (isCollision)
            {
                sCollisionData collData = sCollisionData();
                collData.entityA = entityIdA;
                collData.entityB = entityIdB;
                collData.contactPoint = CollisionsUtils::SphereAABB_CollisionPoint(worldMinA, worldMaxA,
                                                                                   pTransformA->worldPosition,
                                                                                   pSphereB->radius);
                collData.collisionNormalB = CollisionsUtils::AABB_Normal(worldMinA, worldMaxA,
										                                 collData.contactPoint);
                collData.collisionNormalA = -collData.collisionNormalB;
                collData.velocityAtCollisionA = pMovementA->velocity;

                collData.pScene = pScene;

                if (pMovementB)
                {
                    collData.velocityAtCollisionB = pMovementB->velocity;
                }

                m_TriggerCollision(collData);
            }

            break;
        }
        case eShape::AABB:
        {
            AABBColliderComponent* pAABBB = pScene->Get<AABBColliderComponent>(entityIdB);
            
            glm::vec3 worldMinB = TransformUtils::LocalToWorldPoint(pAABBB->min,
                                                                    pTransformB->worldPosition,
                                                                    pTransformB->worldScale);
            glm::vec3 worldMaxB = TransformUtils::LocalToWorldPoint(pAABBB->max,
                                                                    pTransformB->worldPosition,
                                                                    pTransformB->worldScale);

            isCollision = CollisionsUtils::AABBAABB_Overlap(worldMinA,
                                                            worldMaxA,
                                                            worldMinB,
                                                            worldMaxB);
            
            if (isCollision)
            {
                sCollisionData collData = sCollisionData();
                collData.entityA = entityIdA;
                collData.entityB = entityIdB;
                collData.contactPoint = CollisionsUtils::AABBAABB_CollisionPoint(worldMinA, worldMaxA,
                                                                                 worldMinB, worldMaxB);
                collData.collisionNormalB = CollisionsUtils::AABB_Normal(worldMinA, worldMaxA,
										                                 collData.contactPoint);
                collData.collisionNormalA = -collData.collisionNormalB;
                collData.velocityAtCollisionA = pMovementA->velocity;

                collData.pScene = pScene;
                
                if (pMovementB)
                {
                    collData.velocityAtCollisionB = pMovementB->velocity;
                }

                m_TriggerCollision(collData);
            }

            break;
        }
        default:
            LOG_WARNING("Unknown shape for entity " + std::to_string(entityIdB));
            return false;
        }

        return isCollision;
    }
}