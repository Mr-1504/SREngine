//
// Created by Monika on 06.05.2022.
//

#ifndef SRENGINE_FRAMEBUFFER_H
#define SRENGINE_FRAMEBUFFER_H

#include <Debug.h>
#include <Math/Vector2.h>
#include <ResourceManager/IResource.h>

namespace SR_GRAPH_NS {
    class Shader;
}

namespace SR_GTYPES_NS {
    class Framebuffer : SR_UTILS_NS::IResource {
    public:
        using Ptr = Framebuffer*;
        using Super = SR_UTILS_NS::IResource;

    private:
        Framebuffer();
        ~Framebuffer() override;

    public:
        static Ptr Create(uint32_t images, const SR_MATH_NS::IVector2& size);
        static Ptr Create(uint32_t images, const SR_MATH_NS::IVector2& size, const std::string& shaderPath);

    public:
        bool Bind();
        void Draw();
        void Free();

        bool BeginRender();
        void EndRender();

        void SetSize(const SR_MATH_NS::IVector2& size);
        void SetImagesCount(uint32_t count);

    protected:
        void OnResourceUpdated(IResource* pResource, int32_t deep) override;

    private:
        bool Init();
        bool OnResize();
        bool InitShader(const std::string& path);

    private:
        Shader*              m_shader      = nullptr;

        bool                 m_isInit      = false;
        std::atomic<bool>    m_hasErrors   = false;
        std::atomic<bool>    m_needResize  = false;
        std::atomic<bool>    m_dirtyShader = false;

        std::vector<int32_t> m_colors      = { };
        int32_t              m_depth       = -1;
        int32_t              m_frameBuffer = -1;

        SR_MATH_NS::IVector2 m_size        = {};

    };
}

#endif //SRENGINE_FRAMEBUFFER_H