#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/buffers/transform_buffer.h>
#include <rendering/buffers/text_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;

        class GeometryStorage;
        class Font;

        class ENGINE_API TextPass : public RenderPass
        {

        public:
            TextPass(
                const core::vector<RenderPassBinding>& bindings,
                GeometryStorage* geometryStorage,
                RenderTarget* output
            );
            ~TextPass() override;

        protected:
            void DrawText(const Transform& transform, const string& text, const Ref<Font>& font);

            TextBuffer m_TextBuffer;
            TransformBuffer m_TransformBuffer;
        };

    }

}