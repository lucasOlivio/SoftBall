#pragma once

#include "Engine/ECS/System/iSystem.h"
#include "Engine/ECS/Components.h"

namespace MyEngine
{
	// Responsible for creating and satisfying all of the softbody constraints using Verlet's formula
	class SotBodyConstraintsSystem : public iSystem
	{
	public:
		SotBodyConstraintsSystem() = default;
		virtual ~SotBodyConstraintsSystem() { };

		virtual std::string SystemName() { return "SotBodyConstraintsSystem"; };

		virtual void Init();

		virtual void Start(Scene* pScene);

		virtual void Update(Scene* pScene, float deltaTime);

		virtual void Render(Scene* pScene);

		virtual void End(Scene* pScene);

		virtual void Shutdown();
	};
}
