#include "pch.h"

#include "GameplayUtils.h"

#include "Engine/ECS/Components.h"
#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/Scene/SceneManagerLocator.h"

#include "Engine/Utils/TransformUtils.h"
#include "Engine/Utils/Math.h"
#include "Engine/Utils/SoftBodyUtils.h"

#include "Engine/Graphics/VAO/VAOManagerLocator.h"

namespace MyEngine
{
    Entity GameplayUtils::GetPlayerId(Scene* pScene)
    {
        SceneView<PlayerComponent>::Iterator it = SceneView<PlayerComponent>(*pScene).begin();

        return *it;
    }

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

    Entity GameplayUtils::CreateSoftBodyChain(uint numOfNodes, float nodeSpacing,  
										      int iterations, float springStrength,
										      const glm::vec3& position)
    {
        iVAOManager* pVAOManager = VAOManagerLocator::Get();
        iSceneManager* pSceneManager = SceneManagerLocator::Get();
        Scene* pScene = pSceneManager->GetCurrentScene();
        Entity chainId = pScene->CreateEntity();

        TagComponent* pTag = pScene->AddComponent<TagComponent>(chainId);
        pTag->name = "chain";
        TransformComponent* pTransformChain = pScene->AddComponent<TransformComponent>(chainId);
        pTransformChain->position = position;
        pTransformChain->orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        pTransformChain->scale = 1.0f;
        SoftBodyComponent* pSoftBody = pScene->AddComponent<SoftBodyComponent>(chainId);
        pSoftBody->isInternalSprings = false;
        pSoftBody->isWireframe = false;
        pSoftBody->iterations = iterations;
        pSoftBody->springStrength = springStrength;
        pSoftBody->meshName = "";
        MovementComponent* pMovementChain = pScene->AddComponent<MovementComponent>(chainId);
        pMovementChain->acceleration = glm::vec3(0.0f, -100.0f, 0.0f);
        pMovementChain->maxSpeed = 0.0f;

        // Create chain particles
        pSoftBody->vecParticles.reserve(numOfNodes);
        pSoftBody->position = pTransformChain->position;
        pSoftBody->orientation = pTransformChain->orientation;
        pSoftBody->oldPosition = pSoftBody->position;

        // Create all the particles needed
        // We attach each mesh to the upper and inferior node, so we create one particle extra for the last chain
        for (uint i = 0; i <= numOfNodes; i++)
        {
            SoftBodyParticle* pParticle = new SoftBodyParticle();

            pParticle->entityId = chainId;
            pParticle->position = glm::vec3(pTransformChain->position.x,
                                            pTransformChain->position.y - (i * nodeSpacing), 
                                            pTransformChain->position.z);
            pParticle->oldPosition = pParticle->position;
            pParticle->worldPosition = pParticle->position;

            pSoftBody->vecParticles.push_back(pParticle);
        }

        // HACK: +2 for the orientation particles
        SoftBodyParticle* pParticleFront = new SoftBodyParticle();
        pParticleFront->entityId = chainId;
        pParticleFront->position = TransformUtils::LocalToWorldPoint(FORWARD_VECTOR,
                                                                    pSoftBody->position,
                                                                    pSoftBody->orientation,
                                                                    pTransformChain->worldScale);
        pParticleFront->oldPosition = pParticleFront->position;
        pParticleFront->worldPosition = pParticleFront->position;

        pSoftBody->vecParticles.push_back(pParticleFront);

        SoftBodyParticle* pParticleUp = new SoftBodyParticle();
        pParticleUp->entityId = chainId;
        pParticleUp->position = TransformUtils::LocalToWorldPoint(UP_VECTOR,
                                                                pSoftBody->position,
                                                                pSoftBody->orientation,
                                                                pTransformChain->worldScale);
        pParticleUp->oldPosition = pParticleUp->position;
        pParticleUp->worldPosition = pParticleUp->position;

        pSoftBody->vecParticles.push_back(pParticleUp);

        // Create constraints for each node
        for (uint i = 0; i < numOfNodes; i++)
        {
            SoftBodyParticle* particleA = pSoftBody->vecParticles[i];
            SoftBodyParticle* particleB = pSoftBody->vecParticles[i + 1];

            SoftBodySpring* pSpringAB = new SoftBodySpring();
            pSpringAB->restLength = glm::distance(particleA->position, particleB->position);
            pSpringAB->particleA = particleA;
            pSpringAB->particleB = particleB;
            pSoftBody->vecSprings.push_back(pSpringAB);
        }

        return chainId;
    }
}
