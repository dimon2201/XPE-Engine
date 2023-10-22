#pragma once

namespace xpe {

    namespace render
    {
        class Canvas;
        class Renderer;
        class TextureStorage;
        class MaterialStorage;
        class FontStorage;
        class RenderTarget;
    }

    namespace anim
    {
        class SkeletStorage;
        class AnimStorage;
        class Animator;
    }

    namespace audio
    {
        class AudioSystem;
        class AudioStorage;
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

            virtual void InitRenderer();

            std::atomic_bool m_IsOpen = true;

            ecs::MainScene* m_MainScene = nullptr;

            render::FontStorage* m_FontStorage = nullptr;
            render::MaterialStorage* m_MaterialStorage = nullptr;
            render::TextureStorage* m_TextureStorage = nullptr;

            render::Canvas* m_Canvas = nullptr;
            render::Renderer* m_Renderer = nullptr;

            anim::Animator* m_Animator = nullptr;
            anim::SkeletStorage* m_SkeletStorage = nullptr;
            anim::AnimStorage* m_AnimStorage = nullptr;

            audio::AudioManager* m_AudioSystem = nullptr;
            audio::AudioStorage* m_AudioStorage = nullptr;

            render::RenderTarget* MainRT = nullptr;
            render::RenderTarget* SsaoRT = nullptr;
        };

    }

}

xpe::core::Application* CreateApplication();