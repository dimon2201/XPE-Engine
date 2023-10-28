#pragma once

namespace xpe
{
    namespace render
    {
        class Canvas;
        class RenderSystem;
        class RenderTarget;
        class FXAAPass;
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

    namespace physics
    {
        class PhysicsSystem;
    }

    namespace core {

        class ENGINE_API Application : public Object {

        public:
            Time CPUTime;              // delta CPU time ms = end time - begin time
            Time DeltaTime;            // delta CurrentTime ms = end time - begin time
            Time CurrentTime;
            AppConfig Config;

            Application() {}
            ~Application() {}

            void Run();

        protected:

            virtual void Init() {}
            virtual void Update() {}
            virtual void Free() {}
            virtual void Render();

            virtual void InitRenderPasses();
            virtual void ClearRenderPasses();

            void LockFPSFromConfig();

            std::atomic_bool m_IsOpen = true;

            ecs::MainScene* m_MainScene = nullptr;

            render::Canvas* m_Canvas = nullptr;

            render::Viewport m_Viewport;

            render::RenderSystem* m_RenderSystem = nullptr;
            anim::AnimSystem* m_AnimSystem = nullptr;
            audio::AudioSystem* m_AudioSystem = nullptr;
            physics::PhysicsSystem* m_PhysicsSystem = nullptr;

            render::SSAOPass* m_SsaoPass = nullptr;
            render::FXAAPass* m_FxaaPass = nullptr;
        };
    }
}

xpe::core::Application* CreateApplication();