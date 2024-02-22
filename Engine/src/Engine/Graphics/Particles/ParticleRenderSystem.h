#pragma once

#include "Engine/ECS/System/iSystem.h"

namespace MyEngine
{
	// Processes the particles attributes every frame
	class ParticleRenderSystem : public iSystem
	{
	public:
		ParticleRenderSystem() = default;
		virtual ~ParticleRenderSystem() { };

		virtual std::string SystemName() { return "ParticleRenderSystem"; };

		virtual void Init();

		virtual void Start(Scene* pScene);

		virtual void Update(Scene* pScene, float deltaTime);

		virtual void Render(Scene* pScene);

		virtual void End(Scene* pScene);

		virtual void Shutdown();
	};
}
