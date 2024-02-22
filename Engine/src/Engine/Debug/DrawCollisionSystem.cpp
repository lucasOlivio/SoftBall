#include "pch.h"

#include "DrawCollisionSystem.h"

#include "Engine/Graphics/Renderer/RendererManagerLocator.h"

#include "Engine/ECS/SingletonComponents/DebugLocator.h"
#include "Engine/ECS/SingletonComponents/PhysicsLocator.h"
#include "Engine/ECS/SingletonComponents/CoreLocator.h"

#include "Engine/Utils/GraphicsUtils.h"
#include "Engine/Utils/CollisionsUtils.h"

namespace MyEngine
{
	void DrawCollisionSystem::Init()
	{
	}

	void DrawCollisionSystem::Start(Scene* pScene)
	{
	}

	void DrawCollisionSystem::Update(Scene* pScene, float deltaTime)
	{
	}

	void DrawCollisionSystem::Render(Scene* pScene)
	{
		iRendererManager* pRendererManager = RendererManagerLocator::Get();

		DebugSphereComponent* pSphere = DebugLocator::GetSphere();
		sMesh* pMesh = pSphere->pMesh;

		const std::set<sCollisionData>& currFrameColls = CollisionsUtils::CurrentFrameCollisions();
		// Draw a wireframe blue sphere at the contact points
		for (const sCollisionData& coll : currFrameColls)
		{
			glm::mat4 matTransf = glm::mat4(1.0f);
			TransformUtils::GetTransform(coll.contactPoint, 1.0f, matTransf);

			sRenderModelInfo renderInfo = sRenderModelInfo();
			renderInfo.matModel = matTransf;
			renderInfo.VAO_ID = pMesh->VAO_ID;
			renderInfo.numberOfIndices = pMesh->numberOfIndices;
			renderInfo.isWireFrame = true;
			renderInfo.doNotLight = true;
			renderInfo.useDebugColor = true;
			renderInfo.debugColor = BLUE;

			for (uint fboid : pSphere->FBOIDs)
			{
				pRendererManager->AddToRender(fboid, renderInfo);
			}
		}
	}

	void DrawCollisionSystem::End(Scene* pScene)
	{
	}

	void DrawCollisionSystem::Shutdown()
	{
	}
}
