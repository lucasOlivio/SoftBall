#include "Core/AppEngine.h"

#include "Engine/Core/EngineLocator.h"

int main()
{
	using namespace MyEngine;

	Application* pApp = new Application();
	EngineLocator::Set(pApp);

	// TODO: This would be better comming from command args?
	const std::string INITIAL_SCENE = "scene01.json";

	pApp->Run(INITIAL_SCENE, true);

	delete pApp;
}