//
// Created by Monika on 13.12.2020.
//

#ifndef SRENGINE_ILIGHTCOMPONENT_H
#define SRENGINE_ILIGHTCOMPONENT_H

#include <Graphics/Types/IRenderComponent.h>
#include <Utils/Common/Enumerations.h>

namespace SR_GRAPH_NS {
    class RenderScene;

    SR_ENUM_NS_CLASS_T(ShadowType, uint8_t,
        Soft, Hard
    );

    class ILightComponent : public SR_GTYPES_NS::IRenderComponent {
    public:
        SR_NODISCARD SR_FORCE_INLINE bool ExecuteInEditMode() const override { return true; }
        SR_NODISCARD SR_FORCE_INLINE bool IsCanUpdate() const noexcept override { return false; }

    protected:
        float_t m_intensity = 1.f;
        float_t m_bounceIntensity = 1.f;
        ShadowType m_shadowType = ShadowType::Soft;

    };
}

#endif //SRENGINE_ILIGHTCOMPONENT_H