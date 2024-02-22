#include "pch.h"

#include "FrameBufferSystem.h"

#include "Engine/Graphics/FrameBuffers/FrameBufferManagerLocator.h"

#include "Engine/ECS/Components.h"
#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/SingletonComponents/GraphicsLocator.h"

#include "Engine/Utils/CameraUtils.h"

namespace MyEngine
{
	void FrameBufferSystem::Init()
	{
	}

	void FrameBufferSystem::Start(Scene* pScene)
	{
		iFrameBufferManager* pFrameBufferManager = FrameBufferManagerLocator::Get();

		// Load scene buffers
		uint FBOID;
		for (Entity entityId : SceneView<FrameBufferComponent>(*pScene))
		{
			FrameBufferComponent* pFBO = pScene->Get<FrameBufferComponent>(entityId);

			pFrameBufferManager->CreateFBO(pFBO->cameraId, pFBO->width, pFBO->height, FBOID);
		}
	}

	void FrameBufferSystem::Update(Scene* pScene, float deltaTime)
	{
	}

	void FrameBufferSystem::Render(Scene* pScene)
	{
	}

	void FrameBufferSystem::End(Scene* pScene)
	{
		iFrameBufferManager* pFrameBufferManager = FrameBufferManagerLocator::Get();

		pFrameBufferManager->DeleteAllFBOs();
	}

	void FrameBufferSystem::Shutdown()
	{
	}
}
