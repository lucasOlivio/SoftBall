#include "pch.h"

#include "ParticleManagerLocator.h"

namespace MyEngine
{
	NullParticleManager ParticleManagerLocator::m_nullService = NullParticleManager();
	iParticleManager* ParticleManagerLocator::m_particleManager = &(ParticleManagerLocator::m_nullService);

	iParticleManager* ParticleManagerLocator::Get() { return m_particleManager; }

	void ParticleManagerLocator::Set(iParticleManager* particleManager)
	{
		if (particleManager == nullptr)
		{
			m_particleManager = &m_nullService;
		}
		else
		{
			m_particleManager = particleManager;
		}
	}
}
