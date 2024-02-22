#include "pch.h"

#include "ParticleRenderSystem.h"

#include "Engine/Graphics/Renderer/RendererManagerLocator.h"
#include "Engine/Graphics/Particles/ParticleManagerLocator.h"

#include "Engine/Utils/TransformUtils.h"
#include "Engine/Utils/GraphicsUtils.h"

namespace MyEngine
{
    void ParticleRenderSystem::Init()
    {
    }

    void ParticleRenderSystem::Start(Scene* pScene)
    {
    }

    void ParticleRenderSystem::Update(Scene* pScene, float deltaTime)
    {
    }

    void ParticleRenderSystem::Render(Scene* pScene)
    {
        iParticleManager* pParticleManager = ParticleManagerLocator::Get();
        iRendererManager* pRendererManager = RendererManagerLocator::Get();

        const std::vector<ParticleProps>& particles = pParticleManager->GetParticles();

        for (int i = 0; i < particles.size(); i++)
        {
            const ParticleProps& particle = particles[i];

            if (particle.lifetime <= 0.0f || particle.pMesh == nullptr)
            {
                continue;
            }

            glm::mat4 matTransform = glm::mat4(1.0);

            TransformUtils::GetTransform(particle.position,
                                         particle.orientation,
                                         particle.scale,
                                         matTransform);

            sRenderModelInfo renderInfo = sRenderModelInfo();
            renderInfo.materialName = particle.material;
            renderInfo.matModel = matTransform;
            renderInfo.defaultColor = particle.defaultColor;
            renderInfo.VAO_ID = particle.pMesh->VAO_ID;
            renderInfo.numberOfIndices = particle.pMesh->numberOfIndices;
            // This all should come from material
            renderInfo.isWireFrame = false;
            renderInfo.doNotLight = true;
            renderInfo.useDefaultColor = true;
            renderInfo.useColorTexture = false;
            renderInfo.useDebugColor = false;

            for (uint fboid : particle.FBOIDs)
            {
                pRendererManager->AddToRender(fboid, renderInfo);
            }
        }
    }

    void ParticleRenderSystem::End(Scene* pScene)
    {
    }

    void ParticleRenderSystem::Shutdown()
    {
    }
}
