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
            std::string meshCopy = pSoftBody->meshName + std::to_string(entityId);
            sMesh* pMesh = pVAOManager->LoadModelCopyIntoVAO(pSoftBody->meshName, true, true, false, meshCopy);
            if (!pMesh)
            {
                continue;
            }

            size_t sizeVertices = static_cast<size_t>(pMesh->numberOfVertices);

            m_ClearSoftBody(pSoftBody);
            pSoftBody->vecParticles.reserve(sizeVertices);
            pSoftBody->position = pTransform->position;
            pSoftBody->orientation = pTransform->orientation;
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

            // HACK: Create particles for forward and up reference
            SoftBodyParticle* pParticleFront = new SoftBodyParticle();
            pParticleFront->entityId = entityId;
            pParticleFront->position = FORWARD_VECTOR;
            pParticleFront->oldPosition = pParticleFront->position;
            pParticleFront->worldPosition = pParticleFront->position;

            pSoftBody->vecParticles.push_back(pParticleFront);

            SoftBodyParticle* pParticleUp = new SoftBodyParticle();
            pParticleUp->entityId = entityId;
            pParticleUp->position = UP_VECTOR;
            pParticleUp->oldPosition = pParticleUp->position;
            pParticleUp->worldPosition = pParticleUp->position;

            pSoftBody->vecParticles.push_back(pParticleUp);

            // Create "wireframe" springs and orientation springs, to hold the particles to their original location
            for (unsigned int i = 0; i < pSoftBody->vecParticles.size() - 2; i++)
            {
                SoftBodyParticle* particleA = pSoftBody->vecParticles[i];
                SoftBodyParticle* particleB = new SoftBodyParticle();

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

                // Reference springs
                SoftBodySpring* pSpringForward = new SoftBodySpring();
                pSpringForward->restLength = glm::distance(particleA->position, pParticleFront->position);
                pSpringForward->particleA = particleA;
                pSpringForward->particleB = pParticleFront;
                pSoftBody->vecSprings.push_back(pSpringForward);

                SoftBodySpring* pSpringUp = new SoftBodySpring();
                pSpringUp->restLength = glm::distance(particleA->position, pParticleUp->position);
                pSpringUp->particleA = particleA;
                pSpringUp->particleB = pParticleUp;
                pSoftBody->vecSprings.push_back(pSpringUp);
            }

            // HACK: Set particle to be at the world position initially
            for (unsigned int i = 0; i < pSoftBody->vecParticles.size() - 2; i++)
            {
                SoftBodyParticle* particle = pSoftBody->vecParticles[i];
                particle->position = TransformUtils::LocalToWorldPoint(particle->position,
                                                                        pSoftBody->position,
                                                                        pSoftBody->orientation,
                                                                        pTransform->worldScale);
                particle->oldPosition = particle->position;
                particle->worldPosition = particle->position;
            }

            pParticleUp->position = TransformUtils::LocalToWorldPoint(pParticleUp->position,
                                                                    pSoftBody->position,
                                                                    pTransform->worldScale);
            pParticleUp->oldPosition = pParticleUp->position;
            pParticleUp->worldPosition = pParticleUp->position;

            pParticleFront->position = TransformUtils::LocalToWorldPoint(pParticleFront->position,
                                                                    pSoftBody->position,
                                                                    pTransform->worldScale);
            pParticleFront->oldPosition = pParticleFront->position;
            pParticleFront->worldPosition = pParticleFront->position;

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
                                       pSpring->restLength, pSoftBody->springStrength,
                                        pSoftBody->iterations);
            }

            for (SoftBodySpring* pSpring : pSoftBody->vecWireframeSprings)
            {
                SoftBodyParticle* particleA = pSpring->particleA;
                SoftBodyParticle* particleB = pSpring->particleB;

                UpdateSpringConstraint(particleA->position, particleB->worldPosition,
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

        for (SoftBodySpring* pSpring : pSoftBody->vecWireframeSprings)
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
        pSoftBody->vecWireframeSprings.clear();
        pSoftBody->vecParticles.clear();
        pSoftBody->vecWireframeParticles.clear();
    }
}
