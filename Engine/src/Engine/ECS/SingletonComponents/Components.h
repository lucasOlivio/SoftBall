#pragma once

#include "Engine/Core/Shapes.hpp"
#include "Engine/Core/StateProperties.h"

#include "Engine/ECS/Base.h"
#include "Engine/ECS/Scene/Scene.h"

#include "Engine/Graphics/opengl.h"
#include "Engine/Graphics/Textures/TextureProperties.h"
#include "Engine/Graphics/GraphicsProperties.h"

#include "Engine/Physics/BroadPhase/GridAABB.h"
#include "Engine/Physics/PhysicsProperties.h"

#include "Engine/Utils/TransformUtils.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <map>

namespace MyEngine
{
	// Singleton components
	struct WindowComponent
	{
		std::string prevName = ""; // Only for window system to change
		std::string name;
		int width = 640;
		int height = 480;

		GLFWwindow* pGLFWWindow;
	};

	struct ConfigPathComponent
	{
		std::string pathModels;
		std::string pathAudio;
		std::string pathShaders;
		std::string pathScripts;
		std::string pathTextures;
		std::string pathScenes;

		// Path to debug models inside models default folder
		std::string pathDebugSquare; 
		std::string pathDebugSphere;
	};

	struct TransparentEntitiesComponent
	{
		std::vector<Entity> entities;
	};

	struct AnimationControllerComponent
	{
		float timeLastKeyFrame = 0.0f;
		float timeFirstKeyFrame = FLT_MAX;

		float speed = 1.0f;
		bool reset = false;
	};

	struct FrameCounterComponent
	{
		int frameCount;
		float fpsTimer;
		float fps;
	};

	struct GridBroadphaseComponent
	{
		// Mapping of AABBs to their indices in the grid
		std::map< uint /*index*/, GridAABB* > mapAABBs;

		// Length per grid box in the grid
		glm::vec3 lengthPerBox;
	};

	// Generic grouping for any narrow phase testing 
	struct NarrowPhaseTestsComponent
	{
		// Groups of shapes and entities to test,
		// the triangles to be tested against the entities will be in the same index
		std::vector<std::vector<sTriangle*>> trianglesToTest;
		std::vector<std::vector<Entity>> passiveEntitiesToTest;
		std::vector<std::vector<Entity>> staticEntitiesToTest;
		std::vector<std::vector<Entity>> activeEntitiesToTest;
		std::vector<std::vector<SoftBodyParticle*>> particlesToTest;
	};

	// All collisions that happened in the last <FRAME_RATE> frames
	struct FrameCollisionComponent
	{
		std::set<sCollisionData> collisions[FRAME_RATE];
		std::set<sCollisionParticleData> particleCollisions[FRAME_RATE];
		size_t currSize;
		size_t currParticlesSize;
	};

	struct KeyInputComponent
	{
		// One for each key code from glfw
		bool key[350];
	};

	struct MouseInputComponent
	{
		// One for each mouse button code from glfw
		bool button[7];

		float posX;
		float posY;

		float lastPosX;
		float lastPosY;

		float sensitivity;

		bool mouseCaptured;
	};

	struct GameStateComponent
	{
		// All the systems that should be running 
		std::vector<std::string> mainSystems; // Always running
		std::map<eGameStates, std::vector<std::string>> mapStateSystems;

		eGameStates prevState; // Only state system should modify this
		eGameStates currState; // Any system can modify this to stop/run simulation
	};

	// Debug objects

	struct DebugSquareComponent
	{
		sMesh* pMesh;

		std::vector<uint> FBOIDs = { 0 };
	};

	struct DebugSphereComponent
	{
		sMesh* pMesh;

		std::vector<uint> FBOIDs = { 0 };
	};
}