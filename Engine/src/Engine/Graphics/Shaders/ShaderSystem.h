#pragma once

#include "Engine/ECS/System/iSystem.h"

namespace MyEngine
{
	class ShaderSystem : public iSystem
	{
	public:
		ShaderSystem() = default;
		virtual ~ShaderSystem() { };

		virtual std::string SystemName() { return "ShaderSystem"; };

		virtual void Init();

		virtual void Start(Scene* pScene);

		virtual void Update(Scene* pScene, float deltaTime);

		virtual void Render(Scene* pScene);

		virtual void End(Scene* pScene);

		virtual void Shutdown();
	};
}