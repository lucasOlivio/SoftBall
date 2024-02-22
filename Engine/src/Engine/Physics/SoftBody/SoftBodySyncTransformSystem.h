#pragma once

#include "Engine/ECS/System/iSystem.h"
#include "Engine/ECS/Components.h"

namespace MyEngine
{
	// Syncronizes the soft body center with the transform position and rotations
	class SoftBodySyncTransformSystem : public iSystem
	{
	public:
		SoftBodySyncTransformSystem() = default;
		virtual ~SoftBodySyncTransformSystem() { };

		virtual std::string SystemName() { return "SoftBodySyncTransformSystem"; };

		virtual void Init();

		virtual void Start(Scene* pScene);

		virtual void Update(Scene* pScene, float deltaTime);

		virtual void Render(Scene* pScene);

		virtual void End(Scene* pScene);

		virtual void Shutdown();
	};
}
