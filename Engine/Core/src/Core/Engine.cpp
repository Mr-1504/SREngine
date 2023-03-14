//
// Created by Nikita on 29.12.2020.
//

#include <Core/Engine.h>
#include <Core/GUI/EditorGUI.h>
#include <Core/UI/Button.h>
#include <Core/World/SceneInitializer.h>

#include <Utils/Events/EventManager.h>
#include <Utils/World/Scene.h>
#include <Utils/World/SceneBuilder.h>
#include <Utils/Common/Features.h>
#include <Utils/Types/SafePtrLockGuard.h>
#include <Utils/ECS/Prefab.h>
#include <Utils/ECS/ComponentManager.h>
#include <Utils/ECS/Migration.h>
#include <Utils/DebugDraw.h>

#include <Graphics/Pipeline/Environment.h>
#include <Graphics/GUI/WidgetManager.h>
#include <Graphics/Render/Render.h>
#include <Graphics/Render/RenderScene.h>
#include <Graphics/Render/DebugRenderer.h>
#include <Graphics/Render/RenderContext.h>
#include <Graphics/Memory/CameraManager.h>
#include <Graphics/Types/Camera.h>
#include <Graphics/Animations/Bone.h>
#include <Graphics/Window/Window.h>
#include <Graphics/UI/Sprite2D.h>
#include <Graphics/Types/Geometry/SkinnedMesh.h>
#include <Graphics/GUI/Editor/Theme.h>

#include <Physics/Rigidbody.h>
#include <Physics/LibraryImpl.h>
#include <Physics/PhysicsLib.h>
#include <Physics/PhysicsScene.h>
#include <Physics/3D/Rigidbody3D.h>
#include <Physics/3D/Raycast3D.h>
#include <Physics/PhysicsMaterial.h>

#include <Scripting/Impl/EvoScriptManager.h>

namespace SR_CORE_NS {
    bool Engine::Create() {
        SR_INFO("Engine::Create() : register all resources...");

        if (!Resources::RegisterResources()) {
            SR_ERROR("Engine::Create() : failed to register engine resources!");
            return false;
        }

        RegisterMigrators();

        SR_INFO("Engine::Create() : create main window...");

        if (!CreateMainWindow()) {
            SR_ERROR("Engine::Create() : failed to create main window!");
            return false;
        }

        if (!InitializeRender()) {
            SR_ERROR("Engine::Create() : failed to initialize render!");
            return false;
        }

        SR_UTILS_NS::ComponentManager::Instance().SetContextInitializer([](auto&& context) {
            context.SetValue(Engine::Instance().GetWindow());

            context.template SetPointer<SR_PHYSICS_NS::LibraryImpl>("2DPLib", SR_PHYSICS_NS::PhysicsLibrary::Instance().GetActiveLibrary(SR_UTILS_NS::Measurement::Space2D));
            context.template SetPointer<SR_PHYSICS_NS::LibraryImpl>("3DPLib", SR_PHYSICS_NS::PhysicsLibrary::Instance().GetActiveLibrary(SR_UTILS_NS::Measurement::Space3D));
        });

        RegisterLibraries();

        m_cmdManager = new SR_UTILS_NS::CmdManager();
        m_input      = new SR_UTILS_NS::InputDispatcher();
        m_editor     = new Core::GUI::EditorGUI();

        m_worldTimer = SR_HTYPES_NS::Timer(1u);

        if (m_isCreate) {
            SR_ERROR("Engine::Create() : game engine is already created!");
            return false;
        }

        SR_INFO("Engine::Create() : creating game engine...");

        m_input->Register(&Graphics::GUI::GlobalWidgetManager::Instance());
        m_input->Register(m_editor);

        SetGameMode(!SR_UTILS_NS::Features::Instance().Enabled("EditorOnStartup", false));

        if (!m_scene.Valid() && !m_editor->LoadSceneFromCachedPath()) {
            auto&& scenePath = SR_UTILS_NS::ResourceManager::Instance().GetCachePath().Concat("Scenes/New-scene.scene");

            if (SR_PLATFORM_NS::IsExists(scenePath)) {
                if (!SetScene(SR_WORLD_NS::Scene::Load(scenePath))) {
                    SR_ERROR("Engine::Create() : failed to load scene!\n\tPath: " + scenePath.ToString())
                }
            }
            else if (!SetScene(SR_WORLD_NS::Scene::New(scenePath))) {
                SR_ERROR("Engine::Create() : failed to create new scene!\n\tPath: " + scenePath.ToString())
            }
        }

        FlushScene();

        m_updateFrequency = (1.f / (60.f * m_speed)) * CLOCKS_PER_SEC;
        m_accumulator = m_updateFrequency;
        m_timeStart = Clock::now();

        m_isCreate = true;

        return true;
    }

    bool Engine::CreateMainWindow() {
        SR_LOG("Engine::CreateMainWindow() : try found screen resolution...");

        auto&& resolutions = SR_PLATFORM_NS::GetScreenResolutions();

        if (resolutions.empty()) {
            SR_ERROR("Engine::CreateMainWindow() : not found supported resolutions!");
            return false;
        }
        else {
            SR_LOG("Engine::CreateMainWindow() : found " + std::to_string(resolutions.size()) + " resolutions");
        }

        auto&& resolution = resolutions[SR_MAX(static_cast<uint32_t>(resolutions.size() / 2), 0)];

        m_window = new SR_GRAPH_NS::Window();
        if (!m_window->Initialize("SpaRcle Engine", resolution)) {
            SR_ERROR("Engine::CreateMainWindow() : failed to initialize window!");
            return false;
        }

        if (auto&& pWin = m_window->GetImplementation<SR_GRAPH_NS::BasicWindowImpl>()) {
            pWin->Centralize();
            pWin->Maximize();
        }

        SR_LOG("Engine::CreateMainWindow() : initializing window callbacks...");

        m_window->SetDrawCallback([this]() {
            DrawCallback();
        });

        m_window->SetFocusCallback([this](bool focus) {
            SR_SYSTEM_LOG(SR_UTILS_NS::Format("Window focus state: %s", focus ? "True" : "False"));
            SR_UTILS_NS::Input::Instance().Reload();
        });

        m_window->SetResizeCallback([this](auto&& size) {
            m_pipeline->OnResize(size);
            m_renderContext->OnResize(size);
        });

        m_window->SetScrollCallback([](double_t xOffset, double_t yOffset) {
            SR_UTILS_NS::Input::Instance().SetMouseScroll(xOffset, yOffset);
        });

        m_window->SetCloseCallback([this]() {
            m_pipeline->StopGUI();

            if (auto&& pWin = m_window->GetImplementation<SR_GRAPH_NS::BasicWindowImpl>()) {
                pWin->StopGUI();
            }

            m_renderContext.Do([](auto&& pContext) {
                pContext->Close();
            });

            SynchronizeFreeResources();

            if (m_pipeline) {
                m_pipeline->DeInitialize();
                m_pipeline = nullptr;
            }
        });

        return true;
    }

    void Engine::SynchronizeFreeResources() {
        SR_SYSTEM_LOG("Engine::SynchronizeFreeResources() : synchronizing resources...");

        SR_LOCK_GUARD

        std::atomic<bool> syncComplete(false);

        /** Ждем, пока все графические ресурсы не освободятся */
        auto&& thread = SR_HTYPES_NS::Thread::Factory::Instance().Create([&syncComplete, this]() {
            uint32_t syncStep = 0;
            const uint32_t maxErrStep = 50;

            SR_UTILS_NS::ResourceManager::Instance().Synchronize(true);

            while(!m_renderContext->IsEmpty()) {
                SR_SYSTEM_LOG("Engine::SynchronizeFreeResources() : synchronizing resources (step " + std::to_string(++syncStep) + ")");

                SR_UTILS_NS::ResourceManager::Instance().Synchronize(true);

                if (maxErrStep == syncStep) {
                    SR_ERROR("Engine::SynchronizeFreeResources() : [FATAL] resources can not be released!");
                    SR_UTILS_NS::ResourceManager::Instance().PrintMemoryDump();
                    SR_UTILS_NS::Debug::Instance().Terminate();
                    break;
                }

                Helper::Types::Thread::Sleep(50);
            }

            SR_SYSTEM_LOG("Engine::SynchronizeFreeResources() : close synchronization thread...");

            syncComplete = true;
        });

        /** Так как некоторые ресурсы, такие как материалы, имеют вложенные ресурсы,
         * то они могут ожидать пока графический поток уберет метку использования с них */
        while (!syncComplete) {
            SR_UTILS_NS::ResourceManager::LockSingleton();
            m_renderContext.Do([](auto&& pContext) {
                pContext->Update();
            });
            SR_UTILS_NS::ResourceManager::UnlockSingleton();
        }

        SR_UTILS_NS::ResourceManager::Instance().Synchronize(true);

        thread->TryJoin();
        thread->Free();

        SR_SYSTEM_LOG("Engine::SynchronizeFreeResources() : synchronizing is complete!");
    }

    bool Engine::InitializeRender() {
        m_renderContext = new SR_GRAPH_NS::RenderContext(m_window);
        m_pipeline = SR_GRAPH_NS::Environment::Get();

        return m_window->GetThread()->Execute([this]() -> bool {
            if (!m_renderContext->Init()) {
                SR_ERROR("Engine::InitializeRender() : failed to initialize the render context!");
                return false;
            }

            SR_GRAPH("Engine::InitializeRender() : initializing the render environment...");

            SR_GRAPH("Engine::InitializeRender() : pre-initializing...");
            if (!m_pipeline->PreInit(
                    64,
                    "SpaRcle Engine", /// App name
                    "SREngine",       /// Engine name
                    SR_UTILS_NS::ResourceManager::Instance().GetResPath().Concat("Engine/Utilities/glslc.exe")))
            {
                SR_ERROR("Engine::InitializeRender() : failed to pre-initialize the environment!");
                return false;
            }

            SR_GRAPH_LOG("Engine::InitializeRender() : set thread context as current...");
            if (!m_pipeline->SetContextCurrent()) {
                SR_ERROR("Engine::InitializeRender() : failed to set context!");
                return false;
            }

            SR_GRAPH("Engine::InitializeRender() : initializing the environment...");
            if (!m_pipeline->Init(m_window, false /** vsync */)) {
                SR_ERROR("Engine::InitializeRender() : failed to initialize the environment!");
                return false;
            }

            SR_GRAPH("Engine::InitializeRender() : post-initializing the environment...");

            if (!m_pipeline->PostInit()) {
                SR_ERROR("Engine::InitializeRender() : failed to post-initialize environment!");
                return false;
            }

            SR_LOG("Engine::InitializeRender() : vendor is "   + m_pipeline->GetVendor());
            SR_LOG("Engine::InitializeRender() : renderer is " + m_pipeline->GetRenderer());
            SR_LOG("Engine::InitializeRender() : version is "  + m_pipeline->GetVersion());

            if (m_pipeline->IsGUISupport()) {
                if (m_pipeline->PreInitGUI(SR_UTILS_NS::ResourceManager::Instance().GetResPath().Concat("Engine/Fonts/tahoma.ttf"))) {
                    ImGuiStyle & style = ImGui::GetStyle();

                    if (auto&& theme = SR_GRAPH_NS::GUI::Theme::Load("Engine/Configs/Themes/Dark.xml")) {
                        theme->Apply(style);
                        delete theme;
                    }
                    else {
                        SR_ERROR(" Engine::InitializeRender() : failed to load theme!");
                    }

                    const static auto iniPathEditor = SR_UTILS_NS::ResourceManager::Instance().GetCachePath().Concat("Editor/Configs/ImGuiEditor.config");
                    const static auto iniPathWidgets = SR_UTILS_NS::ResourceManager::Instance().GetCachePath().Concat("Editor/Configs/EditorWidgets.xml");
                    if (!iniPathEditor.Exists()) {
                        iniPathEditor.Make(SR_UTILS_NS::Path::Type::File);
                        SR_UTILS_NS::Platform::Copy(SR_UTILS_NS::ResourceManager::Instance().GetResPath().Concat("Editor/Configs/ImGuiEditor.config"),iniPathEditor);
                        SR_UTILS_NS::Platform::Copy(SR_UTILS_NS::ResourceManager::Instance().GetResPath().Concat("Editor/Configs/EditorWidgets.xml"), iniPathWidgets);
                    }
                    ImGui::GetIO().IniFilename = iniPathEditor.CStr();

                    m_window->GetImplementation<SR_GRAPH_NS::BasicWindowImpl>()->InitGUI();

                    if (!m_pipeline->InitGUI()) {
                        SR_ERROR("Engine::InitializeRender() : failed to initialize the GUI!");
                        return false;
                    }
                }
                else {
                    SR_ERROR("Engine::InitializeRender() : failed to pre-initialize the GUI!");
                    return false;
                }
            }

            SR_THIS_THREAD->GetContext()->SetValue<RenderContextPtr>(m_renderContext);
            SR_THIS_THREAD->GetContext()->SetValue<WindowPtr>(m_window);

            return true;
        });
    }

    bool Engine::Init() {
        if (!m_isCreate) {
            SR_ERROR("Engine::Init() : engine is not created!");
            return false;
        }

        if (m_isInit) {
            SR_ERROR("Engine::Init() : engine is already initialized!");
            return false;
        }

        SR_INFO("Engine::Init() : initializing game engine...");

        m_isInit = true;

        return true;
    }

    bool Engine::Run() {
        if (!m_isInit) {
            SR_ERROR("Engine::Run() : engine is not initialized!");
            return false;
        }

        if (m_isRun) {
            SR_ERROR("Engine::Run() : engine already running!");
            return false;
        }

        SR_INFO("Engine::Run() : running game engine...");

        if (!Core::Commands::RegisterEngineCommands()) {
            SR_ERROR("Engine::Run() : errors were detected during the registration of commands!");
            return false;
        }

        m_isRun = true;

        if (SR_UTILS_NS::Features::Instance().Enabled("ChunkSystem", true)) {
            SR_INFO("Engine::Run() : running world thread...");

            m_worldThread = SR_HTYPES_NS::Thread::Factory::Instance().Create(&Engine::WorldThread, this);
        }

        return true;
    }

    void Engine::Await() {
        SR_INFO("Engine::Await() : waiting for the engine to close...");

        while (m_isRun) {
            SR_HTYPES_NS::Thread::Sleep(10);

            if (!m_window || !m_window->IsValid()) {
                SR_SYSTEM_LOG("Engine::Await() : window has been closed!");
                break;
            }
        }

        if (m_editor && m_editor->Enabled()) {
            m_editor->Enable(false);
            SR_SYSTEM_LOG("Engine::Await() : disabling editor gui...");
        }
    }

    bool Engine::Close() {
        SR_INFO("Engine::Close() : closing game engine...");

        m_isRun = false;

        if (m_input) {
            m_input->UnregisterAll();
        }
        SR_SAFE_DELETE_PTR(m_input);

        FlushScene();

        if (m_scene.Valid()) {
            SetScene(ScenePtr());
            FlushScene();
        }

        SR_INFO("Engine::Close() : destroying the editor...");
        SR_SAFE_DELETE_PTR(m_editor);

        if (m_cmdManager) {
            SR_INFO("Engine::Close() : close the command manager...");
            m_cmdManager->Close();
        }
        SR_SAFE_DELETE_PTR(m_cmdManager);

        if (m_worldThread) {
            SR_INFO("Engine::Close() : destroying world thread...");
            m_worldThread->TryJoin();
            m_worldThread->Free();
            m_worldThread = nullptr;
        }

        m_window.AutoFree([](auto&& pWindow) {
            pWindow->Close();
            delete pWindow;
        });

        SR_SCRIPTING_NS::EvoScriptManager::Instance().Update(0.f, true);

        return true;
    }

    void Engine::DrawCallback() {
        if (!m_isRun || !m_window || m_window->IsWindowCollapsed()) {
            m_accumulator = 0.f;
            return;
        }

        SR_HTYPES_NS::Time::Instance().Update();

        FlushScene();

        if (m_renderScene.RecursiveLockIfValid()) {
            SR_UTILS_NS::DebugDraw::Instance().SwitchCallbacks(m_renderScene->GetDebugRenderer());
            m_renderScene.Unlock();
        }

        if (m_physicsScene.LockIfValid()){
            SR_PHYSICS_NS::Raycast3D::Instance().SwitchPhysics(m_physicsScene->Get3DWorld());
            m_physicsScene.Unlock();
        }

        if (m_scene.LockIfValid()) {
            const auto now = SR_HTYPES_NS::Time::Instance().Now();
            const auto deltaTime = now - m_timeStart;
            const auto dt = static_cast<float_t>(deltaTime.count()) / CLOCKS_PER_SEC / CLOCKS_PER_SEC;
            m_timeStart = now;

            m_cmdManager->Update();

            m_scene->GetLogicBase()->PostLoad();

            SR_SCRIPTING_NS::EvoScriptManager::Instance().Update(dt, false);

            Prepare();

            Update(dt);

            /// fixed update
            if (m_accumulator >= m_updateFrequency)
            {
                while (m_accumulator >= m_updateFrequency)
                {
                    if (!IsPaused() && IsActive() && m_physicsScene.RecursiveLockIfValid()) {
                        m_physicsScene->FixedUpdate();
                        m_physicsScene.Unlock();
                    }

                    FixedUpdate();

                    m_accumulator -= m_updateFrequency;
                }
            }

            m_accumulator += dt;

            m_scene.Unlock();
        }

        if (m_renderContext.LockIfValid()) {
            m_renderContext->Update();
            m_renderContext.Unlock();
        }

        if (m_renderScene.RecursiveLockIfValid()) {
            if (auto&& pWin = GetWindow()->GetImplementation<SR_GRAPH_NS::BasicWindowImpl>()) {
                const bool isOverlay = m_renderScene->IsOverlayEnabled();
                const bool isMaximized = pWin->IsMaximized();
                const bool isHeaderEnabled = pWin->IsHeaderEnabled();

                if (isHeaderEnabled != !isOverlay) {
                    pWin->SetHeaderEnabled(!isOverlay);
                    if (isMaximized) {
                        pWin->Maximize();
                    }
                }
            }

            m_renderScene->Render();
            /// В процессе отрисовки сцена могла быть заменена
            m_renderScene.TryUnlock();
        }
    }

    bool Engine::RegisterLibraries() {
        SR_LOG("Engine::RegisterLibraries() : registering all libraries...");

        API::RegisterEvoScriptClasses();

        return true;
    }

    bool Engine::SetScene(const SR_HTYPES_NS::SafePtr<SR_WORLD_NS::Scene> &scene)  {
        SRAssert(!scene.IsLocked());
        m_sceneQueue.Push(scene);
        return true;
    }

    void Engine::Reload() {
        SR_PLATFORM_NS::SelfOpen();
    }

    void Engine::WorldThread() {
        while (m_isRun) {
            SR_HTYPES_NS::Thread::Sleep(250);

            m_mainCamera = m_renderScene.Do<CameraPtr>([](SR_GRAPH_NS::RenderScene* ptr) -> CameraPtr {
                if (auto&& pCamera = ptr->GetMainCamera()) {
                    return pCamera;
                }

                return ptr->GetFirstOffScreenCamera();
            }, nullptr);

            if (m_worldTimer.Update() && m_scene.LockIfValid()) {
                if (!m_mainCamera) {
                    SR_NOOP;
                }
                else if (auto&& gameObject = dynamic_cast<SR_UTILS_NS::GameObject*>(m_mainCamera->GetParent())) {
                    auto&& pLogic = m_scene->GetLogicBase().DynamicCast<SR_WORLD_NS::SceneCubeChunkLogic*>();
                    if (pLogic && gameObject) {
                        pLogic->SetObserver(gameObject->GetThis());
                    }
                }

                m_scene->GetLogicBase()->Update(m_worldTimer.GetDeltaTime());
                m_scene.Unlock();
            }
        }

        SR_SYSTEM_LOG("Engine::WorldThread() : world thread completed!");
    }

    void Engine::FixedUpdate() {
        ///В этом блоке находится обработка нажатия клавиш, которая не должна срабатывать, если окно не сфокусированно
        if (m_window->IsWindowFocus())
        {
            SR_UTILS_NS::Input::Instance().Check();

            m_input->Check();

            bool lShiftPressed = SR_UTILS_NS::Input::Instance().GetKeyDown(SR_UTILS_NS::KeyCode::LShift);

            if (!IsGameMode() && SR_UTILS_NS::Input::Instance().GetKey(SR_UTILS_NS::KeyCode::Ctrl)) {
                if (SR_UTILS_NS::Input::Instance().GetKeyDown(SR_UTILS_NS::KeyCode::Z))
                    m_cmdManager->Cancel();

                if (SR_UTILS_NS::Input::Instance().GetKeyDown(SR_UTILS_NS::KeyCode::Y))
                    if (!m_cmdManager->Redo())
                        SR_WARN("Engine::Await() : failed to redo \"" + m_cmdManager->GetLastCmdName() + "\" command!");
            }

            if (!IsGameMode() && m_editor && SR_UTILS_NS::Input::Instance().GetKeyDown(SR_UTILS_NS::KeyCode::F1)) {
                m_editor->SetDockingEnabled(!m_editor->IsDockingEnabled());
            }

            if (m_editor && SR_UTILS_NS::Input::Instance().GetKeyDown(SR_UTILS_NS::KeyCode::F9)) {
                SR_UTILS_NS::Input::Instance().LockCursor(!SR_UTILS_NS::Input::Instance().IsCursorLocked());
            }

            if (m_editor && IsActive() && SR_UTILS_NS::Input::Instance().GetKeyDown(SR_UTILS_NS::KeyCode::F2)) {
                SetGameMode(!IsGameMode());
            }

            if (SR_UTILS_NS::Input::Instance().GetKeyDown(SR_UTILS_NS::KeyCode::F3) && lShiftPressed) {
                Reload();
                return;
            }

            if (IsGameMode() && SR_UTILS_NS::Input::Instance().IsMouseMoved() && SR_UTILS_NS::Input::Instance().IsCursorLocked()) {
                auto&& resolution = m_window->GetSize();
                resolution /= 2;
                SR_PLATFORM_NS::SetMousePos(m_window->GetPosition() + resolution.Cast<int32_t>());
                SR_UTILS_NS::Input::Instance().ResetMouse();
            }
        }

        m_sceneBuilder->FixedUpdate();

        if (m_editor && m_window->IsWindowFocus()) {
            m_editor->Update();
        }
    }

    void Engine::Update(float_t dt) {
        m_sceneBuilder->Update(dt);
    }

    void Engine::Prepare() {
        const bool isPaused = !m_isActive || m_isPaused;
        m_sceneBuilder->Build(isPaused);
    }

    void Engine::SetActive(bool isActive) {
        if (m_isActive == isActive) {
            return;
        }
        m_isActive = isActive;
        m_sceneBuilder->SetDirty();
    }

    void Engine::SetSpeed(float_t speed) {
        m_speed = speed;
    }

    void Engine::SetPaused(bool isPaused) {
        if (m_isPaused == isPaused) {
            return;
        }
        m_isPaused = isPaused;
        m_sceneBuilder->SetDirty();
    }

    void Engine::FlushScene() {
        /// не блочим, иначе deadlock
        /// SR_LOCK_GUARD

        if (m_sceneQueue.Empty()) {
            return;
        }

        m_sceneQueue.Lock();

    repeat:
        if (m_sceneQueue.Empty()) {
            m_sceneQueue.Unlock();
            return;
        }

        auto&& newScene = m_sceneQueue.Pop(ScenePtr());
        if (newScene && !SR_CORE_NS::InitializeScene(newScene, this)) {
            SR_ERROR("Engine::FlushScene() : failed to initialize scene!");
        }

        if (m_scene.RecursiveLockIfValid()) {
            if (m_editor) {
                m_editor->SetScene(SR_WORLD_NS::Scene::Ptr());
            }

            m_scene->Save();
            if (!DeInitializeScene(m_scene, this)) {
                SR_ERROR("Engine::FlushScene() : failed to de initialize scene!");
            }
            m_scene.Unlock();
        }

        if (m_editor) {
            m_editor->SetScene(newScene);
        }

        if (m_cmdManager) {
            m_cmdManager->Clear();
        }

        m_scene = newScene;

        m_renderScene = m_scene ? m_scene->GetDataStorage().GetValue<RenderScenePtr>() : RenderScenePtr();
        m_physicsScene = m_scene ? m_scene->GetDataStorage().GetValue<PhysicsScenePtr>() : PhysicsScenePtr();

        m_sceneBuilder = m_scene ? m_scene->GetSceneBuilder() : nullptr;

        if (!m_sceneQueue.Empty()) {
            goto repeat;
        }

        m_sceneQueue.Unlock();
    }

    void Engine::RegisterMigrators() {
        static const auto GAME_OBJECT_HASH_NAME = SR_HASH_STR("GameObject");
        SR_UTILS_NS::Migration::Instance().RegisterMigrator(GAME_OBJECT_HASH_NAME, 1004, 1005, [](SR_HTYPES_NS::Marshal& marshal) -> bool {
            SR_HTYPES_NS::Marshal migrated;

            uint64_t position = marshal.GetPosition();

            migrated.Stream::Write(marshal.Stream::View(), marshal.GetPosition());

            migrated.Write(marshal.Read<bool>());
            auto name = marshal.Read<std::string>();
            migrated.Write(name);
            migrated.Write(marshal.Read<uint64_t>());

            auto&& measurement = static_cast<SR_UTILS_NS::Measurement>(marshal.Read<int8_t>());

            migrated.Write<uint8_t>(static_cast<uint8_t>(measurement));

            switch (measurement) {
                case SR_UTILS_NS::Measurement::Space2D:
                case SR_UTILS_NS::Measurement::Space3D:
                    migrated.Write<SR_MATH_NS::FVector3>(marshal.Read<SR_MATH_NS::Vector3<double>>(SR_MATH_NS::Vector3<double>(0.0)).Cast<SR_MATH_NS::Unit>(), SR_MATH_NS::FVector3(0.f));
                    migrated.Write<SR_MATH_NS::FVector3>(marshal.Read<SR_MATH_NS::Vector3<double>>(SR_MATH_NS::Vector3<double>(0.0)).Cast<SR_MATH_NS::Unit>(), SR_MATH_NS::FVector3(0.f));
                    migrated.Write<SR_MATH_NS::FVector3>(marshal.Read<SR_MATH_NS::Vector3<double>>(SR_MATH_NS::Vector3<double>(1.0)).Cast<SR_MATH_NS::Unit>(), SR_MATH_NS::FVector3(1.f));
                    migrated.Write<SR_MATH_NS::FVector3>(marshal.Read<SR_MATH_NS::Vector3<double>>(SR_MATH_NS::Vector3<double>(1.0)).Cast<SR_MATH_NS::Unit>(), SR_MATH_NS::FVector3(1.f));
                    SR_FALLTHROUGH;
                default:
                    break;
            }

            migrated.Stream::Write(marshal.Stream::View() + marshal.GetPosition(), marshal.Size() - marshal.GetPosition());

            marshal.SetData(migrated.Stream::View(), migrated.Size());
            marshal.SetPosition(position);

            return true;
        });

        static const auto RIGID_BODY_3D_HASH_NAME = SR_HASH_STR("Rigidbody3D");
        SR_UTILS_NS::Migration::Instance().RegisterMigrator(RIGID_BODY_3D_HASH_NAME, 1004, 1005, [](SR_HTYPES_NS::Marshal& marshal) -> bool {
            SR_HTYPES_NS::Marshal migrated;

            uint64_t position = marshal.GetPosition();

            migrated.Stream::Write(marshal.Stream::View(), marshal.GetPosition());
            /// --------------------
            migrated.Write<int32_t>(marshal.Read<int32_t>());

            migrated.Write<SR_MATH_NS::Vector3<float_t>>(marshal.Read<SR_MATH_NS::Vector3<float_t>>(SR_MATH_NS::Vector3<float_t>(0.f)), SR_MATH_NS::Vector3<float_t>(0.f));
            migrated.Write<SR_MATH_NS::Vector3<float_t>>(marshal.Read<SR_MATH_NS::Vector3<float_t>>(SR_MATH_NS::Vector3<float_t>(1.f)), SR_MATH_NS::Vector3<float_t>(1.f));

            migrated.Write<float_t>(marshal.Read<float_t>());
            migrated.Write<bool>(marshal.Read<bool>());
            migrated.Write<bool>(marshal.Read<bool>());
            migrated.Write<std::string>("Engine/PhysicsMaterials/DefaultMaterial.physmat");


            migrated.Write<SR_MATH_NS::BVector3>(marshal.Read<SR_MATH_NS::BVector3>());
            migrated.Write<SR_MATH_NS::BVector3>(marshal.Read<SR_MATH_NS::BVector3>());

            /// -------------------- меня наняли дублировать длинные строки потому что я люблю большие длинные прямые комментарии, состоящие исключительно из тире.
            migrated.Stream::Write(marshal.Stream::View() + marshal.GetPosition(), marshal.Size() - marshal.GetPosition());

            marshal.SetData(migrated.Stream::View(), migrated.Size());
            marshal.SetPosition(position);

            return true;
        });
    }

    void Engine::SetGameMode(bool enabled) {
        m_isGameMode = enabled;

        m_renderScene.Do([this](SR_GRAPH_NS::RenderScene *ptr) {
            ptr->SetOverlayEnabled(!m_isGameMode);
        });

        m_editor->Enable(!m_isGameMode);
    }
}