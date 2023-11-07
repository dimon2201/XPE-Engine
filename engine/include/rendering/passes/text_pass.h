#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/buffers/text_buffer.h>
#include <rendering/geometry/geometry.h>

namespace xpe {

    namespace render {

        using namespace core;

        class sFont;

        class ENGINE_API cTextPass : public cRenderPass
        {

        public:
            cTextPass(eType type, const vector<sRenderPassBinding>& bindings);

        protected:
            void DrawText(const sTransform& transform, const string& text, sFont& font);

            sCharBuffer m_CharBuffer;
            sTextBuffer m_TextBuffer;
            sGeometry m_Quad;
        };

    }

}