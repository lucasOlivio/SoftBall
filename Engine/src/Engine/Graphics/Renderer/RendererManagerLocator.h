#pragma once

#include "NullRendererManager.hpp"
#include "iRendererManager.h"

namespace MyEngine
{
	class RendererManagerLocator
	{
	public:
		static iRendererManager* Get();

		static void Set(iRendererManager* rendererManager);

	private:
		static iRendererManager* m_rendererManager;
		static NullRendererManager m_nullService;
	};
}
