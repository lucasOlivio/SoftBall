#pragma once

#include "Engine/ECS/SingletonComponents/Components.h"

namespace MyEngine
{
	class GraphicsLocator
	{
	public:
		static TransparentEntitiesComponent* GetTransparentEntities();
		static WindowComponent* GetWindow();
		static AnimationControllerComponent* GetAnimationController();

		static void SetTransparentEntities(TransparentEntitiesComponent* pTransparentEntities);
		static void SetWindow(WindowComponent* pWindow);
		static void SetAnimationController(AnimationControllerComponent* pAnimationController);

		static void Clear();

	private:
		static TransparentEntitiesComponent* m_pTransparentEntities;
		static WindowComponent* m_pWindow;
		static AnimationControllerComponent* m_pAnimationController;
	};
}
