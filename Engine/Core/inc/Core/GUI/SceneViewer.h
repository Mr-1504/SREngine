//
// Created by Monika on 11.02.2022.
//

#ifndef SRENGINE_SCENEVIEWER_H
#define SRENGINE_SCENEVIEWER_H

#include <Utils/ECS/GameObject.h>
#include <Utils/World/Scene.h>

namespace SR_GRAPH_NS::Types {
    class Camera;
}

namespace SR_GRAPH_NS {
    class Window;
}

namespace SR_GRAPH_NS::GUI {
    class Guizmo;
}

namespace SR_CORE_NS::GUI {
    class Hierarchy;

    class SR_DLL_EXPORT SceneViewer : public Graphics::GUI::Widget {
        using GameObjectPtr = SR_UTILS_NS::GameObject::Ptr;
    public:
        explicit SceneViewer(Graphics::Window* window, Hierarchy* hierarchy);
        ~SceneViewer() override;

    public:
        void SetScene(const SR_WORLD_NS::Scene::Ptr& scene);
        void Enable(bool value);
        void Update();

        void OnKeyDown(const SR_UTILS_NS::KeyboardInputData* data) override;
        void OnKeyPress(const SR_UTILS_NS::KeyboardInputData* data) override;

    private:
        void SetCameraActive(bool value);
        void OnClose() override;
        void OnOpen() override;
        void Draw() override;

        void InitCamera();
        void SetCamera(const GameObjectPtr& camera);
        void DrawTexture(Helper::Math::IVector2 winSize, Helper::Math::IVector2 texSize, uint32_t id, bool centralize);
        void DrawImage(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col, bool imposition);

    private:
        Graphics::Window* m_window = nullptr;
        Hierarchy* m_hierarchy = nullptr;
        Graphics::GUI::Guizmo* m_guizmo = nullptr;
        int32_t m_id;
        GameObjectPtr m_camera;
        SR_WORLD_NS::Scene::Ptr m_scene;
        std::atomic<bool> m_enabled = false;
        std::atomic<bool> m_cameraActive = false;
        SR_MATH_NS::FVector3 m_rotation;
        SR_MATH_NS::FVector3 m_translation;
        bool m_updateNonHoveredSceneViewer = false;

    };
}

#endif //SRENGINE_SCENEVIEWER_H
