//
// Created by Monika on 27.11.2022.
//

#include <Physics/2D/Rigidbody2D.h>

namespace SR_PTYPES_NS {
    SR_REGISTER_COMPONENT(Rigidbody2D);

    SR_UTILS_NS::Component* Rigidbody2D::LoadComponent(SR_HTYPES_NS::Marshal& marshal, const SR_HTYPES_NS::DataStorage* dataStorage) {
        return Super::LoadComponent(SR_UTILS_NS::Measurement::Space2D, marshal, dataStorage);
    }

    SR_UTILS_NS::Measurement Rigidbody2D::GetMeasurement() const {
        return SR_UTILS_NS::Measurement::Space2D;
    }
}
