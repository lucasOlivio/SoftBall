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

            size_t sizeVertices = static_cast<size_t>(pMesh->numberOfVertices);

            m_ClearSoftBody(pSoftBody);
            pSoftBody->vecParticles.reserve(sizeVertices);
            pSoftBody->position = pTransform->worldPosition;
            pSoftBody->orientation = pTransform->worldOrientation;
            pSoftBody->oldPosition = pSoftBody->position;

            // Create all the particles needed
            for (size_t i = 0; i < sizeVertices; i++)
            {
                SoftBodyParticle* pParticle = new SoftBodyParticle();
                const sVertex& vertex = pMesh->pVertices[i];

                pParticle->entityId = entityId;
                pParticle->position = glm::vec3(vertex.x, vertex.y, vertex.z);
                pParticle->oldPosition = pParticle->position;
                pParticle->worldPosition = pParticle->position;

                pSoftBody->vecParticles.push_back(pParticle);
            }

            if (pSoftBody->isWireframe)
            {
                // Create "wireframe" springs, to hold the particles to their original location
                for (unsigned int i = 0; i < sizeVertices; i++)
                {
                    SoftBodyParticle* particleA = pSoftBody->vecParticles[i];
                    SoftBodyParticle* particleB = new SoftBodyParticle();
                    glm::mat4 transfMat = glm::mat4(1.0f);
                    particleB->entityId = entityId;
                    particleB->position = particleA->position;
                    particleB->oldPosition = particleB->position;
                    particleB->worldPosition = TransformUtils::LocalToWorldPoint(particleA->position,
                        pTransform->worldPosition,
                        pTransform->worldOrientation,
                        pTransform->worldScale);

                    pSoftBody->vecWireframeParticles.push_back(particleB);

                    SoftBodySpring* pSpringAB = new SoftBodySpring();
                    pSpringAB->restLength = 0.0f;
                    pSpringAB->particleA = particleA;
                    pSpringAB->particleB = particleB;
                    pSoftBody->vecWireframeSprings.push_back(pSpringAB);
                }
            }

            // HACK: Set particle to be at the world position initially
            for (unsigned int i = 0; i < sizeVertices; i++)
            {
                SoftBodyParticle* particle = pSoftBody->vecParticles[i];
                particle->position = TransformUtils::LocalToWorldPoint(particle->position,
                    pSoftBody->position,
                    pSoftBody->orientation,
                    pTransform->worldScale);
                particle->oldPosition = particle->position;
                particle->worldPosition = particle->position;
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
                if (!pSoftBody->isInternalSprings || i > halfIndices)
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
        // Update velocity and position
        for (Entity entityId : SceneView<SoftBodyComponent>(*pScene))
        {
            SoftBodyComponent* pSoftBody = pScene->Get<SoftBodyComponent>(entityId);

            for (SoftBodySpring* pSpring : pSoftBody->vecSprings)
            {
                SoftBodyParticle* particleA = pSpring->particleA;
                SoftBodyParticle* particleB = pSpring->particleB;

                UpdateSpringConstraint(particleA->position, particleB->position,
                                       pSpring->restLength, pSoftBody->defaultSpringStrength);
            }

            if (pSoftBody->isWireframe)
            {
                for (SoftBodySpring* pSpring : pSoftBody->vecWireframeSprings)
                {
                    SoftBodyParticle* particleA = pSpring->particleA;
                    SoftBodyParticle* particleB = pSpring->particleB;

                    UpdateSpringConstraint(particleA->position, particleB->worldPosition,
                        pSpring->restLength, pSoftBody->defaultSpringStrength,
                        false);
                }
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

            m_ClearSoftBody(pSoftBody);
        }
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

        for (SoftBodyParticle* pParticle : pSoftBody->vecWireframeParticles)
        {
            delete pParticle;
        }

        pSoftBody->vecSprings.clear();
        pSoftBody->vecParticles.clear();
        pSoftBody->vecWireframeParticles.clear();
    }
}
