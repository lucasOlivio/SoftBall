#pragma once

#include "Engine/ECS/System/iSystem.h"

namespace MyEngine
{
	// TODO: The particles could be entities later
	// Processes the particles attributes every frame
	class ParticleUpdaterSystem : public iSystem
	{
	public:
		ParticleUpdaterSystem() = default;
		virtual ~ParticleUpdaterSystem() { };

		virtual std::string SystemName() { return "ParticleUpdaterSystem"; };

		virtual void Init();

		virtual void Start(Scene* pScene);

		virtual void Update(Scene* pScene, float deltaTime);

		virtual void Render(Scene* pScene);

		virtual void End(Scene* pScene);

		virtual void Shutdown();
	};
}
