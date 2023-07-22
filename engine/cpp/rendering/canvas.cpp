#include <rendering/canvas.hpp>

#include <core/user_input.hpp>

namespace xpe {

    namespace render {

        Canvas::Canvas(s32 width, s32 height, Context* context) {
            m_Context = context;
            m_Viewport.Width = width;
            m_Viewport.Height = height;

            CreateRenderTarget(width, height);
            CreatePresentTarget();
            CreatePresentSampler();

            m_Shader = ShaderManager::CreateShader("canvas");
            ShaderManager::AddVertexStageFromFile(m_Shader, "engine_shaders/canvas.vs");
            ShaderManager::AddPixelStageFromFile(m_Shader, "engine_shaders/canvas.ps");
            ShaderManager::BuildShader(m_Shader);

            Input::WindowFrameResizedEvents->AddEvent(this, OnWindowFrameResized<Canvas>, 1);
        }

        Canvas::Canvas(const glm::ivec2& size, Context* context) : Canvas(size.x, size.y, context) {}

        Canvas::~Canvas() {
            m_Context->FreeRenderTarget(m_RenderTarget);
            m_Context->FreeSampler(&m_PresentSampler);
            FreePresentTarget();
        }

        void Canvas::Clear(const glm::vec4& color) {
            Context& context = *m_Context;

            context.BindViewport(&m_Viewport);
            context.BindTextureSlot(0);
            context.BindRenderTarget(m_RenderTarget.ColorTargetView, m_RenderTarget.DepthTargetView);

            context.ClearColorTarget(color);
            context.ClearDepthTarget(1.0f);
        }

        void Canvas::Present() {
            Context& context = *m_Context;

            context.BindRenderTarget(m_PresentTarget.ColorTargetView, m_PresentTarget.DepthTargetView);
            context.BindViewport(&m_Viewport);
            context.BindShader(m_Shader);
            context.BindTexture(m_RenderTarget.ColorTexture);
            context.BindSampler(&m_PresentSampler);

            context.ClearColorTarget(m_ClearColor);

            context.DrawQuad();

            context.Present();
        }

        void Canvas::WindowFrameResized(int width, int height) {
            m_Context->ResizeRenderTarget(m_RenderTarget, width, height);
            m_Context->ResizeSwapchain(m_PresentTarget, width, height);
        }

        void Canvas::CreateRenderTarget(int width, int height) {
            m_ColorTexture.Width = width;
            m_ColorTexture.Height = height;
            m_ColorTexture.Format = Texture::eFormat::RGB8;
            m_ColorTexture.InitializeData = false;
            m_ColorTexture.EnableRenderTarget = true;

            m_DepthTexture.Width = width;
            m_DepthTexture.Height = height;

            m_RenderTarget.ColorTexture = &m_ColorTexture;
            m_RenderTarget.DepthTexture = &m_DepthTexture;

            m_Context->CreateRenderTarget(m_RenderTarget);
        }

        void Canvas::CreatePresentTarget() {
            m_PresentTarget.ColorTargetView = m_Context->GetSwapchainTargetView();
        }

        void Canvas::CreatePresentSampler() {
            m_PresentSampler.Filter   = TextureSampler::eFilter::MIN_MAG_MIP_POINT;
            m_PresentSampler.AddressU = TextureSampler::eAddress::CLAMP;
            m_PresentSampler.AddressV = TextureSampler::eAddress::CLAMP;
            m_PresentSampler.AddressW = TextureSampler::eAddress::CLAMP;
            m_PresentSampler.AnisotropyLevel = 1;

            m_Context->CreateSampler(m_PresentSampler);
        }

        void Canvas::FreePresentTarget() {
            m_Context->FreeRenderTarget(m_PresentTarget);
        }

    }

}
