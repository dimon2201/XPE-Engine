#pragma once

#include <ecs/ecs.h>
#include <ecs/globals.h>

#include <rendering/camera.h>

namespace xpe {

    namespace ecs {

        using namespace core;

        template<typename T, typename... Args>
        T* Scene::AddGlobal(Args&&... args)
        {
            T* global = new T(std::forward<Args>(args)...);
            m_Globals.insert({ GetGlobalType<T>(), static_cast<Global*>(global) });
            return global;
        }

        struct ENGINE_API MainScene : public Scene
        {
            ecs::Audio* Audio = nullptr;
            ecs::Skybox* Skybox = nullptr;
            render::PerspectiveCamera* PerspectiveCamera = nullptr;
            render::OrthoCamera* OrthoCamera = nullptr;

            MainScene();

            MainScene(const string& tag);

            void ToJson(json &root) override;

            void FromJson(json &root) override;

        private:
            void Init();
        };

    }

}