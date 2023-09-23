//
// Created by Capitan_Slider on 16.09.1901.
//

#include <Audio/Types/AudioListener.h>

namespace SR_AUDIO_NS{
    SR_REGISTER_COMPONENT(AudioListener);

    AudioListener::AudioListener()
        : SR_UTILS_NS::Component()
    { }

    void AudioListener::OnDestroy() {
        Super::OnDestroy();
        delete this;
    }

    SR_UTILS_NS::Component* AudioListener::LoadComponent(SR_HTYPES_NS::Marshal& marshal, const SR_HTYPES_NS::DataStorage* dataStorage) {
        auto&& pComponent = new AudioListener();
        return dynamic_cast<Component*>(pComponent);
    }

    SR_HTYPES_NS::Marshal::Ptr AudioListener::Save(SR_UTILS_NS::SavableSaveData data) const {
        data.pMarshal = Super::Save(data);
        return data.pMarshal;
    }

    void AudioListener::OnAttached() {
        Component::OnAttached();
    }

}
