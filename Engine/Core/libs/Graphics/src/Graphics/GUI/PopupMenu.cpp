//
// Created by Monika on 14.08.2023.
//

#include <Graphics/GUI/PopupMenu.h>

namespace SR_GRAPH_GUI_NS {
    void MenuItemSubWidget::Draw(const DrawPopupContext& context) {
        if (m_name.empty()) {
            return;
        }

        if (!m_menuItems.empty()) {
            if (ImGui::BeginMenu(m_name.c_str())) {
                for (uint16_t i = 0; i < m_menuItems.size(); ++i) {
                    m_menuItems[i]->Draw(context);
                    if (i + 1 < m_menuItems.size()) {
                        ImGui::Separator();
                    }
                }
                ImGui::EndMenu();
            }
        }
        else if (ImGui::MenuItem(m_name.c_str())) {
            if (m_action) {
                m_action(context);
            }
        }
    }

    void PopupItemSubWidget::Draw(const DrawPopupContext& context) {
        if (m_name.empty()) {
            return;
        }

        if (ax::NodeEditor::ShowBackgroundContextMenu()) {
            ImGui::OpenPopup(m_name.c_str());
        }

        if (ImGui::BeginPopup(m_name.c_str())) {
            for (uint16_t i = 0; i < m_menuItems.size(); ++i) {
                m_menuItems[i]->Draw(context);
                if (i + 1 < m_menuItems.size()) {
                    ImGui::Separator();
                }
            }
            ImGui::EndPopup();
        }
    }

    MenuItemSubWidget& MenuItemSubWidget::AddMenu(std::string name) {
        auto&& pMenu = new MenuItemSubWidget(std::move(name));
        return *m_menuItems.emplace_back(pMenu);
    }

    MenuItemSubWidget& PopupItemSubWidget::AddMenu(std::string name) {
        auto&& pMenu = new MenuItemSubWidget(std::move(name));
        return *m_menuItems.emplace_back(pMenu);
    }

    MenuItemSubWidget& MenuItemSubWidget::SetAction(MenuItemSubWidget::Action action) {
        m_action = action;
        return *this;
    }
}