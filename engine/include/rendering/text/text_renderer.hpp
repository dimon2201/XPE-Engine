#pragma once

namespace xpe
{
    namespace ecs
    {
        struct TransformComponent;
        struct TextComponent;
    }

    namespace render
    {

        using namespace core;
        using namespace ecs;

        class Canvas;
        class InputLayout;
        class Pipeline;
        class TextBatchManager;
        class Font;

        class ENGINE_API TextRenderer : public Object
        {

        public:
            static void Init(TextBatchManager* batchManager, Canvas* canvas);
            static void Free();

            static void Draw(TextComponent& text, TransformComponent& transform, Font& font);

        private:
            static TextBatchManager* s_BatchManager;
            static InputLayout* s_InputLayout;
            static Pipeline* s_Pipeline;
        };
    }
}