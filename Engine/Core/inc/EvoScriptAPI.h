//
// Created by Nikita on 11.07.2021.
//

#ifndef GAMEENGINE_EVOSCRIPTAPI_H
#define GAMEENGINE_EVOSCRIPTAPI_H

#include <Impl/EvoCompiler.h>

namespace EvoScript {
    class AddressTableGen;
}

namespace Framework {
    class API {
    public:
        API() = delete;
        API(const API &) = delete;
        ~API() = delete;
    public:
        static void RegisterEvoScriptClasses(Framework::Scripting::EvoCompiler *compiler);
        static void RegisterDebug(EvoScript::AddressTableGen* generator);
        static void RegisterEngine(EvoScript::AddressTableGen* generator);
        static void RegisterScene(EvoScript::AddressTableGen* generator);
        static void RegisterComponent(EvoScript::AddressTableGen* generator);
        static void RegisterMesh(EvoScript::AddressTableGen* generator);
        static void RegisterResourceManager(EvoScript::AddressTableGen* generator);
        static void RegisterGameObject(EvoScript::AddressTableGen* generator);
        static void RegisterCamera(EvoScript::AddressTableGen* generator);
        static void RegisterRender(EvoScript::AddressTableGen* generator);
        static void RegisterUtils(EvoScript::AddressTableGen* generator);
        static void RegisterWindow(EvoScript::AddressTableGen* generator);
        static void RegisterShader(EvoScript::AddressTableGen* generator);
        static void RegisterTransform(EvoScript::AddressTableGen* generator);
        static void RegisterInput(EvoScript::AddressTableGen* generator);
        static void RegisterSkybox(EvoScript::AddressTableGen* generator);
        static void RegisterTexture(EvoScript::AddressTableGen* generator);
        static void RegisterMaterial(EvoScript::AddressTableGen* generator);
        static void RegisterGUISystem(EvoScript::AddressTableGen* generator);
        static void RegisterPostProcessing(EvoScript::AddressTableGen* generator);
        static void RegisterISavable(EvoScript::AddressTableGen* generator);

        static void RegisterCasts(EvoScript::CastingGen* generator);
    };
}

#endif //GAMEENGINE_EVOSCRIPTAPI_H