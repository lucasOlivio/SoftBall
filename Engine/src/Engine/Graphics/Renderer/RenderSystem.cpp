#include "pch.h"

#include "RenderSystem.h"

#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/SingletonComponents/GraphicsLocator.h"
#include "Engine/ECS/SingletonComponents/CoreLocator.h"
#include "Engine/ECS/Components.h"

#include "Engine/Graphics/Shaders/ShaderManager.h"
#include "Engine/Graphics/Renderer/RendererManagerLocator.h"
#include "Engine/Graphics/Textures/TextureManagerLocator.h"
#include "Engine/Graphics/Materials/MaterialManagerLocator.h"
#include "Engine/Graphics/VAO/VAOManagerLocator.h"
#include "Engine/Graphics/GraphicsProperties.h"

#include "Engine/Utils/TransformUtils.h"
#include "Engine/Utils/GraphicsUtils.h"

namespace MyEngine
{
	void RenderSystem::Init()
	{
	}

	void RenderSystem::Start(Scene* pScene)
    {
		ConfigPathComponent* pConfigPath = CoreLocator::GetConfigPath();
		iTextureManager* pTextureManager = TextureManagerLocator::Get();
		iMaterialManager* pMaterialManager = MaterialManagerLocator::Get();
		iVAOManager* pVAOManager = VAOManagerLocator::Get();

		// Load textures
		pTextureManager->SetBasePath(pConfigPath->pathTextures);
		for (Entity entityId : SceneView<TextureComponent>(*pScene))
		{
			TextureComponent* pTexture = pScene->Get<TextureComponent>(entityId);
			std::string errorMsg = "";
			bool isLoaded = true;

			if (pTexture->textureType == eTextureType::CUBE)
			{
				isLoaded = pTextureManager->CreateCubeTextureFromBMPFiles(pTexture->fileName,
																		  pTexture->vecTextures[0],
																		  pTexture->vecTextures[1],
																		  pTexture->vecTextures[2],
																		  pTexture->vecTextures[3],
																		  pTexture->vecTextures[4],
																		  pTexture->vecTextures[5],
																		  true,
																		  errorMsg);
				continue;
			}
			else
			{
				isLoaded = pTextureManager->Create2DTextureFromBMPFile(pTexture->fileName, true);
				errorMsg = pTextureManager->GetLastError();
			}

			if (!isLoaded)
			{
				LOG_ERROR(errorMsg);
				continue;
			}
		}

		// Load Models
		pVAOManager->SetBasePath(pConfigPath->pathModels);
		for (Entity entityId : SceneView<ModelComponent>(*pScene))
		{
			ModelComponent* pModel = pScene->Get<ModelComponent>(entityId);
			pModel->pMeshes.resize(pModel->models.size(), nullptr);
			for (int i = 0; i < pModel->models.size(); i++)
			{
				sMesh* pMesh = pVAOManager->LoadModelIntoVAO(pModel->models[i], pModel->isDynamic);
				if (!pMesh)
				{
					continue;
				}
				pModel->pMeshes[i] = pMesh;
			}
		}

		// Load Materials
		for (Entity entityId : SceneView<MaterialComponent>(*pScene))
		{
			MaterialComponent* pMaterial = pScene->Get<MaterialComponent>(entityId);

			sMaterialInfo materialInfo = sMaterialInfo();
			materialInfo.name = pMaterial->name;
			materialInfo.alphaTexture = pMaterial->alphaTexture;
			materialInfo.cubeTexture = pMaterial->cubeTexture;
			materialInfo.discardTexture = pMaterial->discardTexture;
			materialInfo.specularTexture = pMaterial->specularTexture;
			materialInfo.normalTexture = pMaterial->normalTexture;
			materialInfo.heightMapTexture = pMaterial->heightMapTexture;
			materialInfo.colorTextures = pMaterial->colorTextures;
			materialInfo.offset = pMaterial->offset;
			materialInfo.currOffset = pMaterial->currOffset;
			materialInfo.offsetMove = pMaterial->offsetMove;
			materialInfo.currOffsetHeightMap = pMaterial->currOffsetHeightMap;
			materialInfo.offsetHeightMap = pMaterial->offsetHeightMap;
			materialInfo.colorTexturesRatios = pMaterial->colorTexturesRatios;
			materialInfo.heightScale = pMaterial->heightScale;
			materialInfo.useHeightMap = pMaterial->useHeightMap;
			materialInfo.useNormalTexture = pMaterial->useNormalTexture;
			materialInfo.useSpecularTexture = pMaterial->useSpecularTexture;
			materialInfo.useDiscardTexture = pMaterial->useDiscardTexture;
			materialInfo.useCubeTexture = pMaterial->useCubeTexture;
			materialInfo.isEmissive = pMaterial->isEmissive;
			materialInfo.useAlphaTexture = pMaterial->useAlphaTexture;

			pMaterialManager->LoadMaterial(pMaterial->name, materialInfo);
		}
    }

    void RenderSystem::Update(Scene* pScene, float deltaTime)
    {
    }

    void RenderSystem::Render(Scene* pScene)
    {
		iRendererManager* pRenderer = RendererManagerLocator::Get();
		iShaderProgram* pShader = ShaderManager::GetActiveShader();
		WindowComponent* pWindow = GraphicsLocator::GetWindow();

		pRenderer->RenderAllModels(pScene);
		pRenderer->ClearRender();
    }

    void RenderSystem::End(Scene* pScene)
    {
    }

	void RenderSystem::Shutdown()
	{
	}
}
