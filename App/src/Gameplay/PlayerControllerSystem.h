#pragma once

#include "Engine/ECS/System/iSystem.h"

#include "Engine/Events/InputEvent.h"

namespace MyEngine
{
	// All player input handling
	class PlayerControllerSystem : public iSystem
	{
	public:
		PlayerControllerSystem() = default;
		virtual ~PlayerControllerSystem() { };

		virtual std::string SystemName() { return "PlayerControllerSystem"; };

		virtual void Init();

		virtual void Start(Scene* pScene);

		virtual void Update(Scene* pScene, float deltaTime);

		virtual void Render(Scene* pScene);

		virtual void End(Scene* pScene);

		virtual void Shutdown();

		static void InputTriggered(const KeyboardEvent& event);

	private:
		void m_InitiateMouseCapture();

		void m_StopMouseCapture();

		// HACK: Set chains ids to hold first particle up
		std::vector<Entity> m_chains;
	};
}
