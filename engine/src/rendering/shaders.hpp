#pragma once

#include <rendering/core/shader.hpp>
#include <rendering/buffers.hpp>
#include <rendering/geometry/geometry.hpp>
#include <rendering/font/font.hpp>

namespace xpe {

    namespace render {

        class ENGINE_API cInstancingShader : public cDefaultShader
        {

        public:
            cInstancingShader(eCategory category, const string& name);

        protected:
            void DrawInstanced(
                    cScene* scene,
                    const sGeometryInfo& geometryInfo,
                    const std::function<void(EntityID entityId, sRenderInstance&)>& callback = [](EntityID, sRenderInstance&) {}
            );

            sInstanceBuffer m_InstanceBuffer;
        };

        class ENGINE_API cOpaqueShader : public cInstancingShader
        {
        public:
            cOpaqueShader(const string& name);

            void Draw(ecs::cScene *scene) override;
        };

        class ENGINE_API cTransparentShader : public cInstancingShader
        {
        public:
            cTransparentShader(const string& name);

            void Draw(ecs::cScene *scene) override;
        };

        class ENGINE_API cShadowShader : public cInstancingShader
        {
        public:
            cShadowShader(const string& name);

        protected:
            void DrawShadow(cScene* scene, u32 lightIndex);
            void InitPrepass() override;
        };

        class ENGINE_API cDirectionalShadowShader : public cShadowShader
        {
        public:
            cDirectionalShadowShader(const string& name);
            void Draw(cScene* scene) override;
        };

        class ENGINE_API cPointShadowShader : public cShadowShader
        {
        public:
            cPointShadowShader(const string& name);
            void Draw(cScene* scene) override;
        };

        class ENGINE_API cSpotShadowShader : public cShadowShader
        {
        public:
            cSpotShadowShader(const string& name);
            void Draw(cScene* scene) override;
        };

        class ENGINE_API cSkyboxShader : public cDefaultShader
        {
        public:
            cSkyboxShader(const string& name);
            ~cSkyboxShader() override;

            void Draw(cScene* scene) override;

        protected:
            void InitOpaque() override;
        };

        class ENGINE_API cCompositeTransparentShader : public cDefaultShader
        {
        public:
            cCompositeTransparentShader(const string& name, u32 sampleCount);

            void Draw(ecs::cScene *scene) override;

        protected:
            void InitPostFX() override;
        };

        class ENGINE_API cSsaoShader : public cDefaultShader
        {
        public:
            cSsaoShader(const string& name, u32 sampleCount);
            ~cSsaoShader();

            void Draw(ecs::cScene *scene) override;
        };

        class ENGINE_API cFinalShader : public cDefaultShader
        {
        public:
            cFinalShader(const string& name, u32 sampleCount);

            void Draw(ecs::cScene *scene) override;
        };

        class ENGINE_API cWidgetShader : public cDefaultShader
        {

        public:
            cWidgetShader(const string& name);
            void Draw(cScene* scene) override;

        private:
            void DrawWidgets();
            void UpdateWidget2D(
                    glm::mat4 projection,
                    sTransform transform,
                    const glm::vec4& color,
                    bool enableTexture,
                    const sAtlas::sLocation& atlasLocation,
                    bool fillFrame,
                    bool pressed,
                    const std::function<void(sWidgetData&)>& pressedCallback,
                    const std::function<void(sWidgetData&)>& hoveredCallback
            );
            void UpdateWidget3D(
                    glm::mat4 projection,
                    sTransform transform,
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

        class ENGINE_API cTextShader : public cDefaultShader
        {

        public:
            cTextShader(const string& name);
            ~cTextShader();

            void Draw(cScene* scene) override;

        private:
            void DrawTexts(sTexture& fontAtlas);
            void UpdateText2D(
                    glm::mat4 projection,
                    sTransform transform,
                    u32 textIndex,
                    sFont& font,
                    const string& textString,
                    const glm::vec4& textColor,
                    bool fillFrame
            );
            void UpdateText3D(
                    glm::mat4 projection,
                    sTransform transform,
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
            glm::mat4 m_Projection2D;
            glm::mat4 m_Projection3D;
        };

    }

}