#include "pch.h"

#include "DrawSoftBodyPartclesSystem.h"

#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/Components.h"
#include "Engine/ECS/SingletonComponents/DebugLocator.h"

#include "Engine/Graphics/VAO/VAOManagerLocator.h"
#include "Engine/Graphics/Renderer/RendererManagerLocator.h"

#include "Engine/Utils/TransformUtils.h"

namespace MyEngine
{
    void DrawSoftBodyPartclesSystem::Init()
    {
    }

    void DrawSoftBodyPartclesSystem::Start(Scene* pScene)
    {
    }

    void DrawSoftBodyPartclesSystem::Update(Scene* pScene, float deltaTime)
    {
    }

    void DrawSoftBodyPartclesSystem::Render(Scene* pScene)
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
                TransformUtils::GetTransform(particle->position, 1.0f, matTransf);

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
        }
    }

    void DrawSoftBodyPartclesSystem::End(Scene* pScene)
    {
    }

    void DrawSoftBodyPartclesSystem::Shutdown()
    {
    }
}
