#pragma once

#include <core/game.h>

namespace xpe {

    namespace core {

        class ENGINE_API Application : public Object {

        public:
            Time CPUTime;              // delta CPU time ms = end time - begin time
            Time DeltaTime;            // delta CurrentTime ms = end time - begin time
            Time CurrentTime;
            AppConfig Config;
            glm::vec4 ClearColor = { 1, 1, 1, 1 };

            Application() {}
            ~Application() {}

            void Run();

        protected:

            void LockFPSFromConfig();

            virtual void Init() {}
            virtual void Update() {}
            virtual void Free() {}
            virtual void Render();

            virtual Game* CreateGame();
            void InitGame();

            virtual void InitRenderer();

            Game* m_Game = nullptr;

            ecs::MainScene* m_MainScene = nullptr;

            render::FontStorage* m_FontStorage = nullptr;
            render::GeometryStorage* m_GeometryStorage = nullptr;
            render::MaterialStorage* m_MaterialStorage = nullptr;
            render::TextureStorage* m_TextureStorage = nullptr;
            render::Canvas* m_Canvas = nullptr;
            render::Renderer* m_Renderer = nullptr;

            anim::Animator* m_Animator = nullptr;
            anim::SkeletStorage* m_SkeletStorage = nullptr;
            anim::SkinStorage* m_SkinStorage = nullptr;
            anim::AnimStorage* m_AnimStorage = nullptr;

            audio::AudioSystem* m_AudioSystem = nullptr;
            audio::AudioStorage* m_AudioStorage = nullptr;
        };

    }

}

xpe::core::Application* CreateApplication();