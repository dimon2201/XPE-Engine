#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/buffers/transform_buffer.h>
#include <rendering/buffers/text_buffer.h>
#include <rendering/geometry/geometry.h>

namespace xpe {

    namespace render {

        using namespace core;

        class Font;

        class ENGINE_API TextPass : public RenderPass
        {

        public:
            TextPass(eType type, const vector<RenderPassBinding>& bindings);

        protected:
            void DrawText(const Transform& transform, const string& text, Font& font);

            TextBuffer m_TextBuffer;
            TransformBuffer m_TransformBuffer;
            Geometry m_Quad;
        };

    }

}