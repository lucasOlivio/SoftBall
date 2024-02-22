#pragma once

#include "Engine/Graphics/Particles/iParticleManager.h"

namespace MyEngine
{
	class ParticleManager : public iParticleManager
	{
	public:
		ParticleManager();
		virtual ~ParticleManager();

		// Returns all the particles alive or not
		virtual const std::vector<ParticleProps>& GetParticles();

		// Sets the necessary parameters for the next available particle
		virtual void EmitParticle(const ParticleProps& props);

		// Update the values for the corresponding particle
		virtual void UpdateParticle(uint index, const ParticleProps& props);

		// Reset all particles life to 0
		virtual void ResetParticles();

	private:
		std::vector<ParticleProps> m_vecParticles;

		size_t m_nextParticle = 0;
	};
}
