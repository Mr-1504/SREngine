//
// Created by Monika on 27.01.2024.
//

#include <Core/GUI/EditorCamera.h>
#include <Core/GUI/SceneViewer.h>

namespace SR_CORE_GUI_NS {
    EditorCamera::EditorCamera(SceneViewer* pSceneViewer)
        : Camera()
        , m_sceneViewer(pSceneViewer)
    { }

    bool EditorCamera::ExecuteInEditMode() const {
        return true;
    }

    SR_MATH_NS::FPoint EditorCamera::GetMousePos() const {
        auto&& imMouseGuiPos = ImGui::GetMousePos();
        auto&& mousePos = SR_MATH_NS::FPoint(imMouseGuiPos.x, imMouseGuiPos.y) - m_sceneViewer->GetImagePosition();
        return mousePos / m_sceneViewer->GetTextureSize().Cast<SR_MATH_NS::Unit>();
    }
}
