#pragma once

#include "Engine/Graphics/Renderer/iRendererManager.h"

#include "Engine/Graphics/Materials/iMaterialManager.h"
#include "Engine/Graphics/FrameBuffers/iFrameBufferManager.h"
#include "Engine/Graphics/Shaders/iShaderProgram.h"

#include <map>
#include <vector>

namespace MyEngine
{
	class RendererManager : public iRendererManager
	{
	public:
		RendererManager();
		virtual ~RendererManager() {};

		// Maps the fbo id and reserve its vector
		virtual void AddFBO(uint FBOID);

		// Add model to rendering pipeline into the respective FBO
		virtual void AddToRender(uint FBOID, const sRenderModelInfo& renderInfo);
		// HACK: Adding transparent models in order relative to distance to camera
		virtual void AddToRenderTransparent(uint FBOID, const sRenderModelInfo& renderInfo);

		// Render all models mapped into their respective FBOs
		virtual void RenderAllModels(Scene* pScene);

		// Clear all model mapping from all FBOs
		virtual void ClearRender();

		// Sort conditional for transparent models vector
		static bool SortInfoFromCamera(const sRenderModelInfo& infoA, const sRenderModelInfo& infoB);

	private:
		std::map<uint /* FBOID */, std::vector<sRenderModelInfo>> m_mapRenderInfos;
		std::map<uint /* FBOID */, std::vector<sRenderModelInfo>> m_mapRenderTransparentInfos;

		void m_RenderList(iMaterialManager* pMaterialManager, 
						  iFrameBufferManager* pFrameBufferManager,
						  iShaderProgram* pShader,
						  const std::vector<sRenderModelInfo>& renderInfos);

		void m_RenderMaps(Scene* pScene, bool clearFBOs, 
						  std::map<uint /* FBOID */, std::vector<sRenderModelInfo>>& mapRenderInfos);
	};
}
