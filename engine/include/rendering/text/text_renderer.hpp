#pragma once

#include <core/types.hpp>
#include <rendering/buffers/structure_buffer.h>
#include <string>

namespace xpe
{
    namespace ecs
    {
        struct TransformComponent;
        struct Text2DComponent;
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

            void Draw2D(Font* font, const xpe::ecs::TransformComponent* transform, const xpe::ecs::Text2DComponent* text);
            void Draw3D(Font* font, const xpe::ecs::TransformComponent* transform, const xpe::ecs::Text3DComponent* text);

        private:
            static TextRenderer* s_Instance;
            xpe::render::Context* m_Context;
            xpe::render::TextBatchManager* m_BatchManager;
            xpe::render::Canvas* m_Canvas;
            xpe::render::InputLayout* m_InputLayout;
            xpe::render::StructureBuffer<glm::vec4>* m_RTInfo;
            xpe::render::Pipeline* m_Pipeline2D;
            xpe::render::Pipeline* m_Pipeline3D;
        };
    }
}