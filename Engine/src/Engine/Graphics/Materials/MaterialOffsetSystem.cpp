#include "pch.h"

#include "MaterialOffsetSystem.h"

#include "Engine/Graphics/Materials/MaterialManagerLocator.h"
#include "Engine/Graphics/GraphicsProperties.h"

#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/Components.h"

namespace MyEngine
{
	using itMaterials = std::map<std::string, sMaterialInfo>::iterator;

	void MaterialOffsetSystem::Init()
	{
	}

	void MaterialOffsetSystem::Start(Scene* pScene)
	{
	}

	void MaterialOffsetSystem::Update(Scene* pScene, float deltaTime)
	{
		iMaterialManager* pMaterialManager = MaterialManagerLocator::Get();
		std::map<std::string, sMaterialInfo>& materials = pMaterialManager->GetMaterials();

		// Increase offset for all materials
		for (itMaterials it = materials.begin(); it != materials.end(); it++)
		{
			sMaterialInfo& material = it->second;

			// Position offset
			material.currOffset += material.offsetMove * deltaTime;

			float resetValue = 1.0;
			if (material.useCubeTexture)
			{
				resetValue = 2.0;
			}
			// Clamp offset between 0 and 1
			if (material.currOffset.x > 1)
			{
				material.currOffset.x = 0;
			}
			if (material.currOffset.y > 1)
			{
				material.currOffset.y = 0;
			}

			// Height map offset
			if (material.offsetHeightMap == glm::vec3(0))
			{
				continue;
			}

			material.currOffsetHeightMap += material.offsetHeightMap * deltaTime;

			// Clamp offset between 0 and 1
			if (material.currOffsetHeightMap.x > 1)
			{
				material.currOffsetHeightMap.x = 0;
			}
			if (material.currOffsetHeightMap.y > 1)
			{
				material.currOffsetHeightMap.y = 0;
			}
		}
	}

	void MaterialOffsetSystem::Render(Scene* pScene)
	{
	}

	void MaterialOffsetSystem::End(Scene* pScene)
	{
	}

	void MaterialOffsetSystem::Shutdown()
	{
	}
}