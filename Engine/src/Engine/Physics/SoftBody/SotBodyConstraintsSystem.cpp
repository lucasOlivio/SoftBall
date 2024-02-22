#include "pch.h"

#include "SotBodyConstraintsSystem.h"

#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/Components.h"
#include "Engine/ECS/SingletonComponents/DebugLocator.h"

#include "Engine/Graphics/VAO/VAOManagerLocator.h"
#include "Engine/Graphics/Renderer/RendererManagerLocator.h"

#include "Engine/Utils/TransformUtils.h"
#include "Engine/Utils/Math.h"
#include "Engine/Utils/Random.h"

namespace MyEngine
{
    const std::string PREFIX_MESH = "softbody-";

    void SotBodyConstraintsSystem::Init()
    {
    }

    void SotBodyConstraintsSystem::Start(Scene* pScene)
    {
        iVAOManager* pVAOManager = VAOManagerLocator::Get();

        // Create all particles and springs for the soft body
        for (Entity entityId : SceneView<TransformComponent, SoftBodyComponent>(*pScene))
        {
            TransformComponent* pTransform = pScene->Get<TransformComponent>(entityId);
            SoftBodyComponent* pSoftBody = pScene->Get<SoftBodyComponent>(entityId);

            // Create a copy of the mesh into the VAO
            std::string meshCopy = PREFIX_MESH + pSoftBody->meshName + std::to_string(entityId);
            sMesh* pMesh = pVAOManager->LoadModelCopyIntoVAO(pSoftBody->meshName, true, true, false, meshCopy);

            size_t numParticles = static_cast<size_t>(pMesh->numberOfVertices);

            m_ClearSoftBody(pSoftBody);
            pSoftBody->vecParticles.reserve(numParticles);

            // Create all the particles needed
            for (unsigned int i = 0; i < pMesh->numberOfVertices; i++)
            {
                SoftBodyParticle* pParticle = new SoftBodyParticle();
                const sVertex& vertex = pMesh->pVertices[i];

                glm::mat4 transfMat = glm::mat4(1.0f);
                pParticle->entityId = entityId;
                pParticle->position = glm::vec3(vertex.x, vertex.y, vertex.z);
                pParticle->position = TransformUtils::LocalToWorldPoint(pParticle->position,
                                                                        pTransform->worldPosition,
                                                                        pTransform->worldOrientation,
                                                                        pTransform->worldScale, transfMat);
                pParticle->oldPosition = pParticle->position;

                pSoftBody->vecParticles.push_back(pParticle);
            }

            unsigned int halfIndices = static_cast<int>(pMesh->numberOfIndices / 2);
            uint32_t seed = static_cast<uint32_t>(entityId);
            // Create the particles and constraints based on original location from the mesh
            for (unsigned int i = 0; i < pMesh->numberOfIndices; i += 3)
            {
                SoftBodyParticle* particleA = pSoftBody->vecParticles[pMesh->pIndices[i]];
                SoftBodyParticle* particleB = pSoftBody->vecParticles[pMesh->pIndices[i + 1]];
                SoftBodyParticle* particleC = pSoftBody->vecParticles[pMesh->pIndices[i + 2]];

                SoftBodySpring* pSpringAB = new SoftBodySpring();
                pSpringAB->restLength = glm::distance(particleA->position, particleB->position);
                pSpringAB->particleA = particleA;
                pSpringAB->particleB = particleB;
                pSoftBody->vecSprings.push_back(pSpringAB);

                SoftBodySpring* pSpringBC = new SoftBodySpring();
                pSpringBC->restLength = glm::distance(particleB->position, particleC->position);
                pSpringBC->particleA = particleB;
                pSpringBC->particleB = particleC;
                pSoftBody->vecSprings.push_back(pSpringBC);

                SoftBodySpring* springCA = new SoftBodySpring();
                springCA->restLength = glm::distance(particleC->position, particleA->position);
                springCA->particleA = particleC;
                springCA->particleB = particleA;
                pSoftBody->vecSprings.push_back(springCA);

                // Create internal springs to help keep the structure
                // HACK: For now we keep fixed at half the vertices to hold everything
                if (i > halfIndices)
                {
                    continue;
                }

                int randomIndexA = Random::Int(seed, 0, pMesh->numberOfIndices - 1);
                int randomIndexB = Random::Int(seed, 0, pMesh->numberOfIndices - 1);
                int randomIndexC = Random::Int(seed, 0, pMesh->numberOfIndices - 1);

                SoftBodyParticle* particleA2 = pSoftBody->vecParticles[pMesh->pIndices[randomIndexA]];
                SoftBodyParticle* particleB2 = pSoftBody->vecParticles[pMesh->pIndices[randomIndexB]];
                SoftBodyParticle* particleC2 = pSoftBody->vecParticles[pMesh->pIndices[randomIndexC]];

                SoftBodySpring* pSpringA2 = new SoftBodySpring();
                pSpringA2->restLength = glm::distance(particleA->position, particleA2->position);
                pSpringA2->particleA = particleA;
                pSpringA2->particleB = particleA2;
                pSoftBody->vecSprings.push_back(pSpringA2);

                SoftBodySpring* pSpringB2 = new SoftBodySpring();
                pSpringB2->restLength = glm::distance(particleB->position, particleB2->position);
                pSpringB2->particleA = particleB;
                pSpringB2->particleB = particleB2;
                pSoftBody->vecSprings.push_back(pSpringB2);

                SoftBodySpring* springC2 = new SoftBodySpring();
                springC2->restLength = glm::distance(particleC->position, particleC2->position);
                springC2->particleA = particleC;
                springC2->particleB = particleC2;
                pSoftBody->vecSprings.push_back(springC2);
            }
        }
    }

    void SotBodyConstraintsSystem::Update(Scene* pScene, float deltaTime)
    {
        const float minFloat = 1.192092896e-07f;
        // Update velocity and position
        for (Entity entityId : SceneView<SoftBodyComponent>(*pScene))
        {
            SoftBodyComponent* pSoftBody = pScene->Get<SoftBodyComponent>(entityId);

            for (SoftBodySpring* pSpring : pSoftBody->vecSprings)
            {
                SoftBodyParticle* particleA = pSpring->particleA;
                SoftBodyParticle* particleB = pSpring->particleB;

                glm::vec3& positionA = particleA->position;
                glm::vec3& positionB = particleB->position;

                glm::vec3 delta = positionB - positionA;
                float deltalength = glm::length(delta);
                if (deltalength <= minFloat)
                {
                    continue;
                }

                float diffA = (deltalength - pSpring->restLength);
                float diff = diffA / deltalength;
                
                positionA += delta * 0.5f * diff * pSoftBody->defaultSpringStrength;
                positionB -= delta * 0.5f * diff * pSoftBody->defaultSpringStrength;
                
                CleanZeros(positionA);
                CleanZeros(positionB);
            }
        }
    }

    void SotBodyConstraintsSystem::Render(Scene* pScene)
    {
    }

    void SotBodyConstraintsSystem::End(Scene* pScene)
    {
    }

    void SotBodyConstraintsSystem::Shutdown()
    {
    }

    void SotBodyConstraintsSystem::m_ClearSoftBody(SoftBodyComponent* pSoftBody)
    {
        for (SoftBodySpring* pSpring : pSoftBody->vecSprings)
        {
            delete pSpring;
        }

        for (SoftBodyParticle* pParticle : pSoftBody->vecParticles)
        {
            delete pParticle;
        }

        pSoftBody->vecSprings.clear();
        pSoftBody->vecParticles.clear();
    }
}
