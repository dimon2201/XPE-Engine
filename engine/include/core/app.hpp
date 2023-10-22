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

            std::atomic_bool m_IsOpen = true;

            Game* m_Game = nullptr;

            ecs::MainScene* m_MainScene = nullptr;

            core::Boolean m_UseMSAA = core::K_TRUE;
            core::usize m_MSAASampleCount = 4;

            render::FontStorage* m_FontStorage = nullptr;
            render::MaterialStorage* m_MaterialStorage = nullptr;
            render::TextureStorage* m_TextureStorage = nullptr;

            render::Canvas* m_Canvas = nullptr;
            render::Renderer* m_Renderer = nullptr;

            anim::Animator* m_Animator = nullptr;
            anim::SkeletStorage* m_SkeletStorage = nullptr;
            anim::AnimStorage* m_AnimStorage = nullptr;

            audio::AudioSystem* m_AudioSystem = nullptr;
            audio::AudioStorage* m_AudioStorage = nullptr;

            render::SSAOPass* m_SSAOPass = nullptr;
            render::FXAAPass* m_FXAAPass = nullptr;
        };

    }

}

xpe::core::Application* CreateApplication();