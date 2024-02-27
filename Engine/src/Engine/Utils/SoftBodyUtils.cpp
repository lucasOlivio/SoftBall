#include "pch.h"

#include "SoftBodyUtils.h"

#include "Engine/Graphics/VAO/VAOManagerLocator.h"

#include "Engine/Utils/TransformUtils.h"

namespace MyEngine
{

    void SoftBodyUtils::UpdateSpringConstraint(glm::vec3& positionA, glm::vec3& positionB,
        const float& restLength, const float& springStregth,
        int iterations, bool moveB)
    {
        for (int i = 0; i < iterations; i++)
        {
            glm::vec3 delta = positionB - positionA;
            float deltalength = glm::length(delta);
            if (deltalength <= MIN_FLOAT)
            {
                return;
            }

            float diffA = (deltalength - restLength);
            float diff = diffA / deltalength;

            glm::vec3 deltaPos = delta * 0.5f * diff * springStregth;

            positionA += deltaPos;

            if (moveB)
            {
                positionB -= deltaPos;
            }
        }
    }

    void SoftBodyUtils::ApplyVerlet(glm::vec3& position, glm::vec3& oldPosition,
        const glm::vec3& acceleration, float deltaTime)
    {
        glm::vec3 currentPos = position;
        glm::vec3 oldPos = oldPosition;

        position += (currentPos - oldPos) + (acceleration * static_cast<float>(deltaTime * deltaTime));

        oldPosition = currentPos;
    }

    void SoftBodyUtils::CreateParticlesAndSprings(Entity entityId, 
											      TransformComponent* pTransform, 
											      SoftBodyComponent* pSoftBody)
    {
        if (pSoftBody->vecParticles.size() > 0)
        {
            // Soft body already created
            return;
        }

        iVAOManager* pVAOManager = VAOManagerLocator::Get();

        // Create a copy of the mesh into the VAO
        std::string meshCopy = pSoftBody->meshName + std::to_string(entityId);
        sMesh* pMesh = pVAOManager->LoadModelCopyIntoVAO(pSoftBody->meshName, true, true, false, meshCopy);
        if (!pMesh)
        {
            return;
        }

        size_t sizeVertices = static_cast<size_t>(pMesh->numberOfVertices);

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

        if (pSoftBody->isWireframe)
        {
            // Create "wireframe" springs and orientation springs, to hold the particles to their original location
            for (unsigned int i = 0; i < pSoftBody->vecParticles.size() - 2; i++)
            {
                SoftBodyParticle* particleA = pSoftBody->vecParticles[i];
                SoftBodyParticle* particleB = new SoftBodyParticle();

                particleB->entityId = entityId;
                particleB->position = particleA->position;
                particleB->oldPosition = particleB->position;
                particleB->worldPosition = TransformUtils::LocalToWorldPoint(particleA->position,
                    pTransform->position,
                    pTransform->orientation,
                    pTransform->scale);

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
        }

        // HACK: Set particle to be at the world position initially
        for (unsigned int i = 0; i < pSoftBody->vecParticles.size(); i++)
        {
            SoftBodyParticle* particle = pSoftBody->vecParticles[i];
            particle->position = TransformUtils::LocalToWorldPoint(particle->position,
                pSoftBody->position,
                pSoftBody->orientation,
                pTransform->scale);
            particle->oldPosition = particle->position;
            particle->worldPosition = particle->position;
        }

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
    void SoftBodyUtils::ClearSoftBody(SoftBodyComponent* pSoftBody)
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
