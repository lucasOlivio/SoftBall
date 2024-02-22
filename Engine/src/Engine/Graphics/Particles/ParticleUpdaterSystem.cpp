#include "pch.h"

#include "ParticleUpdaterSystem.h"

#include "Engine/Graphics/Particles/ParticleManagerLocator.h"

#include "Engine/ECS/Components.h"

#include "Engine/Utils/TransformUtils.h"

namespace MyEngine
{
    void ParticleUpdaterSystem::Init()
    {
    }

    void ParticleUpdaterSystem::Start(Scene* pScene)
    {
    }

    void ParticleUpdaterSystem::Update(Scene* pScene, float deltaTime)
    {
        iParticleManager* pParticleManager = ParticleManagerLocator::Get();

        const std::vector<ParticleProps>& particles = pParticleManager->GetParticles();

        for (int i = 0; i < particles.size(); i++)
        {
            ParticleProps particle = particles[i];

            if (particle.lifetime <= 0.0f)
            {
                continue;
            }

            particle.lifetime -= deltaTime;

            if (particle.lifetime <= 0.0f)
            {
                EmitterComponent* pEmitter = pScene->Get<EmitterComponent>(particle.entityId);
                pEmitter->totalEmitPart--;
            }

            // Euler integration
            particle.velocity = particle.velocity + (particle.acceleration * deltaTime);
            particle.position = particle.position + (particle.velocity * deltaTime);

            particle.orientation = TransformUtils::AdjustOrientation(particle.orientation, glm::vec3(particle.rotationSpeed * deltaTime));

            // Color change over time
            particle.defaultColor = particle.defaultColor + (particle.colorChange * deltaTime);

            pParticleManager->UpdateParticle(i, particle);
        }
    }

    void ParticleUpdaterSystem::Render(Scene* pScene)
    {
    }

    void ParticleUpdaterSystem::End(Scene* pScene)
    {
    }

    void ParticleUpdaterSystem::Shutdown()
    {
    }
}
