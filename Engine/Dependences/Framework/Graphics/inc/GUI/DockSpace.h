//
// Created by Nikita on 14.01.2021.
//

#ifndef GAMEENGINE_DOCKSPACE_H
#define GAMEENGINE_DOCKSPACE_H

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>
#include <imgui_internal.h>

namespace Framework::Graphics::GUI {
    class DockSpace{
        DockSpace()  = delete;
        ~DockSpace() = delete;
    public:
        inline static void Begin() {
            const float toolbarSize = 0;

            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos + ImVec2(0, toolbarSize));
            ImGui::SetNextWindowSize(viewport->Size - ImVec2(0, toolbarSize));
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGuiWindowFlags window_flags = 0
                                            | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
                                            | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                                            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                                            | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::Begin("Master DockSpace", nullptr, window_flags);
            ImGuiID dockMain = ImGui::GetID("MyDockspace");

            // Save off menu bar height for later.
            //float menuBarHeight = ImGui::GetCurrentWindow()->MenuBarHeight();

            ImGui::DockSpace(dockMain);
            ImGui::End();
            ImGui::PopStyleVar(3);
        }
    };
}

#endif //GAMEENGINE_DOCKSPACE_H
