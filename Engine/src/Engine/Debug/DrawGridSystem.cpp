#include "pch.h"

#include "DrawGridSystem.h"

#include "Engine/Graphics/Renderer/RendererManagerLocator.h"

#include "Engine/ECS/SingletonComponents/PhysicsLocator.h"
#include "Engine/ECS/SingletonComponents/DebugLocator.h"

#include "Engine/Utils/TransformUtils.h"

namespace MyEngine
{
	void DrawGridSystem::Init()
	{
		// Just a check if the debug square mesh loaded
		DebugSquareComponent* pSquare = DebugLocator::GetSquare();

		sMesh* pMesh = pSquare->pMesh;
		if (!pMesh)
		{
			LOG_WARNING("Debug Square not loaded!");
			return;
		}
	}

	void DrawGridSystem::Start(Scene* pScene)
	{
	}

	void DrawGridSystem::Update(Scene* pScene, float deltaTime)
	{
	}

	void DrawGridSystem::Render(Scene* pScene)
	{
		iRendererManager* pRendererManager = RendererManagerLocator::Get();

		GridBroadphaseComponent* pGrid = PhysicsLocator::GetGridBroadphase();
		DebugSquareComponent* pSquare = DebugLocator::GetSquare();

		sMesh* pMesh = pSquare->pMesh;

		if (!pMesh)
		{
			return;
		}

		// Draw a red wireframe square for every AABB in the grid
		for (std::pair<uint, GridAABB*> pairAABB : pGrid->mapAABBs)
		{
			GridAABB* pAABB = pairAABB.second;

			glm::mat4 matTransf = glm::mat4(1.0f);
			TransformUtils::GetTransform(pAABB->minXYZ, pGrid->lengthPerBox.x, matTransf);

			sRenderModelInfo renderInfo = sRenderModelInfo();
			renderInfo.matModel = matTransf;
			renderInfo.VAO_ID = pMesh->VAO_ID;
			renderInfo.numberOfIndices = pMesh->numberOfIndices;
			renderInfo.isWireFrame = true;
			renderInfo.doNotLight = true;
			renderInfo.useDebugColor = true;
			renderInfo.debugColor = RED;

			for (uint fboid : pSquare->FBOIDs)
			{
				pRendererManager->AddToRender(fboid, renderInfo);
			}
		}
	}

	void DrawGridSystem::End(Scene* pScene)
	{
	}

	void DrawGridSystem::Shutdown()
	{
	}
}
