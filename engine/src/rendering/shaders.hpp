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
            ~cInstancingShader() {}

        protected:
            void DrawInstanced(cScene* scene, const sGeometryInfo& geometryInfo);

            cInstanceBuffer* m_InstanceBuffer;

        };

        class ENGINE_API cOpaqueShader : public cInstancingShader
        {

        public:
            cOpaqueShader(const string& name);
            ~cOpaqueShader() {}

            void Draw(ecs::cScene *scene) override;

        };

        class ENGINE_API cTransparentShader : public cInstancingShader
        {

        public:
            cTransparentShader(const string& name);
            ~cTransparentShader() {}

            void Draw(ecs::cScene *scene) override;

        };

        class ENGINE_API cDirectionalShadowShader : public cInstancingShader
        {

        public:
            cDirectionalShadowShader(const string& name);
            ~cDirectionalShadowShader() {}

            void Draw(cScene* scene) override;

        };

        class ENGINE_API cPointShadowShader : public cInstancingShader
        {

        public:
            cPointShadowShader(const string& name);
            ~cPointShadowShader() {}

            void Draw(cScene* scene) override;

        };

        class ENGINE_API cSpotShadowShader : public cInstancingShader
        {

        public:
            cSpotShadowShader(const string& name);
            ~cSpotShadowShader() {}

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
            ~cCompositeTransparentShader() {}

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
            ~cFinalShader() {}

            void Draw(ecs::cScene *scene) override;

        };

        class ENGINE_API cWidgetShader : public cDefaultShader
        {

        public:
            cWidgetShader(const string& name);
            ~cWidgetShader() {}

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
            cWidgetBuffer* m_WidgetBuffer;
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
            void DrawTexts(cTexture& fontAtlas);
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
            cCharBuffer* m_CharBuffer;
            cTextBuffer* m_TextBuffer;
            glm::mat4 m_Projection2D;
            glm::mat4 m_Projection3D;

        };

        class ENGINE_API cParticleComputeShader : public cComputeShader
        {

        public:
            cParticleComputeShader(const string& name, usize maxParticleCount, usize maxEmitterCount);
            ~cParticleComputeShader();

            void Draw(cScene* scene) override;

        };

        class ENGINE_API cParticleRenderShader : public cDefaultShader
        {

        public:
            cParticleRenderShader(const string& name);
            ~cParticleRenderShader();

            void Draw(cScene* scene) override;

        };

    }

}