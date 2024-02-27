#include "pch.h"

#include "PlayerControllerSystem.h"

#include "Engine/ECS/Scene/SceneManagerLocator.h"
#include "Engine/ECS/SingletonComponents/CoreLocator.h"
#include "Engine/ECS/SingletonComponents/GraphicsLocator.h"
#include "Engine/ECS/Scene/SceneView.hpp"
#include "Engine/ECS/Components.h"

#include "Engine/Events/InputEvent.h"
#include "Engine/Events/WindowEvents.h"
#include "Engine/Events/EventBusLocator.hpp"

#include "Engine/Core/InputProperties.h"

#include "Engine/Utils/InputUtils.h"
#include "Engine/Utils/GameplayUtils.h"

#include "Engine/Graphics/VAO/VAOManagerLocator.h"
#include "Engine/Graphics/Renderer/RendererManagerLocator.h"
#include "Engine/Graphics/VAO/VAOManagerLocator.h"

namespace MyEngine
{
    const float PLAYER_JUMP_SPEED = 20.0f;
    const glm::vec3 CHAIN_POSITION = glm::vec3(5000.0f, 10.0f, 4980.0f);
    glm::vec3 clothPos1 = glm::vec3(0.0f);
    glm::vec3 clothPos2 = glm::vec3(0.0f);

	void PlayerControllerSystem::Init()
	{
        // Subscribe to keyboard event
        iEventBus<eInputEvents, KeyboardEvent>* pEventBus = EventBusLocator<eInputEvents, KeyboardEvent>::Get();

        pEventBus->Subscribe(eInputEvents::KEYBOARD, InputTriggered);

        m_InitiateMouseCapture();

        // Create the chains for the environment
        // HACK: THIS SHOULD ALL BE ABSTRACTED LATER, ITS A MESS
        Entity chainid = GameplayUtils::CreateSoftBodyChain(10, 1.0f, 15, 1.0f, CHAIN_POSITION);
        m_chains.push_back(chainid);
	}

	void PlayerControllerSystem::Start(Scene* pScene)
	{
	}

	void PlayerControllerSystem::Update(Scene* pScene, float deltaTime)
	{
        if (clothPos1 == glm::vec3(0.0f))
        {
            // HACK: Get initial cloth particle position
            Entity clothId = 11;
            SoftBodyComponent* pSoftBodyCloth = pScene->Get<SoftBodyComponent>(clothId);
            clothPos1 = pSoftBodyCloth->vecParticles[0]->position;
            clothPos2 = pSoftBodyCloth->vecParticles[100]->position;
        }

        KeyInputComponent* pKey = CoreLocator::GetKeyInput();
        for (Entity playerId : SceneView<PlayerComponent, TransformComponent, MovementComponent>(*pScene))
        {
            PlayerComponent* pPlayer = pScene->Get<PlayerComponent>(playerId);
            TransformComponent* pTransform = pScene->Get<TransformComponent>(playerId);
            MovementComponent* pMovement = pScene->Get<MovementComponent>(playerId);

            glm::vec3 playerRotation = TransformUtils::GetQuatAsDegrees(pTransform->orientation);

            glm::vec3 playerFront = glm::normalize(TransformUtils::GetForwardVector(pTransform->orientation));
            glm::vec3 playerSides = glm::normalize(glm::cross(playerFront, glm::vec3(UP_VECTOR)));

            glm::vec3 newVelocity = glm::vec3(0.0f);

            float verticalMovement = 0.0f;
            float horizontalMovement = 0.0f;

            // Handle key presses for movement
            if (pKey->key[eKeyCodes::W])
            {
                verticalMovement += 1.0f;
            }
            if (pKey->key[eKeyCodes::S])
            {
                verticalMovement -= 1.0f;
            }
            if (pKey->key[eKeyCodes::A])
            {
                horizontalMovement -= 1.0f;
            }
            if (pKey->key[eKeyCodes::D])
            {
                horizontalMovement += 1.0f;
            }

            newVelocity = verticalMovement * (playerFront * pPlayer->speed);
            newVelocity += horizontalMovement * (playerSides * pPlayer->speed);

            pMovement->velocity.x = newVelocity.x;
            pMovement->velocity.z = newVelocity.z;

            // Mouse position for changing forward direction
            MouseInputComponent* pMouse = CoreLocator::GetMouseInput();

            float xoffset = (pMouse->posX - pMouse->lastPosX) * pMouse->sensitivity;

            // Create quaternions for yaw
            glm::quat yawQuat = glm::angleAxis(-xoffset, glm::vec3(0.0f, 1.0f, 0.0f));

            // Combine pitch and yaw quaternions
            glm::quat orientationChange = yawQuat;

            // Apply the combined quaternion to the player's orientation
            pTransform->orientation = glm::normalize(orientationChange * pTransform->orientation);

            pMouse->lastPosX = pMouse->posX;
            pMouse->lastPosY = pMouse->posY;

            // Keep jumping
            if (pTransform->position.y <= 1.1f)
            {
                pMovement->velocity.y = PLAYER_JUMP_SPEED;
            }

            // HACK Hold first chain particle up
            for (Entity chainid : m_chains)
            {
                SoftBodyComponent* pSoftBody = pScene->Get<SoftBodyComponent>(chainid);

                pSoftBody->vecParticles[0]->position = CHAIN_POSITION;
            }

            // HACK: Hold cloth suspended
            Entity clothId = 11;
            SoftBodyComponent* pSoftBodyCloth = pScene->Get<SoftBodyComponent>(clothId);
            pSoftBodyCloth->vecParticles[0]->position = clothPos1;
            pSoftBodyCloth->vecParticles[100]->position = clothPos2;
        }
	}

	void PlayerControllerSystem::Render(Scene* pScene)
	{
        // HACK: Just to render something for the chain nodes, Create a chain system maybe?
        iRendererManager* pRendererManager = RendererManagerLocator::Get();
        iVAOManager* pVAOManager = VAOManagerLocator::Get();

        std::string chainMesh = "sphere.ply";
        std::string chainmaterial = "matground";
        float scale = 0.5f;
        sMesh* pMesh = pVAOManager->LoadModelIntoVAO(chainMesh, false);

        for (Entity chainId : m_chains)
        {
            SoftBodyComponent* pSoftBody = pScene->Get<SoftBodyComponent>(chainId);
            for (uint i = 0; i < pSoftBody->vecParticles.size() - 3; i++)
            {
                // Calculate position based on the middle of the top and down particles holding this node
                SoftBodyParticle* pParticleChainTop = pSoftBody->vecParticles[i];
                SoftBodyParticle* pParticleChainDown = pSoftBody->vecParticles[i + 1];
                const glm::vec3& top = pParticleChainTop->position;
                const glm::vec3& down = pParticleChainDown->position;
                glm::vec3 positionNode = top;
                positionNode.y = (positionNode.y + down.y) / 2.0f;

                glm::mat4 matTransform = glm::mat4(1.0);
                TransformUtils::GetTransform(positionNode,
                                              glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                                              scale,
                                              matTransform);

                sRenderModelInfo renderInfo = sRenderModelInfo();
                renderInfo.materialName = chainmaterial;
                renderInfo.matModel = matTransform;
                renderInfo.VAO_ID = pMesh->VAO_ID;
                renderInfo.numberOfIndices = pMesh->numberOfIndices;
                renderInfo.useColorTexture = true;

                pRendererManager->AddToRender(0, renderInfo);
            }
        }
	}

	void PlayerControllerSystem::End(Scene* pScene)
	{
	}

	void PlayerControllerSystem::Shutdown()
	{
        // Subscribe to keyboard event
        iEventBus<eInputEvents, KeyboardEvent>* pEventBus = EventBusLocator<eInputEvents, KeyboardEvent>::Get();

        pEventBus->Unsubscribe(eInputEvents::KEYBOARD, InputTriggered);

        m_StopMouseCapture();
	}

    void PlayerControllerSystem::InputTriggered(const KeyboardEvent& event)
    {
        switch (event.keyData.key)
        {
        case eKeyCodes::ESCAPE:
        {
            // Trigger window close event
            iEventBus<eWindowEvents, WindowCloseEvent>* pEventBus = EventBusLocator<eWindowEvents, WindowCloseEvent>::Get();

            WindowCloseEvent collEvent = WindowCloseEvent();
            pEventBus->Publish(collEvent);
            break;
        }
        default:
            break;
        }
    }

    void PlayerControllerSystem::m_InitiateMouseCapture()
    {
        // Set initial mouse position to consider mouse on center of window
        MouseInputComponent* pMouse = CoreLocator::GetMouseInput();
        if (pMouse->mouseCaptured)
        {
            return;
        }

        WindowComponent* pWindow = GraphicsLocator::GetWindow();

        pMouse->lastPosX = pMouse->posX;
        pMouse->lastPosY = pMouse->posY;

        InputUtils::CaptureMouse(pWindow->pGLFWWindow);
        pMouse->mouseCaptured = true;
        pMouse->sensitivity = DEFAULT_MOUSE_SENSITIVITY;

        return;
    }

    void PlayerControllerSystem::m_StopMouseCapture()
    {
        MouseInputComponent* pMouse = CoreLocator::GetMouseInput();
        if (!pMouse->mouseCaptured)
        {
            return;
        }
        WindowComponent* pWindow = GraphicsLocator::GetWindow();

        InputUtils::ReleaseMouse(pWindow->pGLFWWindow);
        pMouse->mouseCaptured = false;

        return;
    }
}
