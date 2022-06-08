//
// Created by Monika on 11.02.2022.
//

#ifndef SRENGINE_HIERARCHY_H
#define SRENGINE_HIERARCHY_H

#include <GUI/Widget.h>
#include <Types/SafePointer.h>
#include <World/Scene.h>
#include <Input/InputEvents.h>
#include <ECS/GameObject.h>

namespace SR_CORE_NS::GUI {
    class Hierarchy : public Graphics::GUI::Widget {
    public:
        Hierarchy();
        ~Hierarchy() override = default;

    public:
        void SetScene(const SR_WORLD_NS::Scene::Ptr& scene);

        void OnKeyDown(const Helper::KeyDownEvent& event) override;
        void OnKeyUp(const Helper::KeyUpEvent& event) override;

        SR_NODISCARD std::set<Helper::GameObject::Ptr> GetSelected() const;

    private:
        void Draw() override;
        void CheckSelected(const Helper::GameObject::Ptr& gm);
        void ContextMenu(const Helper::GameObject::Ptr& gm, uint64_t id);
        void DrawChild(const Helper::GameObject::Ptr& root);
        void DrawMenu();
        void Copy() const;
        void Paste();

    private:
        const ImGuiTreeNodeFlags m_nodeFlagsWithChild = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        const ImGuiTreeNodeFlags m_nodeFlagsWithoutChild = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf;

        mutable std::mutex m_mutex;
        SR_WORLD_NS::Scene::Ptr m_scene;
        std::list<Helper::GameObject::Ptr> m_pointersHolder;
        std::set<Helper::GameObject::Ptr> m_selected;
        std::unordered_set<Helper::GameObject::Ptr> m_tree;

        std::atomic<bool> m_shiftPressed;
        std::atomic<bool> m_isActive;
        std::atomic<bool> m_isPaused;

    };
}

#endif //SRENGINE_HIERARCHY_H
