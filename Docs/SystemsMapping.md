# Systems usage mapping

-- CORE (WINDOW)
WindowFrameSystem:
    *COMPONENTS:  
    - FrameCounterComponent (write)

-- GRAPHICS (RENDERING)
FrameBufferSystem:
    *MANAGERS:
    - FrameBufferManager (Write)

-- CORE (TRANSFORM)
TransformParentSystem:
    *COMPONENTS:
    - TransformComponent (write) - TransformComponent from parent too!
    - ParentComponent (read)

-- CORE (WINDOW)
WindowSystem:
    *EVENTS:
    - WindowClose (publish)

    *COMPONENTS:
    - WindowComponent (write)

-- CORE (INPUT)
InputSystem:
    *EVENTS:
    - KeyboardEvent (publish)
    - MouseEvent (publish)

    *COMPONENTS:
    - WindowComponent (read)
    - KeyInputComponent (write)
    - MouseInputComponent (write)

-- GRAPHICS (RENDERING)
BaseUISystem:
    *COMPONENTS:
    - WindowComponent (read)

-- GRAPHICS (RENDERING)
ShaderSystem:
    *MANAGERS:
    - iShaderManager (write)
    *COMPONENTS:
    - ConfigPathComponent (read)

-- GRAPHICS (RENDERING)
ModelRenderSystem:
    *MANAGERS:
    - iRendererManager (write)
    *COMPONENTS:
    - TransformComponent (read)
    - ModelComponent (read)
    - FrameBufferViewComponent (read)

-- GRAPHICS (RENDERING)
RenderSystem:
    *MANAGERS:
    - iTextureManager (write)
    - iMaterialManager (write)
    - iVAOManager (write)
    *COMPONENTS:
    - ConfigPathComponent (read)
    - TextureComponent (read)
    - ModelComponent (write)
    - MaterialComponent (read)

-- GRAPHICS (LIGHTS)
LightSystem:
    *COMPONENTS:
    - LightComponent (write)
    - TransformComponent (read)

-- GRAPHICS (ANIMATION)
AnimationSystem:
    *COMPONENTS:
    - TransformComponent (write)
    - TransformAnimationComponent (write)

-- GAMEPLAY
PlayerControllerSystem:
    *EVENTS:
    - Input event bus (subscribe)
    - Window close event (publish)

    *COMPONENTS:
    - Key input component (read only)
    - Player component (read only)
    - Transform component (write)
    - Movement component (write)
    - Mouse input component (write)
    - Window component (read only)

-- GRAPHICS (ANIMATION)
AnimationPlayerSystem:
    *COMPONENTS:
    - AnimationControllerComponent (write)

-- PHYSICS (MOVEMENT)
MovementSystem:
    *COMPONENTS:
    - TransformComponent (write)
    - MovementComponent (write)

-- PHYSICS (MOVEMENT)
RotationSystem:
    *COMPONENTS:
    - TransformComponent (write)
    - MovementComponent (write)

-- PHYSICS (MOVEMENT)
GravitySystem:
    *COMPONENTS:
    - MovementComponent (write)
    - GravityComponent (write)

-- PHYSICS (COLLISION)
GridBroadPhaseSystem:
    *COMPONENTS:
    - GridBroadphaseComponent (write)
    - NarrowPhaseTestsComponent (write)
    - TransformComponent (read)
    - RigidBodyComponent (read)
    - SoftBodyComponent (read)

-- PHYSICS (COLLISION)
CollisionSystem:
    *UTILS:
    - CollisionsUtils (write)

    *COMPONENTS:
    - NarrowPhaseTestsComponent (read)
    - TransformComponent (read)
    - RigidBodyComponent (read)
    - SphereColliderComponent (read)
    - AABBColliderComponent (read)

-- PHYSICS (MOVEMENT)
SoftBodyUpdateSystem:
    *MANAGERS:
    - iVAOManager (write)
    *COMPONENTS:
    - TransformComponent (write)
    - MovementComponent (read)
    - SoftBodyComponent (write)
    - ModelComponent (write (changing mesh vertex))

-- PHYSICS (MOVEMENT)
SoftBodyResponseSystem:
    - Particles (write)
    *COMPONENTS:
        - RigidBodyComponent (read)
        - TransformComponent (read)

-- PHYSICS (MOVEMENT)
SotBodyConstraintsSystem:
    Particles (write)
    *MANAGERS:
    - iVAOManager (write, and writes sMesh vertex)
    *COMPONENTS:
    - TransformComponent (read)
    - SoftBodyComponent (write)

-- GAMEPLAY (AI)
SteeringBehaviorSystem:
    *COMPONENTS:
    - TransformComponent (write, also read target transform)
    - MovementComponent (write)
    - SteeringBehaviorComponent (read)

-- DEBUG (LOADING)
DebugSystem:
    *MANAGERS:
    - iVAOManager (write)
    *COMPONENTS:
    - ConfigPathComponent (read)
    - DebugSquareComponent (write)
    - DebugSphereComponent (write)

-- DEBUG (RENDERING-COLLISIONS)
DrawGridSystem:
    *MANAGERS:
    - iRendererManager (write)
    *COMPONENTS:
    - GridBroadphaseComponent (read)
    - DebugSquareComponent (read)

-- DEBUG (RENDERING-COLLISIONS)
DrawCollisionSystem:
    *UTILS:
    - CurrentFrameCollisions (read)
    *MANAGERS:
    - iRendererManager (write)
    *COMPONENTS:
    - GridBroadphaseComponent (read)
    - DebugSquareComponent (read)

-- DEBUG (RENDERING-SOFTBODY)
DrawSoftBodyPartclesSystem:
    *MANAGERS:
    - iRendererManager (write)
    - iVAOManager (write)
    *COMPONENTS:
    - DebugSphereComponent (read)
    - SoftBodyComponent (read)
