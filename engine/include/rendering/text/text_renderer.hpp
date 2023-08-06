#pragma once

namespace xpe
{
    namespace ecs
    {
        struct TransformComponent;
        struct Text2DComponent;
        struct Text3DComponent;
    }

    namespace math
    {
        class Quad;
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
        class Shader;

        class ENGINE_API TextRenderer : public Object
        {

        public:
            void DrawText(const string& usid, string& text, TransformComponent& transform, Font& font);

        protected:
            void Init(TextBatchManager* batchManager, Canvas* canvas, Shader* shader);
            void Free();

        protected:
            TextBatchManager* m_BatchManager;
            InputLayout* m_InputLayout;
            Pipeline* m_Pipeline;
            math::Quad* m_Quad;
        };

        class ENGINE_API Text2DRenderer : public TextRenderer
        {

        public:
            Text2DRenderer(TextBatchManager* batchManager, Canvas* canvas);
            ~Text2DRenderer();

        public:
            void Draw(Text2DComponent& textComponent, TransformComponent& transform, Font& font);

        };

        class ENGINE_API Text3DRenderer : public TextRenderer
        {

        public:
            Text3DRenderer(TextBatchManager* batchManager, Canvas* canvas);
            ~Text3DRenderer();

        public:
            void Draw(Text3DComponent& textComponent, TransformComponent& transform, Font& font);

        };

    }
}