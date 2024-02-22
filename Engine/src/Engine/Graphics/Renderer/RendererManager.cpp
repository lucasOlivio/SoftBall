#include "pch.h"

#include "RendererManager.h"

#include "Engine/Utils/GraphicsUtils.h"

#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/Components.h"
#include "Engine/Utils/TransformUtils.h"

#include "Engine/Graphics/Materials/MaterialManagerLocator.h"
#include "Engine/Graphics/FrameBuffers/FrameBufferManagerLocator.h"
#include "Engine/Graphics/Shaders/ShaderManager.h"

namespace MyEngine
{
	using pairFBOInfos = std::pair<uint, std::vector<sRenderModelInfo>>;
	using itFBOInfos = std::map<uint, std::vector<sRenderModelInfo>>::iterator;

	RendererManager::RendererManager()
	{
	}

	void RendererManager::AddFBO(uint FBOID)
	{
		m_mapRenderInfos[FBOID] = {};
		m_mapRenderInfos[FBOID].reserve(MAX_ENTITIES);
	}

	void RendererManager::AddToRender(uint FBOID, const sRenderModelInfo& renderInfo)
	{
		assert(m_mapRenderInfos.find(FBOID) != m_mapRenderInfos.end() && "FBOID not found in map!");

		m_mapRenderInfos[FBOID].push_back(renderInfo);
	}

	void RendererManager::AddToRenderTransparent(uint FBOID, const sRenderModelInfo& renderInfo)
	{
		assert(m_mapRenderInfos.find(FBOID) != m_mapRenderInfos.end() && "FBOID not found in map!");

		m_mapRenderTransparentInfos[FBOID].push_back(renderInfo);

		std::sort(m_mapRenderTransparentInfos[FBOID].begin(), m_mapRenderTransparentInfos[FBOID].end(), SortInfoFromCamera);
	}

	void RendererManager::RenderAllModels(Scene* pScene)
	{
		// Render all normal models
		if(m_mapRenderInfos.size() > 0)
			m_RenderMaps(pScene, true, m_mapRenderInfos);
		// Then render all transparent models
		if (m_mapRenderTransparentInfos.size() > 0)
			m_RenderMaps(pScene, false, m_mapRenderTransparentInfos);
	}

	void RendererManager::ClearRender()
	{
		for (itFBOInfos it = m_mapRenderInfos.begin(); it != m_mapRenderInfos.end(); ++it) 
		{
			it->second.clear();
		}

		for (itFBOInfos it = m_mapRenderTransparentInfos.begin(); it != m_mapRenderTransparentInfos.end(); ++it)
		{
			it->second.clear();
		}
	}

	bool RendererManager::SortInfoFromCamera(const sRenderModelInfo& infoA, const sRenderModelInfo& infoB)
	{
		// Sort from more distant, to closer
		return infoA.distToCamera > infoB.distToCamera;
	}

	void RendererManager::m_RenderList(iMaterialManager* pMaterialManager, 
									   iFrameBufferManager* pFrameBufferManager,
									   iShaderProgram* pShader,
									   const std::vector<sRenderModelInfo>& renderInfos)
	{
		for (const sRenderModelInfo& renderInfo : renderInfos)
		{
			pMaterialManager->BindMaterial(renderInfo.materialName);

			if (renderInfo.isFBOView)
			{
				pShader->SetUniformInt("FBOViewFilter", renderInfo.filter);
				pFrameBufferManager->BindFBOText(renderInfo.FBOViewID);
			}

			GraphicsUtils::DrawModel(renderInfo);

			if (renderInfo.isFBOView)
			{
				pShader->SetUniformInt("FBOViewFilter", 0);
				pShader->SetUniformInt("isFBOView", false);
			}
		}
	}

	void RendererManager::m_RenderMaps(Scene* pScene, bool clearFBOs, std::map<uint, std::vector<sRenderModelInfo>>& mapRenderInfos)
	{
		iMaterialManager* pMaterialManager = MaterialManagerLocator::Get();
		iFrameBufferManager* pFrameBufferManager = FrameBufferManagerLocator::Get();
		iShaderProgram* pShader = ShaderManager::GetActiveShader();

		// Bind the fbo then render all their respective models
		// First element will always be the main screen buffer, so we start from the second
		itFBOInfos it = std::next(mapRenderInfos.begin());

		// Iterate over the map starting from the second element
		for (; it != mapRenderInfos.end(); ++it)
		{
			uint FBOID = it->first;

			pFrameBufferManager->BindFBO(pScene, FBOID);

			if (clearFBOs)
			{
				pFrameBufferManager->ClearFBO(FBOID);
			}

			// Render all models associated with the current FBOID
			m_RenderList(pMaterialManager, pFrameBufferManager,
						 pShader, it->second);
		}

		// Making sure we return to main buffer
		pFrameBufferManager->BindFBO(pScene, 0);

		// Render all models associated with the FBOID 0
		m_RenderList(pMaterialManager, pFrameBufferManager,
					 pShader, mapRenderInfos[0]);
	}
}