#pragma once

#include "Engine/Graphics/GraphicsProperties.h"

#include <glm/mat4x4.hpp>

namespace MyEngine
{
	class GraphicsUtils
	{
	public:
		// Sets all needed uniforms and draw model using VAO and opengl
		static void DrawModel(const sRenderModelInfo& renderInfo);
	};
}
