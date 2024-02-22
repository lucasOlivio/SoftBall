#pragma once

#include "Engine/ECS/Scene/Scene.h"

#include "Core/Constants.h"

namespace MyEngine
{
    void Guizmo(Scene* pScene, Entity entityId);

    void Title(std::string text, float fontSize = DEFAULT_TITLE_SIZE, ImVec4 fontColor = DEFAULT_TITLE_COLOR);

    void Separator();
}
