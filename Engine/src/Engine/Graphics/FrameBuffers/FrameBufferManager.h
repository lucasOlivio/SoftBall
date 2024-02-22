#pragma once

#include "Engine/Graphics/FrameBuffers/iFrameBufferManager.h"

#include <unordered_map>

namespace MyEngine
{
	class FrameBufferManager : public iFrameBufferManager
	{
	public:
		FrameBufferManager();
		virtual ~FrameBufferManager();

		// Creates the new buffer using opengl and returns true if buffer status complete
		virtual bool CreateFBO(Entity cameraId, int width, int height, uint& newFBOID);

		virtual void DeleteFBO(uint FBOID);

		virtual void DeleteAllFBOs();

		// Deletes then recreate FBO (ID may change)
		virtual bool ResetFBO(uint FBOID, uint& newFBOID);

		// If not found returns default FBO
		virtual FrameBufferObject& GetFBO(uint FBOID);

		// Binds and update the matrix uniform values
		// To unbind just call it with 0 to bind to the main
		virtual void BindFBO(Scene* pScene, uint FBOID);

		virtual void BindFBOText(uint FBOID);

		// Clears color and depth textures for the fbo
		virtual void ClearFBO(uint FBOID);

	private:
		std::unordered_map<uint /* FBOID */, FrameBufferObject> m_mapFBOs;

		uint m_currFBOID;
	};
}
