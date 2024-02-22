#include "pch.h"

#include "ParticleEmissionSystem.h"

#include "Engine/Graphics/Particles/ParticleManagerLocator.h"
#include "Engine/Graphics/VAO/VAOManagerLocator.h"

#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/Components.h"

#include "Engine/Utils/Random.h"

namespace MyEngine
{
    void ParticleEmissionSystem::Init()
    {
    }

    void ParticleEmissionSystem::Start(Scene* pScene)
    {
        // Load all particles models
        iVAOManager* pVAOManager = VAOManagerLocator::Get();
        for (Entity entityId : SceneView<EmitterComponent>(*pScene))
        {
            EmitterComponent* pEmitter = pScene->Get<EmitterComponent>(entityId);

            pEmitter->pMesh = pVAOManager->LoadModelIntoVAO(pEmitter->properties.meshName,
                                                             false);
        }

        // Kill all particles
        iParticleManager* pParticleManager = ParticleManagerLocator::Get();

        pParticleManager->ResetParticles();

        // Reset all emitters count
        for (Entity entityId : SceneView<EmitterComponent>(*pScene))
        {
            EmitterComponent* pEmitter = pScene->Get<EmitterComponent>(entityId);

            pEmitter->totalEmitPart = 0;
        }
    }

    void ParticleEmissionSystem::Update(Scene* pScene, float deltaTime)
    {
        iParticleManager* pParticleManager = ParticleManagerLocator::Get();

        float timeScale = (deltaTime / 60.0f);

        for (Entity entityId : SceneView<TransformComponent, EmitterComponent>(*pScene))
        {
            TransformComponent* pTransform = pScene->Get<TransformComponent>(entityId);
            EmitterComponent* pEmitter = pScene->Get<EmitterComponent>(entityId);

            if (!pEmitter->isActive)
            {
                continue;
            }

            pEmitter->timer += deltaTime;
            pEmitter->timeLastEmit += deltaTime;

            // Get generate one particle every x time
            uint32_t seed = (uint32_t)(entityId + pEmitter->totalEmitPart + (deltaTime * 1000));
            int randomRate = Random::Int(seed, pEmitter->emitRateMin, pEmitter->emitRateMax);
            float oneParticleTimer = 1.0f / randomRate;
            if (pEmitter->timeLastEmit < oneParticleTimer)
            {
                // Not time to create one particle yet
                continue;
            }

            int particlesToCreate = static_cast<int>(pEmitter->timeLastEmit * randomRate);

            if (pEmitter->totalEmitPart > pEmitter->maxParticles)
            {
                continue;
            }

            if (pEmitter->totalEmitPart + particlesToCreate > pEmitter->maxParticles)
            {
                particlesToCreate = pEmitter->maxParticles - pEmitter->totalEmitPart;
            }

            // Emit particle with random properties
            EmitterProps emitterProps = pEmitter->properties;
            for (int i = 0; i < particlesToCreate; i++)
            {
                ParticleProps particle;

                particle.entityId = entityId;

                particle.FBOIDs = emitterProps.FBOIDs;

                particle.lifetime = Random::Float(seed, emitterProps.minLifeTime, emitterProps.maxLifeTime);

                particle.acceleration = emitterProps.constForce;
                particle.velocity = Random::Vec3(seed, emitterProps.velMin, emitterProps.velMax);
                particle.rotationSpeed = Random::Vec3(seed, emitterProps.rotMin, emitterProps.rotMax);
                
                particle.orientation = Random::Quat(seed, emitterProps.oriMin, emitterProps.oriMax);
                particle.position = pTransform->position + Random::Vec3(seed, emitterProps.posMin, emitterProps.posMax);
                particle.scale = Random::Float(seed, emitterProps.scaMin, emitterProps.scaMax);
               
                particle.pMesh = pEmitter->pMesh;
                particle.material = emitterProps.material;
                particle.defaultColor = emitterProps.colorInitial;
                particle.colorChange = emitterProps.colorChange;

                pParticleManager->EmitParticle(particle);
            }

            pEmitter->totalEmitPart += particlesToCreate;
            pEmitter->timeLastEmit = 0.0f;
        }
    }

    void ParticleEmissionSystem::Render(Scene* pScene)
    {
    }

    void ParticleEmissionSystem::End(Scene* pScene)
    {
        // Kill all particles
        iParticleManager* pParticleManager = ParticleManagerLocator::Get();

        pParticleManager->ResetParticles();
    }

    void ParticleEmissionSystem::Shutdown()
    {
    }
}
