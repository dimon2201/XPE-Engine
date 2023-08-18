#pragma once

namespace xpe {

    namespace render
    {
        class Canvas;
        class Renderer;
        class GeometryStorage;
        class TextureStorage;
        class MaterialStorage;
        class FontStorage;
    }

    namespace anim
    {
        class SkeletStorage;
        class SkinStorage;
        class AnimStorage;
        class Animator;
    }

    namespace ecs
    {
        class MainScene;
    }

    namespace core {

        class ENGINE_API Game : public Object
        {

        public:
            Time* CPUTime;
            Time* DeltaTime;
            Time* CurrentTime;

            AppConfig* Config;

            ecs::MainScene* MainScene;

            render::Canvas* Canvas;
            render::Renderer* Renderer;
            render::FontStorage* FontStorage;
            render::GeometryStorage* GeometryStorage;
            render::MaterialStorage* MaterialStorage;
            render::TextureStorage* TextureStorage;

            anim::Animator* Animator;
            anim::SkeletStorage* SkeletStorage;
            anim::SkinStorage* SkinStorage;
            anim::AnimStorage* AnimStorage;

            virtual void Init() {}
            virtual void Update() {}
            virtual void Free() {}

        };

    }

}