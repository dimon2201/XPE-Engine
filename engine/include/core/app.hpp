#pragma once

namespace xpe {

    namespace render
    {
        class Canvas;
        class RenderSystem;
        class RenderTarget;
        class SSAOPass;
    }

    namespace anim
    {
        class AnimSystem;
    }

    namespace audio
    {
        class AudioSystem;
    }

    namespace ecs
    {
        class MainScene;
    }

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

            virtual void InitRenderPasses();

            std::atomic_bool m_IsOpen = true;

            ecs::MainScene* m_MainScene = nullptr;

            render::RenderSystem* m_RenderSystem = nullptr;
            anim::AnimSystem* m_AnimSystem = nullptr;
            audio::AudioSystem* m_AudioSystem = nullptr;

            render::Canvas* m_Canvas = nullptr;
            render::SSAOPass* m_SsaoPass = nullptr;
        };

    }

}

xpe::core::Application* CreateApplication();