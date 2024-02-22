#pragma once

#include "Event.hpp"
#include "Engine/Physics/PhysicsProperties.h"

namespace MyEngine
{
	enum eCollisionEvents
	{
		RIGIDBODY,
		SOFTBODY
	};

	class RigidBodyCollisionEvent : public Event<eCollisionEvents>
	{
	public:
		RigidBodyCollisionEvent() :
			Event<eCollisionEvents>(eCollisionEvents::RIGIDBODY, "RigidBodyCollision") {};
		virtual ~RigidBodyCollisionEvent() {};

		sCollisionData collisionData;
	};

	class SoftBodyCollisionEvent : public Event<eCollisionEvents>
	{
	public:
		SoftBodyCollisionEvent() :
			Event<eCollisionEvents>(eCollisionEvents::SOFTBODY, "SoftBodyCollision") {};
		virtual ~SoftBodyCollisionEvent() {};

		sCollisionParticleData collisionData;
	};
}
