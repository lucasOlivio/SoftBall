#include "pch.h"

#include "SoftBodyUpdateSystem.h"

#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/Components.h"
#include "Engine/ECS/SingletonComponents/CoreLocator.h"

#include "Engine/Graphics/VAO/VAOManagerLocator.h"

#include "Engine/Utils/TransformUtils.h"
#include "Engine/Utils/SoftBodyUtils.h"

namespace MyEngine
{
    void SoftBodyUpdateSystem::Init()
    {
    }

    void SoftBodyUpdateSystem::Start(Scene* pScene)
    {
        return;
    }

    void SoftBodyUpdateSystem::Update(Scene* pScene, float deltaTime)
    {
        deltaTime = deltaTime / 5.0f;

        iVAOManager* pVAOManager = VAOManagerLocator::Get();
        // Update velocity and position
        for (Entity entityId : SceneView<TransformComponent, MovementComponent, SoftBodyComponent>(*pScene))
        {
            TransformComponent* pTransform = pScene->Get<TransformComponent>(entityId);
            MovementComponent* pMovement = pScene->Get<MovementComponent>(entityId);
            SoftBodyComponent* pSoftBody = pScene->Get<SoftBodyComponent>(entityId);

            size_t vecSize = pSoftBody->vecParticles.size();
            if (vecSize == 0)
            {
                continue;
            }

            std::string meshCopy = pSoftBody->meshName + std::to_string(entityId);
            sMesh* pMesh = pVAOManager->FindMeshByModelName(meshCopy);

            // HACK: The -2 is to avoid the "reference" particles for forward and up
            for (size_t i = 0; i < vecSize; i++)
            {
                SoftBodyParticle* pParticle = pSoftBody->vecParticles[i];
                SoftBodyUtils::ApplyVerlet(pParticle->position, pParticle->oldPosition,
                            pMovement->acceleration, deltaTime);

                // Update vertex positions, normals and transform based on particles
                if (pMesh && i < vecSize - 2)
                {
                    sVertex& vertex = pMesh->pVertices[i];
                    glm::vec3 localPosition = TransformUtils::WorldToLocalPoint(pParticle->position,
                                                                                pTransform->worldPosition, 
                                                                                pTransform->worldOrientation, 
                                                                                pTransform->worldScale);
                    vertex.x = localPosition.x;
                    vertex.y = localPosition.y;
                    vertex.z = localPosition.z;
                }

                // HACK: Just to test it bouncing, later it should go to a collision system
                if (pParticle->position.y < 0.0f)
                {
                    pParticle->position.y = 0.0f;
                }
            }

            if (pMesh)
            {
                m_UpdateModelNormals(pMesh);
                pVAOManager->UpdateVAOBuffers(pMesh->name, pMesh);
            }
        }
    }

    void SoftBodyUpdateSystem::Render(Scene* pScene)
    {
    }

    void SoftBodyUpdateSystem::End(Scene* pScene)
    {
    }

    void SoftBodyUpdateSystem::Shutdown()
    {
    }

    void SoftBodyUpdateSystem::m_UpdateModelNormals(sMesh* pMesh)
    {
        for (unsigned int vertIndex = 0; vertIndex != pMesh->numberOfVertices; vertIndex++)
        {
            pMesh->pVertices[vertIndex].nx = 0.0f;
            pMesh->pVertices[vertIndex].ny = 0.0f;
            pMesh->pVertices[vertIndex].nz = 0.0f;
            pMesh->pVertices[vertIndex].nw = 0.0f;
        }

        for (unsigned int i = 0; i < pMesh->numberOfIndices; i += 3)
        {
            unsigned int vertAIndex = pMesh->pIndices[i + 0];
            unsigned int vertBIndex = pMesh->pIndices[i + 1];
            unsigned int vertCIndex = pMesh->pIndices[i + 2];

            sVertex& vertexA = pMesh->pVertices[vertAIndex];
            sVertex& vertexB = pMesh->pVertices[vertBIndex];
            sVertex& vertexC = pMesh->pVertices[vertCIndex];

            glm::vec3 vertA = glm::vec3(vertexA.x, vertexA.y, vertexA.z);
            glm::vec3 vertB = glm::vec3(vertexB.x, vertexB.y, vertexB.z);
            glm::vec3 vertC = glm::vec3(vertexC.x, vertexC.y, vertexC.z);

            glm::vec3 triangleEdgeAtoB = vertB - vertA;
            glm::vec3 triangleEdgeAtoC = vertC - vertA;

            glm::vec3 normal = glm::cross(triangleEdgeAtoB, triangleEdgeAtoC);
            normal = glm::normalize(normal);

            // Add (accumulate) this normal to the three vertices
            vertexA.nx += normal.x;
            vertexA.ny += normal.y;
            vertexA.nz += normal.z;

            vertexB.nx += normal.x;
            vertexB.ny += normal.y;
            vertexB.nz += normal.z;

            vertexC.nx += normal.x;
            vertexC.ny += normal.y;
            vertexC.nz += normal.z;
        }

        // Now normalize the accumulated normals
        for (unsigned int vertIndex = 0; vertIndex != pMesh->numberOfVertices; vertIndex++)
        {
            glm::vec3 normal = glm::vec3(pMesh->pVertices[vertIndex].nx,
                                         pMesh->pVertices[vertIndex].ny,
                                         pMesh->pVertices[vertIndex].nz);
            normal = glm::normalize(normal);

            pMesh->pVertices[vertIndex].nx = normal.x;
            pMesh->pVertices[vertIndex].ny = normal.y;
            pMesh->pVertices[vertIndex].nz = normal.z;
        }
    }
}
