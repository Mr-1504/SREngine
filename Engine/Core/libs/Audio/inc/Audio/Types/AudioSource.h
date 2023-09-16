//
// Created by Danilka000novishok on 08.08.2023.
//

#ifndef SRENGINE_AUDIOSOURCE_H
#define SRENGINE_AUDIOSOURCE_H

#include <Utils/ECS/Component.h>
#include <Utils/FileSystem/Path.h>
#include <Audio/PlayParams.h>

namespace SR_AUDIO_NS
{
    class AudioSource : public SR_UTILS_NS::Component {
        SR_ENTITY_SET_VERSION(1001);
        SR_INITIALIZE_COMPONENT(AudioSource);
        using Super = SR_UTILS_NS::Component;
        using Handle = void*;
    public:
        AudioSource();

    public:
        static SR_UTILS_NS::Component* LoadComponent(SR_HTYPES_NS::Marshal& marshal, const SR_HTYPES_NS::DataStorage* dataStorage);

        SR_NODISCARD SR_HTYPES_NS::Marshal::Ptr Save(SR_HTYPES_NS::Marshal::Ptr pMarshal, SR_UTILS_NS::SavableFlags flags) const override;

        void SetPath(std::string path);
        void SetVolume(float_t volume);

        SR_NODISCARD SR_UTILS_NS::Path GetPath() const;
        SR_NODISCARD float_t GetVolume() const;

    protected:
        void OnDisable() override;
        void OnEnable() override;
        void OnDestroy() override;

    private:
        void UpdateParams();

    private:
        PlayParams m_params = PlayParams::GetDefault();
        SR_UTILS_NS::Path m_path;
        Handle m_handle = nullptr;
    };
}

#endif //SRENGINE_AUDIOSOURCE_H