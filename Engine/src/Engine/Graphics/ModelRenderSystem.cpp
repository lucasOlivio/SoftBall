#include "pch.h"

#include "ModelRenderSystem.h"

#include "Engine/Graphics/Renderer/RendererManagerLocator.h"

#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/Components.h"

#include "Engine/Graphics/VAO/VAOManagerLocator.h"

#include "Engine/Utils/TransformUtils.h"
#include "Engine/Utils/GraphicsUtils.h"

namespace MyEngine
{
    void ModelRenderSystem::Init()
    {
    }

    void ModelRenderSystem::Start(Scene* pScene)
    {
    }

    void ModelRenderSystem::Update(Scene* pScene, float deltaTime)
    {
    }

    void ModelRenderSystem::Render(Scene* pScene)
    {
        iRendererManager* pRendererManager = RendererManagerLocator::Get();
        iVAOManager* pVAOManager = VAOManagerLocator::Get();

        for (Entity entityId : SceneView<TransformComponent, ModelComponent>(*pScene))
        {
            TransformComponent* pTransform = pScene->Get<TransformComponent>(entityId);
            ModelComponent* pModel = pScene->Get<ModelComponent>(entityId);
            FrameBufferViewComponent* pFrameBufferView = pScene->Get<FrameBufferViewComponent>(entityId);
            if (!pModel->isActive)
            {
                continue;
            }

            glm::mat4 matTransform = glm::mat4(1.0);

            TransformUtils::GetTransform(pTransform->worldPosition,
                                         pTransform->worldOrientation,
                                         pTransform->worldScale,
                                         matTransform);

            SoftBodyComponent* pSoftBody = pScene->Get<SoftBodyComponent>(entityId);
            sMesh* pMesh = nullptr;
            if (pSoftBody)
            {
                if (pSoftBody->meshName == pModel->pMeshes[0]->name)
                {
                    std::string meshCopy = pSoftBody->meshName + std::to_string(entityId);
                    pMesh = pVAOManager->FindMeshByModelName(meshCopy);
                }
                else
                {
                    // HACK: When psoft model different is because is just used for the transform
                    matTransform = glm::mat4(1.0);
                    TransformUtils::GetTransform(pSoftBody->position,
                                                 pSoftBody->orientation,
                                                 pTransform->worldScale,
                                                 matTransform);
                }
            }

            if (!pMesh)
            {
                pMesh = pModel->pMeshes[pModel->currMesh];
            }

            if (!pMesh)
            {
                continue;
            }

            sRenderModelInfo renderInfo = sRenderModelInfo();
            renderInfo.materialName = pModel->material;
            renderInfo.matModel = matTransform;
            renderInfo.defaultColor = pModel->defaultColor;
            renderInfo.VAO_ID = pMesh->VAO_ID;
            renderInfo.numberOfIndices = pMesh->numberOfIndices;
            // This all should come from material
            renderInfo.isDoubleSideFace = pModel->isDoubleSideFace;
            renderInfo.isWireFrame = pModel->isWireframe;
            renderInfo.doNotLight = pModel->doNotLight;
            renderInfo.useDefaultColor = pModel->useDefaultColor;
            renderInfo.useColorTexture = pModel->useColorTexture;
            renderInfo.useDebugColor = false;

            if (pFrameBufferView)
            {
                renderInfo.isFBOView = true;
                renderInfo.FBOViewID = pFrameBufferView->FBOID;
                renderInfo.filter = static_cast<int>(pFrameBufferView->filter);
            }

            TilingComponent* pTiling = pScene->Get<TilingComponent>(entityId);
            if (pTiling)
            {
                renderInfo.tileAxis = pTiling->axis;
                renderInfo.tileOffset = pTiling->offset;
            }

            if (pModel->useTransparency)
            {
                for (uint fboid : pModel->FBOIDs)
                {
                    renderInfo.distToCamera = TransformUtils::DistanceToCamera(pScene, fboid, pTransform->worldPosition);
                    pRendererManager->AddToRenderTransparent(fboid, renderInfo);
                }
            }
            else
            {
                for (uint fboid : pModel->FBOIDs)
                {
                    pRendererManager->AddToRender(fboid, renderInfo);
                }
            }
        }
    }

    void ModelRenderSystem::End(Scene* pScene)
    {
    }

    void ModelRenderSystem::Shutdown()
    {
    }
}
