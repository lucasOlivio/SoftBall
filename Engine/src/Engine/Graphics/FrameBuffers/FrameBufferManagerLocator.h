#pragma once

#include "NullFrameBufferManager.hpp"
#include "iFrameBufferManager.h"

namespace MyEngine
{
	class FrameBufferManagerLocator
	{
	public:
		static iFrameBufferManager* Get();

		static void Set(iFrameBufferManager* frameBufferManager);

	private:
		static iFrameBufferManager* m_frameBufferManager;
		static NullFrameBufferManager m_nullService;
	};
}
