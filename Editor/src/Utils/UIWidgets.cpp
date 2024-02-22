#include "pch.h"

#include "UIWidgets.h"

#include "Engine/ECS/Components.h"
#include "Engine/ECS/SingletonComponents/GraphicsLocator.h"

#include "Engine/Utils/CameraUtils.h"
#include "Engine/Utils/TransformUtils.h"

namespace MyEngine
{
    void Guizmo(Scene* pScene, Entity entityId)
    {
        TransformComponent* pTransform = pScene->Get<TransformComponent>(entityId);
        if (!pTransform)
        {
            return;
        }

        Entity cameraId = CameraUtils::GetMainCamera(pScene);

        WindowComponent* pWindow = GraphicsLocator::GetWindow();
        CameraComponent* pCamera = pScene->Get<CameraComponent>(cameraId);
        TransformComponent* pCameraTransform = pScene->Get<TransformComponent>(cameraId);

        glm::mat4 viewMatrix = CameraUtils::ViewMat(pCameraTransform->position, pCameraTransform->orientation,
                                                 pCamera->distance, pCamera->height, pCamera->offsetTarget);
        glm::mat4 projectionMatrix = CameraUtils::ProjectionMat(pCamera->fovy, pCamera->zNear, pCamera->zFar,
                                                                pWindow->width, pWindow->height);

        // Set the ImGuizmo transformation matrix
        ImGuizmo::SetID(0); // Use 0 as the ID for a single gizmo
        ImGuizmo::SetDrawlist(); // Setup draw list

        // Specify the operation (translate, rotate, scale)
        ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;

        // Get the model matrix from the transform component
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        TransformUtils::GetTransform(pTransform->worldPosition,
                                    pTransform->worldScale, modelMatrix);

        // Draw the ImGuizmo manipulator
        ImGuizmo::DrawGrid(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix), 
                           glm::value_ptr(modelMatrix), 1.0f);
        ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix),
				            operation, ImGuizmo::LOCAL, glm::value_ptr(modelMatrix),
				            nullptr, nullptr);
    }

    void Title(std::string text, float fontSize, ImVec4 fontColor)
    {
        float oldSize = ImGui::GetFont()->Scale;
        ImGui::GetFont()->Scale *= fontSize;
        ImGui::PushFont(ImGui::GetFont());

        ImGui::TextColored(fontColor, text.c_str());

        // Restore the original font size
        ImGui::GetFont()->Scale = oldSize;
        ImGui::PopFont(); 
    }

    void Separator()
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        draw_list->AddLine(ImVec2(p.x - 9999, p.y), ImVec2(p.x + 9999, p.y), ImGui::GetColorU32(ImGuiCol_Border));
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
    }
}
