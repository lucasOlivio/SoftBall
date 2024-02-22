#pragma once

#include "Engine/Graphics/FrameBuffers/iFrameBufferManager.h"

#include "Engine/Utils/Log.h"

namespace MyEngine
{
	class NullFrameBufferManager : public iFrameBufferManager
	{
	public:
		NullFrameBufferManager() {};
		virtual ~NullFrameBufferManager() {};

		virtual bool CreateFBO(Entity cameraId, int width, int height, uint& newFBOID)
		{
			LOG_ERROR("FrameBufferManager not initialized!");
			return false;
		}

		virtual void DeleteFBO(uint FBOID)
		{
			LOG_ERROR("FrameBufferManager not initialized!");
		}

		virtual void DeleteAllFBOs()
		{
			LOG_ERROR("FrameBufferManager not initialized!");
		}

		virtual bool ResetFBO(uint FBOID, uint& newFBOID)
		{
			LOG_ERROR("FrameBufferManager not initialized!");
			return false;
		}

		virtual FrameBufferObject& GetFBO(uint FBOID)
		{
			LOG_ERROR("FrameBufferManager not initialized!");
			return FrameBufferObject();
		}

		virtual void BindFBO(Scene* pScene, uint FBOID)
		{
			LOG_ERROR("FrameBufferManager not initialized!");
		}

		virtual void BindFBOText(uint FBOID)
		{
			LOG_ERROR("FrameBufferManager not initialized!");
		}

		virtual void ClearFBO(uint FBOID)
		{
			LOG_ERROR("FrameBufferManager not initialized!");
		}
	};
}
