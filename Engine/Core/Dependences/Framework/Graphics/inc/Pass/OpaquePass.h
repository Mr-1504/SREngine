//
// Created by Monika on 14.07.2022.
//

#ifndef SRENGINE_OPAQUEPASS_H
#define SRENGINE_OPAQUEPASS_H

#include <Pass/BasePass.h>

namespace SR_GRAPH_NS {
    class OpaquePass : public BasePass {
    public:
        explicit OpaquePass(RenderTechnique* pTechnique);
        ~OpaquePass() override = default;

    public:
        void PreRender() override;
        void Render() override;
        void Update() override;

    };
}

#endif //SRENGINE_OPAQUEPASS_H
