#pragma once

#include "Engine/ECS/Scene/Scene.h"

#include "Engine/Graphics/GraphicsProperties.h"

namespace MyEngine
{
	class iRendererManager
	{
	public:
		iRendererManager() {};
		virtual ~iRendererManager() {};

		// Maps the fbo id and reserve its vector
		virtual void AddFBO(uint FBOID) = 0;

		// Add model to rendering pipeline into the respective FBO
		virtual void AddToRender(uint FBOID, const sRenderModelInfo& renderInfo) = 0;
		// HACK: Adding transparent models in order relative to distance to camera
		virtual void AddToRenderTransparent(uint FBOID, const sRenderModelInfo& renderInfo) = 0;

		// Render all models mapped into their respective FBOs
		virtual void RenderAllModels(Scene* pScene) = 0;

		// Clear all model mapping from all FBOs
		virtual void ClearRender() = 0;
	};
}
