//
// Created by Nikita on 17.11.2020.
//

#ifndef GAMEENGINE_RENDER_H
#define GAMEENGINE_RENDER_H

#include <Render/Shader.h>
#include <Types/Mesh.h>
#include <vector>
#include <mutex>
#include <queue>
#include <Environment/Environment.h>
#include <Types/EditorGrid.h>
#include <Render/ColorBuffer.h>
#include <Types/List.h>
#include <iostream>

#include <Render/MeshCluster.h>

#include <Types/Geometry/IndexedMesh.h>

namespace Framework::Graphics::Types {
    class Skybox;
}

namespace Framework::Graphics {
    // TODO: to refactoring

    // first - current, second - new
    struct RenderSkybox {
        Types::Skybox* m_current;
        Types::Skybox* m_new;
    };

    using namespace Framework::Graphics::Types;

    class Light;
    class Window;
    class Camera;

    class Render {
    protected:
        Render();
    public:
        Render(const Render&) = delete;
    protected:
        volatile bool                 m_isCreate                 = false;
        volatile bool                 m_isInit                   = false;
        volatile bool                 m_isRun                    = false;
        volatile bool                 m_isClose                  = false;
    protected:
        Environment*                  m_env                      = nullptr;

        Window*                       m_window                   = nullptr;
        Camera*                       m_currentCamera            = nullptr;
        mutable std::mutex            m_mutex                    = std::mutex();

        // TO_REFACTORING
        std::vector<Types::Mesh*>     m_newMeshes                = std::vector<Mesh*>();
        std::queue<Types::Mesh*>      m_removeMeshes             = std::queue<Mesh*>();
        std::vector<Types::Texture*>  m_texturesToFree           = std::vector<Types::Texture*>();
        std::vector<Skybox*>          m_skyboxesToFreeVidMem     = std::vector<Skybox*>();
        std::unordered_set<Texture*>  m_textures                 = std::unordered_set<Texture*>();

        MeshCluster                   m_geometry                 = { };
        MeshCluster                   m_transparentGeometry      = { };

        RenderSkybox                  m_skybox                   = { nullptr, nullptr };

        std::vector<Shader*>          m_shaders                  = {};

        ColorBuffer*                  m_colorBuffer              = nullptr;
        EditorGrid*                   m_grid                     = nullptr;

        bool                          m_gridEnabled              = false;
        bool                          m_skyboxEnabled            = true;
        bool                          m_wireFrame                = false;

        const PipeLine                m_pipeLine                 = PipeLine::Unknown;
    public:
        static Render* Allocate();
    public:
        [[nodiscard]] SR_FORCE_INLINE bool IsRun() const noexcept { return m_isRun; }
        [[nodiscard]] SR_FORCE_INLINE bool IsInit() const noexcept { return m_isInit; }
    public:
        [[nodiscard]] SR_FORCE_INLINE ColorBuffer* GetColorBuffer() const noexcept { return this->m_colorBuffer; }

        SR_FORCE_INLINE void SetWireFrameEnabled(const bool& value) noexcept { this->m_wireFrame = value; }
        [[nodiscard]] SR_FORCE_INLINE bool GetWireFrameEnabled() const noexcept { return this->m_wireFrame; }
        SR_FORCE_INLINE void SetGridEnabled(bool value) { this->m_gridEnabled = value; }
        void SetCurrentCamera(Camera* camera) noexcept;
        [[nodiscard]] SR_FORCE_INLINE Camera* GetCurrentCamera() const noexcept { return this->m_currentCamera; }
    public:
        void Synchronize();
        bool IsClean();
        void RemoveMesh(Types::Mesh* mesh);
        void RegisterMesh(Types::Mesh* mesh);
        void RegisterMeshes(const Helper::Types::List<Types::Mesh*>& meshes) {
            for (size_t i = 0; i < meshes.Size(); i++)
                this->RegisterMesh(meshes[i]);
        }

        bool InsertShader(uint32_t id, Shader* shader);
        Shader* FindShader(uint32_t id) const;

        /** \brief Can get a nullptr value for removing skybox */
        void SetSkybox(Skybox* skybox);
        bool FreeSkyboxMemory(Skybox* skybox);
        [[nodiscard]] Skybox* GetSkybox() const { return m_skybox.m_current; }

        void RegisterTexture(Types::Texture* texture);
        void FreeTexture(Types::Texture* texture);
    public:
        [[nodiscard]] inline Window* GetWindow() const noexcept { return m_window; }
    public:
        bool Create(Window* window); //, Camera* camera
        /** \warning call only from window thread */
        bool Init();
        /** \warning call only from window thread */
        bool Run();
        /** \warning call only from window thread */
        bool Close();
    public:
        /** \brief Check all render events. For example: new meshes, remove old meshes */
        void PollEvents();
    public:
        virtual void UpdateUBOs() { }

        virtual void CalculateAll() = 0;
        virtual void DrawGeometry() = 0;
        virtual void DrawSkybox()   = 0;
    public:
        virtual void DrawGrid()                 = 0;
        virtual void DrawSingleColors()         = 0;
        virtual void DrawTransparentGeometry()  = 0;
        virtual void DrawSettingsPanel()        = 0;
    };
}

#endif //GAMEENGINE_RENDER_H
