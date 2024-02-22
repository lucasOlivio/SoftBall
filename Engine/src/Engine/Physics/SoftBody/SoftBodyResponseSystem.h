#pragma once

#include "Engine/ECS/System/iSystem.h"

#include "Engine/Events/CollisionEvent.h"

namespace MyEngine
{
	// Deals with the particle collision responsess
	class SoftBodyResponseSystem : public iSystem
	{
	public:
		SoftBodyResponseSystem() = default;
		virtual ~SoftBodyResponseSystem() { };

		virtual std::string SystemName() { return "SoftBodyResponseSystem"; };

		virtual void Init();

		virtual void Start(Scene* pScene);

		virtual void Update(Scene* pScene, float deltaTime);

		virtual void Render(Scene* pScene);

		virtual void End(Scene* pScene);

		virtual void Shutdown();
	};
}
