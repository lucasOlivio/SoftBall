#pragma once

#include "Engine/ECS/Base.h"
#include "Engine/ECS/Scene/Scene.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <vector>

namespace MyEngine
{
	enum eBody
	{
		STATIC,
		ACTIVE,
		PASSIVE
	};

	struct SoftBodyParticle
	{
		SoftBodyParticle()
		{
			position = glm::vec3(0.0f);
			oldPosition = glm::vec3(0.0f);
			worldPosition = glm::vec3(0.0f);

			entityId = -1;
			isWireframe = false;
		}

		glm::vec3 position;
		glm::vec3 oldPosition;
		glm::vec3 worldPosition;

		Entity entityId;

		// HACK: Wireframe particles are just for reference, they shouldn't move from the constraints
		bool isWireframe;
	};

	struct SoftBodySpring
	{
		SoftBodySpring()
		{
			restLength = 0.0f;
			particleA = nullptr;
			particleB = nullptr;
		}

		float restLength;

		SoftBodyParticle* particleA;
		SoftBodyParticle* particleB;
	};

	struct sCollisionData
	{
		Entity entityA;
		Entity entityB;
		Scene* pScene = nullptr;
		glm::vec3 contactPoint;
		glm::vec3 collisionNormalA;
		glm::vec3 collisionNormalB;
		glm::vec3 velocityAtCollisionA;
		glm::vec3 velocityAtCollisionB;

		bool operator==(const sCollisionData& other) const
		{
			return entityA == other.entityA && entityB == other.entityB;
		}

		bool operator<(const sCollisionData& other) const
		{
			return entityB < other.entityB;
		}
	};

	struct sCollisionParticleData
	{
		Entity entityA;
		SoftBodyParticle* pParticle = nullptr;
		Scene* pScene = nullptr;
		glm::vec3 contactPoint;
		glm::vec3 collisionNormalA;
		glm::vec3 collisionNormalB;

		bool operator==(const sCollisionParticleData& other) const
		{
			return entityA == other.entityA && pParticle == other.pParticle;
		}

		bool operator<(const sCollisionParticleData& other) const
		{
			return entityA < other.entityA;
		}
	};
}
