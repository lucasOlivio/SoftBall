#include "pch.h"

#include "DrawSoftBodyParticlesSystem.h"

#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/Components.h"
#include "Engine/ECS/SingletonComponents/DebugLocator.h"

#include "Engine/Graphics/VAO/VAOManagerLocator.h"
#include "Engine/Graphics/Renderer/RendererManagerLocator.h"

#include "Engine/Utils/TransformUtils.h"

namespace MyEngine
{
    void DrawSoftBodyParticlesSystem::Init()
    {
    }

    void DrawSoftBodyParticlesSystem::Start(Scene* pScene)
    {
    }

    void DrawSoftBodyParticlesSystem::Update(Scene* pScene, float deltaTime)
    {
    }

    void DrawSoftBodyParticlesSystem::Render(Scene* pScene)
    {
        iRendererManager* pRendererManager = RendererManagerLocator::Get();
        iVAOManager* pVAOManager = VAOManagerLocator::Get();
        DebugSphereComponent* pSphere = DebugLocator::GetSphere();

        for (Entity entityId : SceneView<SoftBodyComponent>(*pScene))
        {
            SoftBodyComponent* pSoftBody = pScene->Get<SoftBodyComponent>(entityId);

            for (SoftBodyParticle* particle : pSoftBody->vecParticles)
            {
                glm::mat4 matTransf = glm::mat4(1.0f);
                TransformUtils::GetTransform(particle->position, 0.1f, matTransf);

                sRenderModelInfo renderInfo = sRenderModelInfo();
                renderInfo.matModel = matTransf;
                renderInfo.VAO_ID = pSphere->pMesh->VAO_ID;
                renderInfo.numberOfIndices = pSphere->pMesh->numberOfIndices;
                renderInfo.isWireFrame = true;
                renderInfo.doNotLight = true;
                renderInfo.useDebugColor = true;
                renderInfo.debugColor = BLUE;

                for (uint fboid : pSphere->FBOIDs)
                {
                    pRendererManager->AddToRender(fboid, renderInfo);
                }
            }

            for (SoftBodyParticle* particle : pSoftBody->vecWireframeParticles)
            {
                glm::mat4 matTransf = glm::mat4(1.0f);
                TransformUtils::GetTransform(particle->worldPosition, 0.1f, matTransf);

                sRenderModelInfo renderInfo = sRenderModelInfo();
                renderInfo.matModel = matTransf;
                renderInfo.VAO_ID = pSphere->pMesh->VAO_ID;
                renderInfo.numberOfIndices = pSphere->pMesh->numberOfIndices;
                renderInfo.isWireFrame = true;
                renderInfo.doNotLight = true;
                renderInfo.useDebugColor = true;
                renderInfo.debugColor = GREEN;

                for (uint fboid : pSphere->FBOIDs)
                {
                    pRendererManager->AddToRender(fboid, renderInfo);
                }
            }
        }
    }

    void DrawSoftBodyParticlesSystem::End(Scene* pScene)
    {
    }

    void DrawSoftBodyParticlesSystem::Shutdown()
    {
    }
}
