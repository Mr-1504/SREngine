//
// Created by Monika on 17.07.2022.
//

#ifndef SRENGINE_RENDERTECHNIQUE_H
#define SRENGINE_RENDERTECHNIQUE_H

#include <Utils/Settings.h>
#include <Utils/Math/Vector2.h>
#include <Utils/Types/SafePointer.h>

#include <Graphics/Memory/UBOManager.h>
#include <Graphics/Memory/IGraphicsResource.h>

#include <Graphics/Pass/GroupPass.h>
#include <Graphics/Pass/PassQueue.h>

namespace SR_GTYPES_NS {
    class Camera;
}

namespace SR_SRLM_NS {
    static constexpr uint64_t NODE_START_PASS = SR_COMPILE_TIME_CRC32_STR("START_PASS");
    static constexpr uint64_t NODE_END_PASS = SR_COMPILE_TIME_CRC32_STR("END_PASS");
    static constexpr uint64_t NODE_CASCADED_SHADOW_MAP_PASS = SR_COMPILE_TIME_CRC32_STR("CASCADED_SHADOW_MAP_PASS");
    static constexpr uint64_t NODE_FRAME_BUFFER_SETTINGS = SR_COMPILE_TIME_CRC32_STR("FRAME_BUFFER_SETTINGS");
    static constexpr uint64_t NODE_FRAME_BUFFER_PASS = SR_COMPILE_TIME_CRC32_STR("FRAME_BUFFER_PASS");

    static constexpr uint64_t DATA_TYPE_FRAME_BUFFER_SETTINGS = SR_COMPILE_TIME_CRC32_STR("FRAME_BUFFER_SETTINGS");
}

namespace SR_GRAPH_NS {
    class RenderScene;
    class RenderContext;
    class BasePass;

    /** Частично является настройкой, но может иметь множество экземпляров */
    class RenderTechnique : public SR_UTILS_NS::Settings, public Memory::IGraphicsResource {
        using CameraPtr = Types::Camera*;
        using Super = SR_UTILS_NS::Settings;
        using RenderScenePtr = SR_HTYPES_NS::SafePtr<RenderScene>;
        using Context = RenderContext*;
    private:
        RenderTechnique();

    public:
        ~RenderTechnique() override;

    public:
        static RenderTechnique* Load(const SR_UTILS_NS::Path& path);

    public:
        virtual void Prepare();
        virtual bool Overlay();
        virtual bool Render();
        virtual void Update();

        virtual void OnResize(const SR_MATH_NS::UVector2& size);
        virtual void OnSamplesChanged();

        void FreeVideoMemory() override;

        void SetCamera(CameraPtr pCamera);
        void SetRenderScene(const RenderScenePtr& pRScene);

        SR_NODISCARD CameraPtr GetCamera() const noexcept { return m_camera; }
        SR_NODISCARD Context GetContext() const noexcept { return m_context; }
        SR_NODISCARD RenderScenePtr GetRenderScene() const;
        SR_NODISCARD bool IsEmpty() const;
        SR_NODISCARD std::string_view GetName() const;

        SR_NODISCARD BasePass* FindPass(uint64_t hashName) const;
        SR_NODISCARD BasePass* FindPass(const std::string& name) const;

        SR_NODISCARD const PassQueues& GetQueues() const { return m_queues; }

        template<typename T> SR_NODISCARD T* FindPass() const;

    protected:
        bool Build();

        bool Load() override;
        bool Unload() override;

        bool LoadSettings(const SR_XML_NS::Node &node) override;
        void ClearSettings() override;

    private:
        void SetDirty();

    private:
        RenderScenePtr m_renderScene;
        CameraPtr m_camera = nullptr;
        Context m_context = nullptr;
        std::string m_name;
        std::atomic<bool> m_dirty = false;
        std::atomic<bool> m_hasErrors = false;
        Memory::UBOManager& m_uboManager;

        std::vector<BasePass*> m_passes;
        PassQueues m_queues;

    };

    template<typename T> T* RenderTechnique::FindPass() const {
        for (auto&& pPass : m_passes) {
            if (auto&& pFoundPass = dynamic_cast<T*>(pPass)) {
                return pFoundPass;
            }

            if (auto&& pGroupPass = dynamic_cast<GroupPass*>(pPass)) {
                if (auto&& pFoundPass = pGroupPass->FindPass<T>()) {
                    return pFoundPass;
                }
            }
        }

        return nullptr;
    }
}

#endif //SRENGINE_RENDERTECHNIQUE_H
