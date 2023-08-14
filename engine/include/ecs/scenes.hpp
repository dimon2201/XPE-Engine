#pragma once

#include <ecs/scene.h>
#include <ecs/globals.h>

#include <controls/camera.h>

namespace xpe {

    namespace ecs {

        using namespace core;
        using namespace control;

        template<typename T, typename... Args>
        T* Scene::AddGlobal(Args&&... args)
        {
            T* global = new T(std::forward<Args>(args)...);
            m_Globals.insert({ GetGlobalType<T>(), static_cast<Global*>(global) });
            return global;
        }

        struct ENGINE_API MainScene : public Scene
        {
            ecs::Skybox* Skybox = nullptr;
            control::PerspectiveCamera* PerspectiveCamera = nullptr;
            control::OrthoCamera* OrthoCamera = nullptr;

            MainScene();

            MainScene(const string& tag);

            void ToJson(json &root) override;

            void FromJson(json &root) override;

        private:
            void Init();
        };

    }

}