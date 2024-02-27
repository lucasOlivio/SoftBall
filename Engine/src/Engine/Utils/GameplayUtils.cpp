#include "pch.h"

#include "GameplayUtils.h"

#include "Engine/ECS/Components.h"
#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/Scene/SceneManagerLocator.h"

#include "Engine/Utils/TransformUtils.h"
#include "Engine/Utils/Math.h"

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
										      const glm::vec3& position, 
										      const ModelComponent& modelInfo)
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

        // Create the node entities
        for (uint i = 0; i < numOfNodes; i++)
        {
            // Calculate position based on the middle of the top and down particles holding this node
            SoftBodyParticle* pParticleChainTop = pSoftBody->vecParticles[i];
            SoftBodyParticle* pParticleChainDown = pSoftBody->vecParticles[i + 1];
            const glm::vec3& top = pParticleChainTop->position;
            const glm::vec3& down = pParticleChainDown->position;
            glm::vec3 positionNode = top;
            positionNode.y = (positionNode.y + down.y) / 2.0f;

            Entity nodeId = pScene->CreateEntity();

            TagComponent* pTag = pScene->AddComponent<TagComponent>(nodeId);
            pTag->name = "node";
            TransformComponent* pTransform = pScene->AddComponent<TransformComponent>(nodeId);
            pTransform->position = positionNode;
            pTransform->orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            pTransform->scale = 1.0f;
            MovementComponent* pMovement = pScene->AddComponent<MovementComponent>(nodeId);
            pMovement->acceleration = glm::vec3(0.0f, -100.0f, 0.0f);
            pMovement->maxSpeed = 0.0f;
            SoftBodyComponent* pSoftBodyNode = pScene->AddComponent<SoftBodyComponent>(nodeId);
            pSoftBodyNode->isInternalSprings = false;
            pSoftBodyNode->isWireframe = false;
            pSoftBodyNode->iterations = 1;
            pSoftBodyNode->springStrength = 1.0f;
            pSoftBodyNode->meshName = "";
            pSoftBodyNode->position = positionNode;
            pSoftBodyNode->orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

            // Create the top and down particles to hold the node
            SoftBodyParticle* pParticleTop = new SoftBodyParticle();
            pParticleTop->entityId = nodeId;
            pParticleTop->position = top;
            pParticleTop->oldPosition = pParticleTop->position;
            pParticleTop->worldPosition = pParticleTop->position;
            pSoftBodyNode->vecParticles.push_back(pParticleTop);

            SoftBodyParticle* pParticleDown = new SoftBodyParticle();
            pParticleDown->entityId = nodeId;
            pParticleDown->position = down;
            pParticleDown->oldPosition = pParticleDown->position;
            pParticleDown->worldPosition = pParticleDown->position;
            pSoftBodyNode->vecParticles.push_back(pParticleDown);

            // Create the spring to hold top-down
            SoftBodySpring* pSpringTopDown = new SoftBodySpring();
            pSpringTopDown->restLength = glm::distance(pParticleTop->position, pParticleDown->position);
            pSpringTopDown->particleA = pParticleTop;
            pSpringTopDown->particleB = pParticleDown;
            pSoftBodyNode->vecSprings.push_back(pSpringTopDown);

            // Create the springs to hold top-top down-down
            SoftBodySpring* pSpringTop = new SoftBodySpring();
            pSpringTop->restLength = 0.0f;
            pSpringTop->particleA = pParticleChainTop;
            pSpringTop->particleB = pParticleTop;
            pSoftBodyNode->vecSprings.push_back(pSpringTop);

            SoftBodySpring* pSpringDown = new SoftBodySpring();
            pSpringDown->restLength = 0.0f;
            pSpringDown->particleA = pParticleChainDown;
            pSpringDown->particleB = pParticleDown;
            pSoftBodyNode->vecSprings.push_back(pSpringDown);

            // HACK: +2 for the orientation particles
            SoftBodyParticle* pParticleNodeFront = new SoftBodyParticle();
            pParticleNodeFront->entityId = chainId;
            pParticleNodeFront->position = TransformUtils::LocalToWorldPoint(FORWARD_VECTOR,
                pSoftBody->position,
                pSoftBody->orientation,
                pTransform->worldScale);
            pParticleNodeFront->oldPosition = pParticleNodeFront->position;
            pParticleNodeFront->worldPosition = pParticleNodeFront->position;

            pSoftBodyNode->vecParticles.push_back(pParticleNodeFront);

            SoftBodyParticle* pParticleNodeUp = new SoftBodyParticle();
            pParticleNodeUp->entityId = chainId;
            pParticleNodeUp->position = TransformUtils::LocalToWorldPoint(UP_VECTOR,
                pSoftBody->position,
                pSoftBody->orientation,
                pTransform->worldScale);
            pParticleNodeUp->oldPosition = pParticleNodeUp->position;
            pParticleNodeUp->worldPosition = pParticleNodeUp->position;

            pSoftBodyNode->vecParticles.push_back(pParticleNodeUp);

            ModelComponent* pModel = pScene->AddComponent<ModelComponent>(nodeId);
            *pModel = modelInfo;
        }

        return chainId;
    }
}
