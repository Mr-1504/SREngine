//
// Created by Igor on 12/03/2023.
//

#include <Core/GUI/About.h>

#ifdef SR_COMMON_GIT_METADATA
    #include <git.h>
#endif

namespace SR_CORE_GUI_NS {
    About::About()
        : Super("About", SR_MATH_NS::IVector2(400, 300))
    { }

    void About::Draw() {
        if (ImGui::BeginTabBar("About")) {
            ImGui::Text("SpaRcle Engine v0.0.7");

            ImGui::Separator();
            ImGui::Separator();

            ImGui::Text("Author: ");
            ImGui::Text("   Monika0000");
            ImGui::Separator();
            ImGui::Text("Member contributors: ");
            ImGui::Text("   * innerviewer");
            ImGui::Text("   * Drakeme");
            ImGui::Text("   * CaptainSlider");
            ImGui::Text("   * GitHub Copilot");
            ImGui::Separator();
            ImGui::Text("Made thanks to the following libraries: ");
            ImGui::TextWrapped("    glm, GLFW, GLEW, glad, Bullet3, PhysX, Box2D, Dear ImGui, ImGuizmo, tinyobjloader, imgui-node-editor, stbi, json, Assimp");
            ImGui::Separator();
            ImGui::Text("Build Version: 0.0.7");
    #ifdef SR_COMMON_GIT_METADATA
            static std::string metadata;
            if (metadata.empty()) {
                std::time_t timestamp = std::stoll(git_CommitDate());
                std::tm* timeUTC = std::gmtime(&timestamp);

                metadata += "build '" + std::string(git_CommitSHA1()).substr(0, 7) + "' in '" + git_Branch() + "by '" +
                    + git_AuthorName() + "' on\n\t" + std::asctime(timeUTC);
            }

            ImGui::Text(metadata.c_str());
    #endif
            ImGui::Text("Licensed under the MIT License");

            ImGui::EndTabBar();
        }
    }
}