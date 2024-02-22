#include "pch.h"

#include "RendererManagerLocator.h"

namespace MyEngine
{
	NullRendererManager RendererManagerLocator::m_nullService = NullRendererManager();
	iRendererManager* RendererManagerLocator::m_rendererManager = &(RendererManagerLocator::m_nullService);

	iRendererManager* RendererManagerLocator::Get() { return m_rendererManager; }

	void RendererManagerLocator::Set(iRendererManager* rendererManager)
	{
		if (rendererManager == nullptr)
		{
			m_rendererManager = &m_nullService;
		}
		else
		{
			m_rendererManager = rendererManager;
		}
	}
}
