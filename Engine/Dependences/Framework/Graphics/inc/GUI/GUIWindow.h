//
// Created by Nikita on 14.01.2021.
//

#ifndef GAMEENGINE_GUIWINDOW_H
#define GAMEENGINE_GUIWINDOW_H

#include <Debug.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>

namespace Framework::Graphics::GUI {
    class GUIWindow {
    public:
        GUIWindow() = delete;
        ~GUIWindow() = delete;
    public:
        static inline void Begin(const std::string& winName) noexcept {
            ImGui::Begin(winName.c_str());
        }
        static inline void BeginChild(const std::string& winName) noexcept {
            ImGui::BeginChild(winName.c_str());
        }

        static inline void End() noexcept {
            ImGui::End();
        }
        static inline void EndChild() noexcept {
            ImGui::EndChild();
        }
        static inline glm::vec2 GetWindowSize() {
            ImVec2 size = ImGui::GetWindowSize();
            return {size.x, size.y};
        }
        static inline void DrawImage(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col, bool imposition = false)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems)
                return;

            ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
            if (border_col.w > 0.0f)
                bb.Max += ImVec2(2, 2);

            if (!imposition) {
                ImGui::ItemSize(bb);
                if (!ImGui::ItemAdd(bb, 0))
                    return;
            }

            //user_texture_id = (void*)5;

            if (border_col.w > 0.0f)
            {
                window->DrawList->AddRect(bb.Min, bb.Max, ImGui::GetColorU32(border_col), 0.0f);
                window->DrawList->AddImage(user_texture_id, bb.Min + ImVec2(1, 1), bb.Max - ImVec2(1, 1), uv0, uv1, ImGui::GetColorU32(tint_col));
            }
            else
            {
                window->DrawList->AddImage(user_texture_id, bb.Min, bb.Max, uv0, uv1, ImGui::GetColorU32(tint_col));
            }
        }
        static inline void DrawTexture(glm::vec2 win_size, glm::vec2 img_size, unsigned int tex, const bool centralize = true) {
            const float dx = win_size.x / img_size.x;
            const float dy = win_size.y / img_size.y;

            if (dx > dy)
                img_size *= dy;
            else
                if (dy > dx)
                    img_size *= dx;
                else
                    img_size *= dy;

            // Because I use the texture from OpenGL, I need to invert the V from the UV.

            if (centralize) {
                ImVec2 initialCursorPos = ImGui::GetCursorPos();
                glm::vec2 res = (win_size - img_size) * 0.5f;
                ImVec2 centralizedCursorPos = {res.x,res.y};
                centralizedCursorPos = ImClamp(centralizedCursorPos, initialCursorPos, centralizedCursorPos);
                ImGui::SetCursorPos(centralizedCursorPos);
            }

            //unsigned int* id = new unsigned int(tex);
            //(ImTextureID)static_cast<void*>(&tex)
            //DrawImage((ImTextureID)static_cast<void*>(&tex), ImVec2(img_size.x, img_size.y), ImVec2(0, 1), ImVec2(1, 0), { 1,1,1,1 }, { 0,0,0,0 }, true); // interesting bug
            DrawImage(reinterpret_cast<void*>(tex), ImVec2(img_size.x, img_size.y), ImVec2(0, 1), ImVec2(1, 0), { 1,1,1,1 }, { 0,0,0,0 }, true);
        }
    };
}

#endif //GAMEENGINE_GUIWINDOW_H