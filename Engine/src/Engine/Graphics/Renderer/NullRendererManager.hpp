#pragma once

#include "Engine/Graphics/Renderer/iRendererManager.h"

namespace MyEngine
{
	class NullRendererManager : public iRendererManager
	{
	public:
		NullRendererManager() {};
		virtual ~NullRendererManager() {};

		virtual void AddFBO(uint FBOID)
		{
			LOG_ERROR("Renderer manager not initialized!");
		}

		virtual void AddToRender(uint FBOID, const sRenderModelInfo& renderInfo)
		{
			LOG_ERROR("Renderer manager not initialized!");
		}

		virtual void AddToRenderTransparent(uint FBOID, const sRenderModelInfo& renderInfo)
		{
			LOG_ERROR("Renderer manager not initialized!");
		}

		virtual void RenderAllModels(Scene* pScene)
		{
			LOG_ERROR("Renderer manager not initialized!");
		}

		virtual void ClearRender()
		{
			LOG_ERROR("Renderer manager not initialized!");
		}
	};
}
