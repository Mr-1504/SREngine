//
// Created by Monika on 09.07.2022.
//

#ifndef SR_ENGINE_OPENALTOOLS_H
#define SR_ENGINE_OPENALTOOLS_H

#include <Utils/macros.h>

#include <AL/al.h>
#include <AL/alc.h>

namespace SR_AUDIO_NS {
    typedef struct ALfVec3 { ALfloat vec3[3]; } ALfVec3;
    typedef struct ALfVec6 { ALfloat vec6[6]; } ALfVec6;

    /*bool get_available_devices(std::vector<std::string>& devicesVec, ALCdevice* device)
    {
        const ALCchar* devices;
        if(!alcCall(alcGetString, devices, device, nullptr, ALC_DEVICE_SPECIFIER))
            return false;

        const char* ptr = devices;

        devicesVec.clear();

        do
        {
            devicesVec.push_back(std::string(ptr));
            ptr += devicesVec.back().size() + 1;
        }
        while(*(ptr + 1) != '\0');

        return true;
    }*/

    ALenum DistanceModelToALDistanceModel(ListenerDistanceModel distanceModel) {
        switch (distanceModel) {
            case ListenerDistanceModel::None:
                return AL_NONE;
            case ListenerDistanceModel::InverseDistance:
                return AL_INVERSE_DISTANCE;
            case ListenerDistanceModel::InverseDistanceClamped:
                return AL_INVERSE_DISTANCE_CLAMPED;
            case ListenerDistanceModel::LinearDistance:
                return AL_LINEAR_DISTANCE;
            case ListenerDistanceModel::LinearDistanceClamped:
                return AL_LINEAR_DISTANCE_CLAMPED;
            case ListenerDistanceModel::ExponentDistance:
                return AL_EXPONENT_DISTANCE;
            case ListenerDistanceModel::ExponentDistanceClamped:
                return AL_EXPONENT_DISTANCE_CLAMPED;
            default:
                SRHalt("DistanceModelToALDistanceModel() : no such distance model;");
                return AL_NONE;
        }
    }

    SR_MAYBE_UNUSED static ALfVec6 FV6ToALV6(const SR_MATH_NS::FVector6& vector6) {
        ALfVec6 vec6 = {vector6.x, vector6.y, vector6.z, vector6.w, vector6.v, vector6.t};
        return vec6;
    }

    SR_MAYBE_UNUSED static ALfVec3 FV3ToALV3(const SR_MATH_NS::FVector3& vector3) {
        ALfVec3 vec3 =  {vector3.x, vector3.y, vector3.z};
        return vec3;
    }

    //this is here thanks to https://indiegamedev.net/2020/02/15/the-complete-guide-to-openal-with-c-part-1-playing-a-sound/
    bool check_alc_errors(const std::string& filename, const std::uint_fast32_t line, ALCdevice* device)
    {
        ALCenum error = alcGetError(device);
        if(error != ALC_NO_ERROR)
        {
            std::cerr << "***ERROR*** (" << filename << ": " << line << ")\n" ;
            switch(error)
            {
                case ALC_INVALID_VALUE:
                    std::cerr << "ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function";
                    break;
                case ALC_INVALID_DEVICE:
                    std::cerr << "ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function";
                    break;
                case ALC_INVALID_CONTEXT:
                    std::cerr << "ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function";
                    break;
                case ALC_INVALID_ENUM:
                    std::cerr << "ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function";
                    break;
                case ALC_OUT_OF_MEMORY:
                    std::cerr << "ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function";
                    break;
                default:
                    std::cerr << "UNKNOWN ALC ERROR: " << error;
            }
            std::cerr << std::endl;
            return false;
        }
        return true;
    }
    bool check_al_errors(const std::string& filename, const std::uint_fast32_t line)
    {
        ALenum error = alGetError();
        if(error != AL_NO_ERROR)
        {
            std::cerr << "***ERROR*** (" << filename << ": " << line << ")\n" ;
            switch(error)
            {
                case AL_INVALID_NAME:
                    std::cerr << "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function";
                    break;
                case AL_INVALID_ENUM:
                    std::cerr << "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function";
                    break;
                case AL_INVALID_VALUE:
                    std::cerr << "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function";
                    break;
                case AL_INVALID_OPERATION:
                    std::cerr << "AL_INVALID_OPERATION: the requested operation is not valid";
                    break;
                case AL_OUT_OF_MEMORY:
                    std::cerr << "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory";
                    break;
                default:
                    std::cerr << "UNKNOWN AL ERROR: " << error;
            }
            std::cerr << std::endl;
            return false;
        }
        return true;
    }

    template<typename alFunction, typename... Params> auto alCallImpl(const char* filename,
        const std::uint_fast32_t line,
        alFunction function,
        Params... params)
        ->typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, decltype(function(params...))>
    {
        auto ret = function(std::forward<Params>(params)...);
        check_al_errors(filename, line);
        return ret;
    }

    template<typename alFunction, typename... Params> auto alCallImpl(const char* filename,
        const std::uint_fast32_t line,
        alFunction function,
        Params... params)
        ->typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>
    {
        function(std::forward<Params>(params)...);
        return check_al_errors(filename, line);
    }

    template<typename alcFunction, typename... Params> auto alcCallImpl(const char* filename,
        const std::uint_fast32_t line,
        alcFunction function,
        ALCdevice* device,
        Params... params)
        ->typename std::enable_if_t<std::is_same_v<void,decltype(function(params...))>,bool>
    {
        function(std::forward<Params>(params)...);
        return check_alc_errors(filename,line,device);
    }

    template<typename alcFunction, typename ReturnType, typename... Params> auto alcCallImpl(const char* filename,
        const std::uint_fast32_t line,
        alcFunction function,
        ReturnType& returnValue,
        ALCdevice* device,
        Params... params)
        ->typename std::enable_if_t<!std::is_same_v<void,decltype(function(params...))>,bool>
    {
        returnValue = function(std::forward<Params>(params)...);
        return check_alc_errors(filename,line,device);
    }
}

#define SR_ALC_CALL(function, device, ...) SR_AUDIO_NS::alcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)
#define SR_AL_CALL(function, ...) SR_AUDIO_NS::alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)

#endif //SR_ENGINE_OPENALTOOLS_H
