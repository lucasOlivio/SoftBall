#pragma once

#include "Engine/ECS/System/iSystem.h"

namespace MyEngine
{
	class FrameBufferSystem : public iSystem
	{
	public:
		FrameBufferSystem() = default;
		virtual ~FrameBufferSystem() { };

		virtual std::string SystemName() { return "FrameBufferSystem"; };

		virtual void Init();

		virtual void Start(Scene* pScene);

		virtual void Update(Scene* pScene, float deltaTime);

		virtual void Render(Scene* pScene);

		virtual void End(Scene* pScene);

		virtual void Shutdown();
	};
}
