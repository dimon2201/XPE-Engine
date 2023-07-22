#include <rendering/canvas.hpp>

namespace xpe {

    namespace render {

        Canvas::Canvas(s32 width, s32 height, Context* context) {
            m_Context = context;
            m_Size.x = width;
            m_Size.y = height;

            m_RenderTarget.Width = width;
            m_RenderTarget.Height = height;

            m_ColorTexture.Width = width;
            m_ColorTexture.Height = height;
            m_ColorTexture.Format = Texture::eFormat::RGB8;
            m_ColorTexture.InitializeData = false;
            m_ColorTexture.BindRenderTarget = true;
            m_RenderTarget.ColorTexture = &m_ColorTexture;

            m_DepthTexture.Width = width;
            m_DepthTexture.Height = height;
            m_RenderTarget.DepthTexture = &m_DepthTexture;

            m_Context->CreateRenderTarget(m_RenderTarget);

            m_Shader = *ShaderManager::Builder()
                .AddVertexStageFromFile("engine_shaders/canvas.vs")
                .AddPixelStageFromFile("engine_shaders/canvas.ps")
                .Build("canvas");

            m_Shader.PrimitiveTopology = ePrimitiveTopology::TRIANGLE_STRIP;
        }

        Canvas::Canvas(const glm::ivec2& size, Context* context) : Canvas(size.x, size.y, context) {}

        Canvas::~Canvas() {
            m_Context->FreeRenderTarget(m_RenderTarget);
        }

        void Canvas::Clear(const glm::vec4& color) {
            Context& context = *m_Context;

            context.BindViewport(glm::ivec4(0.0f, 0.0f, m_RenderTarget.Width, m_RenderTarget.Height), 0.0f, 1.0f);
            context.BindTextureSlot(0);
            context.BindRenderTarget(m_RenderTarget.ColorTargetView, m_RenderTarget.DepthTargetView);

            context.ClearColorTarget(color);
            context.ClearDepthTarget(1.0f);
        }

        void Canvas::Present() {
            Context& context = *m_Context;

            glm::ivec2 swapChainDimensions = context.GetSwapChainDimensions();

            context.BindSwapChainTarget();
            context.BindViewport(glm::ivec4(0.0f, 0.0f, swapChainDimensions.x, swapChainDimensions.y), 0.0f, 1.0f);
            context.BindShader(&m_Shader);
            context.BindTexture(m_RenderTarget.ColorTexture);

            context.ClearColorTarget({ 0.0f, 0.0f, 0.0f, 1.0f });

            context.DrawQuad();

            context.Present();
        }

    }

}
