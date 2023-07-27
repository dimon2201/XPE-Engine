#pragma once

#include <string>

namespace xpe
{
    namespace core
    {
        struct TransformComponent;
    }

    namespace render
    {
        class Context;
        class Canvas;
        class InputLayout;
        class Pipeline;
        class TextBatchManager;
    }

    namespace ttf
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

            void Draw(Font* font, const xpe::core::TransformComponent* transform, const char* chars);

        private:
            static TextRenderer* s_Instance;
            xpe::render::Context* m_Context;
            xpe::render::TextBatchManager* m_BatchManager;
            xpe::render::InputLayout* m_InputLayout;
            xpe::render::Pipeline* m_Pipeline;
        };
    }
}