#pragma once

#include "Engine/ECS/System/iSystem.h"

#include "Engine/Graphics/GraphicsProperties.h"

#include "Engine/Physics/PhysicsProperties.h"

namespace MyEngine
{
	// Updates transforms, normals and position for all the softbody particles
	class SoftBodyUpdateSystem : public iSystem
	{
	public:
		SoftBodyUpdateSystem() = default;
		virtual ~SoftBodyUpdateSystem() { };

		virtual std::string SystemName() { return "SoftBodyUpdateSystem"; };

		virtual void Init();

		virtual void Start(Scene* pScene);

		virtual void Update(Scene* pScene, float deltaTime);

		virtual void Render(Scene* pScene);

		virtual void End(Scene* pScene);

		virtual void Shutdown();

	private:
		// HACK: This should not be here
		void m_UpdateModelNormals(sMesh* pMesh);
	};
}
