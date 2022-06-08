//
// Created by Monika on 05.06.2022.
//

#include <Render/CameraManager.h>
#include <Render/Camera.h>
#include <Types/Thread.h>

namespace SR_GRAPH_NS {
    void CameraManager::Update() {
        SR_LOCK_GUARD

        for (auto pIt = m_cameras.begin(); pIt != m_cameras.end();) {
            auto /** copy */ [pCamera, destroyed] = *pIt;

            if (destroyed) {
                SR_LOG("CameraManager::Update() : free camera...");

                delete pCamera;

                pIt = m_cameras.erase(pIt);

                Environment::Get()->SetBuildState(false);
            }
            else {
                ++pIt;
            }
        }
    }

    void CameraManager::OnWindowResized(Window *pWindow, uint32_t width, uint32_t height) {
        SR_LOCK_GUARD

        for (auto&&[camera, destroyed] : m_cameras) {
            if (destroyed) {
                continue;
            }

            camera->UpdateProjection(width, height);
        }
    }

    uint32_t Framework::Graphics::CameraManager::GetCountCameras() const {
        SR_LOCK_GUARD

        return static_cast<uint32>(m_cameras.size());
    }

    CameraManager::CameraPtr CameraManager::GetFirstCamera() const {
        SR_LOCK_GUARD

        for (auto&&[camera, destroyed] : m_cameras) {
            if (destroyed) {
                continue;
            }

            if (!camera->IsActive()) {
                continue;
            }

            return camera;
        }

        return CameraManager::CameraPtr();
    }

    void CameraManager::OnSingletonDestroy() {
        Update();

        SRAssert(m_cameras.empty());

        Singleton::OnSingletonDestroy();
    }

    void CameraManager::RegisterCamera(const CameraPtr &camera) {
        SR_LOCK_GUARD

        SR_LOG("CameraManager::RegisterCamera() : register new camera...");

        m_cameras.insert(std::make_pair(camera, false));
        Environment::Get()->SetBuildState(false);
    }

    void CameraManager::DestroyCamera(const CameraPtr &camera) {
        SR_LOCK_GUARD

        SR_LOG("CameraManager::Update() : destroy camera...");

        if (auto &&pIt = m_cameras.find(camera); pIt != m_cameras.end()) {
            pIt->second = true;
        }
        else {
            SRHalt("Camera not registered!");
        }
    }
}