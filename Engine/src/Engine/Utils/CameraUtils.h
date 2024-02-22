#pragma once

#include "Engine/ECS/Scene/Scene.h"

#include <glm/glm.hpp>

namespace MyEngine
{
	class CameraUtils
	{
	public:
		static glm::vec3 FinalPosition(glm::vec3 position, glm::quat orientation, 
							    float distance, float height, float offsetTarget);

		static glm::mat4 ViewMat(glm::vec3 position, glm::quat orientation,
						  float distance, float height, float offsetTarget);

		static glm::mat4 ProjectionMat(float fovy, float zNear, float zFar, 
									   int width, int height);

		// For now just grab the first entity with camera in the scene
		static Entity GetMainCamera(Scene* pScene);
	};
}
