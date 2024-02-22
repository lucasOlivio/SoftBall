#pragma once

#include "Engine/Core/types.h"

namespace MyEngine
{
	#define INITIAL_FBO_TEXTURE_ID 50;

	enum eFBFilter
	{
		NO_FILTER = 0,
		BLUR,
		CHROMIC_ABERRATION
	};

	struct FrameBufferObject
	{
		uint ID = 0; 
		uint colorTextureId = 0;
		uint depthTextureId = 0;

		Entity cameraId = 1;

		int width = 1;
		int height = 1;
	};
}
