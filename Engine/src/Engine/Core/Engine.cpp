#include "pch.h"

#include "Engine.h"

#include "Engine/Core/Configs/ConfigSerializerFactory.h"

#include "Engine/Events/EventBus.hpp"
#include "Engine/Events/EventBusLocator.hpp"

#include "Engine/ECS/Scene/SceneManager.h"
#include "Engine/ECS/Scene/SceneManagerLocator.h"
#include "Engine/ECS/SingletonComponents/CoreLocator.h"
#include "Engine/ECS/SingletonComponents/DebugLocator.h"
#include "Engine/ECS/SingletonComponents/GraphicsLocator.h"
#include "Engine/ECS/SingletonComponents/PhysicsLocator.h"
#include "Engine/ECS/System/SystemBuilder.h"    

#include "Engine/Graphics/VAO/VAOManagerLocator.h"
#include "Engine/Graphics/VAO/VAOManager.h"
#include "Engine/Graphics/Shaders/ShaderManager.h"
#include "Engine/Graphics/Shaders/ShaderManagerLocator.h"
#include "Engine/Graphics/Materials/MaterialManager.h"
#include "Engine/Graphics/Materials/MaterialManagerLocator.h"
#include "Engine/Graphics/Textures/cBasicTextureManager.h"
#include "Engine/Graphics/Textures/TextureManagerLocator.h"
#include "Engine/Graphics/Renderer/RendererManager.h"
#include "Engine/Graphics/Renderer/RendererManagerLocator.h"
#include "Engine/Graphics/Particles/ParticleManager.h"
#include "Engine/Graphics/Particles/ParticleManagerLocator.h"
#include "Engine/Graphics/FrameBuffers/FrameBufferManager.h"
#include "Engine/Graphics/FrameBuffers/FrameBufferManagerLocator.h"

#include "Engine/Utils/InputUtils.h"

namespace MyEngine
{
    using itSystems = std::map<std::string, iSystem*>::iterator;
    using pairSystems = std::pair<std::string, iSystem*>;

    unsigned __stdcall UpdateThread(void* param) 
    {
        Engine* pEngine = static_cast<Engine*>(param);
        pEngine->Update();

        return 0;
    }

    unsigned __stdcall RenderThread(void* param) 
    {
        Engine* pEngine = static_cast<Engine*>(param);
        pEngine->Render();

        return 0;
    }

    Engine::Engine()
    {
    }

    Engine::~Engine()
    {
    }

    float Engine::GetDeltaTime()
    {
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - m_lastTime;
        m_lastTime = currentTime;

        // Clamp delta time to the maximum frame time
        if (deltaTime > FRAME_DURATION)
        {
            deltaTime = FRAME_DURATION;
        }

        // Add the frame time to the list
        m_frameTimes.push_back(deltaTime);

        // Limit the number of frames
        const size_t maxFrameCount = FRAME_RATE; // Store frame times for a second
        if (m_frameTimes.size() > maxFrameCount)
        {
            m_frameTimes.erase(m_frameTimes.begin());
        }

        // Calculate the average frame time
        float averageFrameTime = 0;
        for (float time : m_frameTimes)
        {
            averageFrameTime += time;
        }
        averageFrameTime /= m_frameTimes.size();

        return averageFrameTime;
    }

    void Engine::AddSystem(std::string systemName, Scene* pScene)
    {
        iSystem* pSystem = GetSystem(systemName);
        if (pSystem)
        {
            return;
        }

        pSystem = SystemBuilder::CreateSystem(systemName);
        if (!pSystem)
        {
            return;
        }

        m_vecSystems.push_back(pSystem);
        pSystem->Init();

        if (pScene)
        {
            pSystem->Start(pScene);
        }
    }

    void Engine::RemoveSystem(std::string systemName, Scene* pScene)
    {
        for (int i = 0; i < m_vecSystems.size(); i++)
        {
            iSystem* pSystem = m_vecSystems[i];
            if (pSystem->SystemName() != systemName)
            {
                continue;
            }

            // Clean system before deleting
            if (pScene)
            {
                pSystem->End(pScene);
            }
            pSystem->Shutdown();

            m_vecSystems.erase(m_vecSystems.begin() + i);

            delete pSystem;

            break;
        }

        return;
    }

    iSystem* Engine::GetSystem(std::string systemName)
    {
        for (int i = 0; i < m_vecSystems.size(); i++)
        {
            iSystem* pSystem = m_vecSystems[i];
            if (pSystem->SystemName() == systemName)
                {
                    return pSystem;
                }
        }

        return nullptr;
    }

    void Engine::Init()
    {
        LoadConfigurations();

        // Setting up events
        m_pEventBusWindow = new EventBus<eWindowEvents, WindowCloseEvent>();
        EventBusLocator<eWindowEvents, WindowCloseEvent>::Set(m_pEventBusWindow);

        m_pEventBusRigidCollision = new EventBus<eCollisionEvents, RigidBodyCollisionEvent>();
        EventBusLocator<eCollisionEvents, RigidBodyCollisionEvent>::Set(m_pEventBusRigidCollision);

        m_pEventBusSoftCollision = new EventBus<eCollisionEvents, SoftBodyCollisionEvent>();
        EventBusLocator<eCollisionEvents, SoftBodyCollisionEvent>::Set(m_pEventBusSoftCollision);

        m_pEventBusKeyboard = new EventBus<eInputEvents, KeyboardEvent>();
        EventBusLocator<eInputEvents, KeyboardEvent>::Set(m_pEventBusKeyboard);

        m_pEventBusMouse = new EventBus<eInputEvents, MouseEvent>();
        EventBusLocator<eInputEvents, MouseEvent>::Set(m_pEventBusMouse);

        m_pEventBusSceneChange = new EventBus<eSceneEvents, SceneChangeEvent>();
        EventBusLocator<eSceneEvents, SceneChangeEvent>::Set(m_pEventBusSceneChange);

        m_pEventBusStartedState = new EventBus<eGameStateEvents, GameStartedEvent>();
        EventBusLocator<eGameStateEvents, GameStartedEvent>::Set(m_pEventBusStartedState);

        m_pEventBusStoppedState = new EventBus<eGameStateEvents, GameStoppedEvent>();
        EventBusLocator<eGameStateEvents, GameStoppedEvent>::Set(m_pEventBusStoppedState);

        m_pEventBusRunningState = new EventBus<eGameStateEvents, GameRunningEvent>();
        EventBusLocator<eGameStateEvents, GameRunningEvent>::Set(m_pEventBusRunningState);

        m_pEventBusGameOverState = new EventBus<eGameStateEvents, GameOverEvent>();
        EventBusLocator<eGameStateEvents, GameOverEvent>::Set(m_pEventBusGameOverState);

        m_pEventBusPosKeyFrame = new EventBus<eAnimationEvents, PositionKeyFrameEvent>();
        EventBusLocator<eAnimationEvents, PositionKeyFrameEvent>::Set(m_pEventBusPosKeyFrame);

        m_pEventBusRotKeyFrame = new EventBus<eAnimationEvents, RotationKeyFrameEvent>();
        EventBusLocator<eAnimationEvents, RotationKeyFrameEvent>::Set(m_pEventBusRotKeyFrame);

        m_pEventBusScaleKeyFrame = new EventBus<eAnimationEvents, ScaleKeyFrameEvent>();
        EventBusLocator<eAnimationEvents, ScaleKeyFrameEvent>::Set(m_pEventBusScaleKeyFrame);

        // Setting up resources managers
        m_pSceneManager = new SceneManager();
        SceneManagerLocator::Set(m_pSceneManager);

        m_pVAOManager = new VAOManager();
        VAOManagerLocator::Set(m_pVAOManager);

        m_pShaderManager = new ShaderManager();
        ShaderManagerLocator::Set(m_pShaderManager);

        m_pMaterialManager = new MaterialManager();
        MaterialManagerLocator::Set(m_pMaterialManager);

        m_pTextureManager = new cBasicTextureManager();
        TextureManagerLocator::Set(m_pTextureManager);

        m_pRendererManager = new RendererManager();
        RendererManagerLocator::Set(m_pRendererManager);

        m_pParticleManager = new ParticleManager();
        ParticleManagerLocator::Set(m_pParticleManager);

        m_pFrameBufferManager = new FrameBufferManager();
        FrameBufferManagerLocator::Set(m_pFrameBufferManager);

        // Global events of engine interest
        m_pEventBusSceneChange->Subscribe(eSceneEvents::CHANGE, [this](const SceneChangeEvent& event) { OnSceneChange(event); });
        m_pEventBusWindow->Subscribe(eWindowEvents::WINDOW_CLOSE, [this](const WindowCloseEvent& event) { OnWindowClose(event); });

        // Load resources
        ConfigPathComponent* pConfigPaths = CoreLocator::GetConfigPath();
        m_pTextureManager->SetBasePath(pConfigPaths->pathTextures);
        m_pShaderManager->SetBasePath(pConfigPaths->pathShaders);
        m_pVAOManager->SetBasePath(pConfigPaths->pathModels);
        m_pSceneManager->SetBasePath(pConfigPaths->pathScenes);
    }

    void Engine::Run(std::string initialSceneName, bool startSimulation)
    {
        // HACK: State system must be first of all systems to trigger the appropiate system changes before other systems
        // make irreversible changes to the scene
        GameStateComponent* pState = CoreLocator::GetGameState();
        pState->mainSystems.insert(pState->mainSystems.begin(), "StateSystem");
        AddSystem("CoreSystem");

        // TODO: Now each resource is been loaded by the systems, 
        // but they should be loaded all here and have separate files
        m_pSceneManager->ChangeScene(initialSceneName);

        // Just need to set current state and the update will trigger the event change
        pState->currState = eGameStates::STARTED;
        if (startSimulation)
        {
            pState->currState = eGameStates::RUNNING;
        }

        m_isRunning = true;

        while (m_isRunning)
        {
            Update();

            Render();
        }
    }

    void Engine::Update()
    {
        float deltaTime = GetDeltaTime();

        for (int i = 0; i < m_vecSystems.size(); i++)
        {
            m_vecSystems[i]->Update(m_pCurrentScene, deltaTime);
        }

        m_ClearFrame();
    }

    void Engine::Render()
    {
        m_BeginFrame();

        for (int i = 0; i < m_vecSystems.size(); i++)
        {
            m_vecSystems[i]->Render(m_pCurrentScene);
        }

        m_EndFrame();
    }

    void Engine::Shutdown()
    {
        for (int i = 0; i < m_vecSystems.size(); i++)
        {
            iSystem* pSystem = m_vecSystems[i];
            pSystem->End(m_pCurrentScene);

            pSystem->Shutdown();

            delete pSystem;
        }

        m_vecSystems.clear();

        // Delete singleton components
        CoreLocator::Clear();
        DebugLocator::Clear();
        GraphicsLocator::Clear();
        PhysicsLocator::Clear();

        // Delete resouce managers
        delete m_pVAOManager;
        delete m_pShaderManager;
        delete m_pMaterialManager;
        delete m_pTextureManager;
        delete m_pRendererManager;
        delete m_pSceneManager;
        delete m_pParticleManager;

        // Delete event bus
        delete m_pEventBusWindow;
        delete m_pEventBusRigidCollision;
        delete m_pEventBusSoftCollision;
        delete m_pEventBusKeyboard;
        delete m_pEventBusMouse;
        delete m_pEventBusSceneChange;
        delete m_pEventBusStartedState;
        delete m_pEventBusRunningState;
        delete m_pEventBusStoppedState;
        delete m_pEventBusGameOverState;
        delete m_pEventBusPosKeyFrame;
        delete m_pEventBusRotKeyFrame;
        delete m_pEventBusScaleKeyFrame;
    }

    void Engine::LoadConfigurations()
    {
        iConfigSerializer* pConfigSerializer = ConfigSerializerFactory::CreateConfigSerializer(DEFAULT_CONFIG);
        ConfigPathComponent* pConfigPath = CoreLocator::GetConfigPath();

        pConfigSerializer->DeserializeConfig(DEFAULT_CONFIG);
    }

    void Engine::StartSystems(Scene* pScene)
    {
        for (int i = 0; i < m_vecSystems.size(); i++)
        {
            iSystem* pSystem = m_vecSystems[i];
            pSystem->Start(pScene);
        }
    }

    void Engine::EndSystems(Scene* pScene)
    {
        for (int i = 0; i < m_vecSystems.size(); i++)
        {
            iSystem* pSystem = m_vecSystems[i];
            pSystem->End(pScene);
        }
    }

    void Engine::ShutdownSystems()
    {
        for (int i = 0; i < m_vecSystems.size(); i++)
        {
            iSystem* pSystem = m_vecSystems[i];
        pSystem->Shutdown();
        }
    }

    void Engine::OnSceneChange(const SceneChangeEvent& event)
    {
        // End old scene
        if (m_pCurrentScene)
        {
            EndSystems(m_pCurrentScene);
        }

        // Start new scene
        m_pCurrentScene = event.pNewScene;
        StartSystems(m_pCurrentScene);
    }

    void Engine::OnWindowClose(const WindowCloseEvent& event)
    {
        m_isRunning = false;
    }

    void Engine::m_ClearFrame()
    {
        iSceneManager* pSceneManager = SceneManagerLocator::Get();
        pSceneManager->ClearDeletedScenes();
        m_pCurrentScene->m_DestroyEntities();
        m_pCurrentScene->m_DestroyComponents();
    }

    void Engine::m_BeginFrame()
    {
        WindowComponent* pWindow = GraphicsLocator::GetWindow();

        if (!pWindow->pGLFWWindow)
        {
            // No window created
            return;
        }

        // Clear frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // ImGui init frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        window_flags |= ImGuiWindowFlags_NoBackground;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        // Start Imgui window context
        ImGui::Begin("ImGuiMainWindow", NULL, window_flags);

        ImGuizmo::BeginFrame();

        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

        ImGuizmo::Enable(true);

        // Set default font size
        ImGui::SetWindowFontScale(TEXT_FONT_SIZE);
    }

    void Engine::m_EndFrame()
    {
        WindowComponent* pWindow = GraphicsLocator::GetWindow();

        if (!pWindow->pGLFWWindow)
        {
            // No window created
            return;
        }

        // ImGui endframe
        ImGui::End();

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)pWindow->width, (float)pWindow->height);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // GLFW endframe
        glfwSwapBuffers(pWindow->pGLFWWindow);
        glfwPollEvents();
    }
}
