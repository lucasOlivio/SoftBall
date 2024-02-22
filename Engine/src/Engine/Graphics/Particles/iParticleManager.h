#pragma once

#include "Engine/Graphics/Particles/ParticlesProperties.h"

namespace MyEngine
{
	class iParticleManager
	{
	public:
		iParticleManager() {};
		virtual ~iParticleManager() {};

		// Returns all the particles alive or not
		virtual const std::vector<ParticleProps>& GetParticles() = 0;

		// Sets the necessary parameters for the next available particle
		virtual void EmitParticle(const ParticleProps& props) = 0;

		// Update the values for the corresponding particle
		virtual void UpdateParticle(uint index, const ParticleProps& props) = 0;

		// Reset all particles life to 0
		virtual void ResetParticles() = 0;
	};
}
