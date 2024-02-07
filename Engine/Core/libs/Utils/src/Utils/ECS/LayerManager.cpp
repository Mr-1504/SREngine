//
// Created by innerviewer on 1/6/2024.
//

#include <Utils/ECS/LayerManager.h>

namespace SR_UTILS_NS {
    SR_UTILS_NS::Path LayerManager::InitializeResourcePath() const {
        return "Engine/Configs/Layers.xml";
    }

    void LayerManager::ClearSettings() {
        SR_LOCK_GUARD;

        m_defaultLayer = StringAtom();
        m_layers.clear();

        Super::ClearSettings();
    }

    bool LayerManager::LoadSettings(const SR_XML_NS::Node& node) {
        SR_LOCK_GUARD;

        m_layers.clear();

        m_defaultLayer = node.TryGetNode("Default").TryGetAttribute("Name").ToString("Default");

        if (auto&& layersNode = node.GetNode("Layers")) {
            for (auto&& layerNode : layersNode.GetNodes()) {
                m_layers.emplace_back(layerNode.Name());
            }
        }

        return Super::LoadSettings(node);
    }

    uint16_t LayerManager::GetLayerIndex(StringAtom layer) const {
        for (uint16_t i = 0; i < m_layers.size(); ++i) {
            if (layer == m_layers[i]) {
                return i;
            }
        }

        return SR_ID_INVALID;
    }

    bool LayerManager::HasLayer(StringAtom layer) const {
        for (uint16_t i = 0; i < m_layers.size(); ++i) {
            if (layer == m_layers[i]) {
                return true;
            }
        }

        return false;
    }
}