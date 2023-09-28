#pragma once

#include <rendering/draw/render_pass.h>
#include <rendering/buffers/transform_buffer.h>
#include <rendering/buffers/text_buffer.h>

namespace xpe {

    namespace ecs
    {
        class Scene;
    }

    namespace render {

        using namespace core;
        using namespace math;
        using namespace ecs;

        class GeometryStorage;
        class Font;
        class TextBuffer;

        class ENGINE_API TextRenderPass : public RenderPass
        {

        public:
            TextRenderPass(
                const core::vector<RenderPassBinding>& bindings,
                RenderTarget* output,
                GeometryStorage* geometryStorage
            );
            ~TextRenderPass() override;

        protected:
            void DrawText(const Transform& transform, const string& text, const Ref<Font>& font);

            TextBuffer m_TextBuffer;
            TransformBuffer m_TransformBuffer;

        };

    }

}