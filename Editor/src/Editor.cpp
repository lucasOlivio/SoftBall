#include "Core/EditorEngine.h"

#include "Engine/Core/EngineLocator.h"

int main()
{
	using namespace MyEngine;

	Editor* pEditor = new Editor();
	EngineLocator::Set(pEditor);

	// TODO: This would be better comming from command args?
	const std::string INITIAL_SCENE = "scene01.json";

	pEditor->Run(INITIAL_SCENE, false);

	delete pEditor;
}