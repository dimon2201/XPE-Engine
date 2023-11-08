#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/buffers/widget_buffer.h>
#include <rendering/buffers/text_buffer.h>
#include <rendering/geometry/geometry_manager.h>
#include <rendering/widget_manager.h>

namespace xpe {

    namespace render {

        using namespace core;

        class sFont;

        class ENGINE_API cWidgetPass : public cRenderPass
        {

        public:
            cWidgetPass(eType type, const vector<sRenderPassBinding>& bindings);

        protected:
            void DrawWidgets();
            void UpdateWidget(sWidget& widget, bool is2DSpace);
            bool IsHovered(const sWidgetData& widgetData, const glm::vec2& mousePos);
            void DrawText(const sText& text, sFont& font, const string& textString, const glm::vec3& textScale);

            sGeometry m_Quad;
            sWidgetBuffer m_WidgetBuffer;
            sCharBuffer m_CharBuffer;
            sTextBuffer m_TextBuffer;
        };

        class ENGINE_API cWidgetPass2D : public cWidgetPass
        {
        public:
            cWidgetPass2D(const vector<sRenderPassBinding>& bindings);
            void DrawUI(cScene* scene) override final;
        };

        class ENGINE_API cWidgetPass3D : public cWidgetPass
        {
        public:
            cWidgetPass3D(const vector<sRenderPassBinding>& bindings);
            void DrawUI(cScene* scene) override final;
        };

    }

}