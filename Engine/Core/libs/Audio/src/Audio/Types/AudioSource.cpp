//
// Created by Danilka000novishok on 08.08.2023.
//

#include <Audio/Types/AudioSource.h>
#include <Utils/ECS/ComponentManager.h>
#include <Audio/SoundManager.h>

namespace SR_AUDIO_NS
{
    SR_REGISTER_COMPONENT(AudioSource);

    AudioSource::AudioSource()
        : SR_UTILS_NS::Component()
    { }

    SR_UTILS_NS::Component* AudioSource::LoadComponent(SR_HTYPES_NS::Marshal &marshal, const SR_HTYPES_NS::DataStorage *dataStorage) {
        auto&& pComponent = new AudioSource();

        pComponent->m_path = marshal.Read<std::string>();
        pComponent->m_params.gain = marshal.Read<int32_t>();

        return dynamic_cast<Component*>(pComponent);
    }

    SR_NODISCARD SR_HTYPES_NS::Marshal::Ptr AudioSource::Save(SR_HTYPES_NS::Marshal::Ptr pMarshal, SR_UTILS_NS::SavableFlags flags) const{
        pMarshal = Component::Save(pMarshal, flags);
        pMarshal->Write<std::string>(m_path.ToString());
        pMarshal->Write<float_t>(GetVolume());
        return pMarshal;
    }

    float_t AudioSource::GetVolume() const {
        return m_params.gain.has_value() ? m_params.gain.value() : 0.f;
    }

    void AudioSource::SetVolume(float volume) {
        m_params.gain = volume;
        UpdateParams();
    }

    SR_UTILS_NS::Path AudioSource::GetPath() const {
        return m_path;
    }

    void AudioSource::SetPath(std::string path) {
        m_path = path;
    }

    void AudioSource::UpdateParams() {
        if (!m_handle) {
            return;
        }
        SoundManager::Instance().ApplyParams(m_handle, m_params);
    }

    void AudioSource::OnEnable() {
        m_handle = SoundManager::Instance().Play(m_path.ToString(),m_params);
        Component::OnEnable();
    }

    void AudioSource::OnDestroy() {
        SoundManager::Instance().Stop(m_handle);
        Super::OnDestroy();
        delete this;
    }

    void AudioSource::OnDisable() {
        SoundManager::Instance().Stop(m_handle);
        Component::OnDisable();
    }
}