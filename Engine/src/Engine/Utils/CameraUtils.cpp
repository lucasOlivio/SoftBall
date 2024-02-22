#include "pch.h"

#include "CameraUtils.h"

#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/Components.h"

#include "Engine/Utils/TransformUtils.h"
#include "Engine/Utils/Math.h"

namespace MyEngine
{
    glm::vec3 CameraUtils::FinalPosition(glm::vec3 position, glm::quat orientation, 
										 float distance, float height, float offsetTarget)
	{
		glm::vec3 transfUp = TransformUtils::GetUpVector(orientation);
		glm::vec3 transfForward = TransformUtils::GetForwardVector(orientation);

		glm::vec3 offsetDist = -distance * transfForward;
		glm::vec3 offsetHeight = height * transfUp;

		glm::vec3 camFinalPos = position + offsetDist + offsetHeight;

		return camFinalPos;
	}

	glm::mat4 CameraUtils::ViewMat(glm::vec3 position, glm::quat orientation, 
								   float distance, float height, float offsetTarget)
	{
		// Calculating camera view
		glm::vec3 cameraPosition = FinalPosition(position, orientation, 
												 distance, height, offsetTarget);

		glm::vec3 target = position + (TransformUtils::GetForwardVector(orientation) * offsetTarget);

		return glm::lookAt(
					cameraPosition,
					target,
					TransformUtils::GetUpVector(orientation)
				);
	}

	glm::mat4 CameraUtils::ProjectionMat(float fovy, float zNear, float zFar,
										 int width, int height)
	{
		return glm::perspective(fovy,
								Ratio(width, height),
								zNear,
								zFar);
	}

	Entity CameraUtils::GetMainCamera(Scene* pScene)
	{
		return *(SceneView<CameraComponent>(*pScene).begin());
	}
}
