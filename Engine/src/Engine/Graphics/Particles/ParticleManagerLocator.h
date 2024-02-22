#pragma once

#include "NullParticleManager.hpp"
#include "iParticleManager.h"

namespace MyEngine
{
	class ParticleManagerLocator
	{
	public:
		static iParticleManager* Get();

		static void Set(iParticleManager* particleManager);

	private:
		static iParticleManager* m_particleManager;
		static NullParticleManager m_nullService;
	};
}
