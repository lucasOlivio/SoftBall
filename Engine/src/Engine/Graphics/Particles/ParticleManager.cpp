#include "pch.h"

#include "Engine/Graphics/Particles/ParticleManager.h"

namespace MyEngine
{
	ParticleManager::ParticleManager()
	{
		m_vecParticles.resize(MAX_PARTICLES);
	}

	ParticleManager::~ParticleManager()
	{
	}

	const std::vector<ParticleProps>& ParticleManager::GetParticles()
	{
		return m_vecParticles;
	}

	void ParticleManager::EmitParticle(const ParticleProps& props)
	{
		ParticleProps& particle = m_vecParticles[m_nextParticle];
		particle = props;

		m_nextParticle = ++m_nextParticle % m_vecParticles.size();
	}

	void ParticleManager::UpdateParticle(uint index, const ParticleProps& props)
	{
		m_vecParticles[index] = props;
	}

	void ParticleManager::ResetParticles()
	{
		for (ParticleProps& particle : m_vecParticles)
		{
			particle.lifetime = 0.0f;
		}

		m_nextParticle = 0;
	}
}