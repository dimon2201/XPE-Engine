#pragma once

#include <string>

namespace xpe
{
    namespace ecs
    {
        struct TransformComponent;
        struct TextComponent;
    }

    namespace render
    {
        class Context;
        class Canvas;
        class InputLayout;
        class Pipeline;
        class TextBatchManager;
    }

    namespace text
    {
        class Font;

        class ENGINE_API TextRenderer : public core::Object
        {

        public:
            static void Init(xpe::render::Context* context, xpe::render::TextBatchManager* batchManager, xpe::render::Canvas* canvas);
            static void Free();

            static TextRenderer& Get() {
                return *s_Instance;
            }

            void Draw(Font* font, const xpe::ecs::TransformComponent* transform, const xpe::ecs::TextComponent* text);

        private:
            static TextRenderer* s_Instance;
            xpe::render::Context* m_Context;
            xpe::render::TextBatchManager* m_BatchManager;
            xpe::render::InputLayout* m_InputLayout;
            xpe::render::Pipeline* m_Pipeline;
        };
    }
}