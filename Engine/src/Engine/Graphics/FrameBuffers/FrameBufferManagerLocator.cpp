#include "pch.h"

#include "FrameBufferManagerLocator.h"

namespace MyEngine
{
	NullFrameBufferManager FrameBufferManagerLocator::m_nullService = NullFrameBufferManager();
	iFrameBufferManager* FrameBufferManagerLocator::m_frameBufferManager = &(FrameBufferManagerLocator::m_nullService);

	iFrameBufferManager* FrameBufferManagerLocator::Get() { return m_frameBufferManager; }

	void FrameBufferManagerLocator::Set(iFrameBufferManager* frameBufferManager)
	{
		if (frameBufferManager == nullptr)
		{
			m_frameBufferManager = &m_nullService;
		}
		else
		{
			m_frameBufferManager = frameBufferManager;
		}
	}
}
