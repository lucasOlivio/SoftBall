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

namespace MyEngine
{
    const float PLAYER_JUMP_SPEED = 20.0f;

	void PlayerControllerSystem::Init()
	{
        // Subscribe to keyboard event
        iEventBus<eInputEvents, KeyboardEvent>* pEventBus = EventBusLocator<eInputEvents, KeyboardEvent>::Get();

        pEventBus->Subscribe(eInputEvents::KEYBOARD, InputTriggered);

        m_InitiateMouseCapture();
	}

	void PlayerControllerSystem::Start(Scene* pScene)
	{
	}

	void PlayerControllerSystem::Update(Scene* pScene, float deltaTime)
	{
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

            // HACK: Keep above floor
            if(pTransform->position.y < 1.0f)
                pTransform->position.y = 1.0f;
        }
	}

	void PlayerControllerSystem::Render(Scene* pScene)
	{
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
        case eKeyCodes::SPACE:
        {
            if (event.keyData.action != eInputActions::KEY_PRESS)
            {
                return;
            }

            // JUMP
            iSceneManager* pSceneManager = SceneManagerLocator::Get();
            Scene* pScene = pSceneManager->GetCurrentScene();
            Entity playerId = GameplayUtils::GetPlayerId(pScene);

            MovementComponent* pMovement = pScene->Get<MovementComponent>(playerId);
            TransformComponent* pTransform = pScene->Get<TransformComponent>(playerId);

            if (pTransform->position.y <= 1.0f)
            {
                pMovement->velocity.y = PLAYER_JUMP_SPEED;
            }

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
