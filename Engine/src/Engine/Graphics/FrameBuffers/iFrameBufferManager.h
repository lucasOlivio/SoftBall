#pragma once

#include "Engine/ECS/Scene/Scene.h"

#include "Engine/Graphics/FrameBuffers/FrameBuffersProperties.h"

#include "Engine/Core/types.h"

namespace MyEngine
{
	class iFrameBufferManager
	{
	public:
		// Use these parameters to create the main framebuffer 0
		iFrameBufferManager() {};
		virtual ~iFrameBufferManager() {};

		// Creates the new buffer using opengl and returns true if buffer status complete
		virtual bool CreateFBO(Entity cameraId, int width, int height, uint& newFBOID) = 0;

		virtual void DeleteFBO(uint FBOID) = 0;

		virtual void DeleteAllFBOs() = 0;

		// Deletes then recreate FBO (ID may change)
		virtual bool ResetFBO(uint FBOID, uint& newFBOID) = 0;

		virtual FrameBufferObject& GetFBO(uint FBOID) = 0;

		virtual void BindFBO(Scene* pScene, uint FBOID) = 0;

		virtual void BindFBOText(uint FBOID) = 0;

		// Clears color and depth textures for the fbo
		virtual void ClearFBO(uint FBOID) = 0;
	};
}
