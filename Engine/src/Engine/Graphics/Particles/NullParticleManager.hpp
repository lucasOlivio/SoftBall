#pragma once

#include "Engine/Utils/Log.h"
#include "Engine/Graphics/Particles/iParticleManager.h"

namespace MyEngine
{
	class NullParticleManager : public iParticleManager
	{
	public:
		NullParticleManager() {};
		virtual ~NullParticleManager() {};

		// Returns all the particles alive or not
		virtual const std::vector<ParticleProps>& GetParticles()
		{
			LOG_ERROR("Particle manager not initialized!");
			return {};
		}

		// Sets the necessary parameters for the next available particle
		virtual void EmitParticle(const ParticleProps& props)
		{
			LOG_ERROR("Particle manager not initialized!");
		}

		// Update the values for the corresponding particle
		virtual void UpdateParticle(uint index, const ParticleProps& props)
		{
			LOG_ERROR("Particle manager not initialized!");
		}

		// Reset all particles life to 0
		virtual void ResetParticles()
		{
			LOG_ERROR("Particle manager not initialized!");
		}
	};
}
