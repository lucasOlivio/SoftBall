#pragma once

#include "Engine/ECS/System/iSystem.h"

namespace MyEngine
{
	// Draw all soft body particles as spheres
	class DrawSoftBodyParticlesSystem : public iSystem
	{
	public:
		DrawSoftBodyParticlesSystem() = default;
		virtual ~DrawSoftBodyParticlesSystem() { };

		virtual std::string SystemName() { return "DrawSoftBodyParticlesSystem"; };

		virtual void Init();

		virtual void Start(Scene* pScene);

		virtual void Update(Scene* pScene, float deltaTime);

		virtual void Render(Scene* pScene);

		virtual void End(Scene* pScene);

		virtual void Shutdown();
	};
}
