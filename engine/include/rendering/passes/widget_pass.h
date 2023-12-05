#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/buffers/widget_buffers.h>
#include <rendering/geometry/geometry_manager.h>
#include <rendering/widget_manager.h>

namespace xpe {

    namespace render {

        using namespace core;

        class sFont;

        class ENGINE_API cWidgetPass : public cRenderPass
        {

        public:
            cWidgetPass(const vector<sRenderPassBinding>& bindings);
            void DrawUI(cScene* scene) override final;

        private:
            void DrawWidgets();
            void UpdateWidget(
                    sTransform transform,
                    eSpace space,
                    const glm::vec4& color,
                    bool enableTexture,
                    const sAtlas::sLocation& atlasLocation,
                    bool fillFrame,
                    bool pressed,
                    const std::function<void(sWidgetData&)>& pressedCallback,
                    const std::function<void(sWidgetData&)>& hoveredCallback
            );
            bool IsHovered2D(const glm::mat4& modelMatrix, const glm::vec2& mousePos);
            bool IsHovered3D(const glm::mat4& modelMatrix, const glm::vec2& mousePos);

            sGeometry m_Quad;
            sGeometryInfo m_QuadInfo;
            sWidgetBuffer m_WidgetBuffer;
            glm::mat4 m_Projection2D;
            glm::mat4 m_Projection3D;
        };

        class ENGINE_API cTextPass : public cRenderPass
        {

        public:
            cTextPass(const vector<sRenderPassBinding>& bindings);
            ~cTextPass();
            void DrawUI(cScene* scene) override final;

        private:
            void DrawTexts(sTexture& fontAtlas);
            void UpdateText(
                    sTransform transform,
                    eSpace space,
                    u32 textIndex,
                    sFont& font,
                    const string& textString,
                    const glm::vec4& textColor,
                    bool fillFrame
            );
            bool IsHovered2D(const glm::mat4& modelMatrix, const glm::vec2& mousePos);
            bool IsHovered3D(const glm::mat4& modelMatrix, const glm::vec2& mousePos);

            sGeometry m_Quad;
            sGeometryInfo m_QuadInfo;
            sCharBuffer m_CharBuffer;
            sTextBuffer m_TextBuffer;
            sSampler m_FontSampler;
            glm::mat4 m_Projection2D;
            glm::mat4 m_Projection3D;
        };

    }

}