#include <rendering/draw/canvas.hpp>

#include <core/user_input.hpp>

namespace xpe {

    namespace render {

        Canvas::Canvas(s32 width, s32 height) {
            m_ViewportBuffer.Item.Width = width;
            m_ViewportBuffer.Item.Height = height;
            m_ViewportBuffer.Flush();

            CreateRenderTarget(width, height);
            CreatePresentTarget();
            CreatePresentSampler();

            m_Shader = ShaderManager::CreateShader("canvas");
            ShaderManager::AddVertexStageFromFile(m_Shader, "engine_shaders/canvas.vs");
            ShaderManager::AddPixelStageFromFile(m_Shader, "engine_shaders/canvas.ps");
            ShaderManager::BuildShader(m_Shader);

            Input::WindowFrameResizedEvents->AddEvent(this, OnWindowFrameResized<Canvas>, 1);
        }

        Canvas::Canvas(const glm::ivec2& size) : Canvas(size.x, size.y) {}

        Canvas::~Canvas() {
            context::FreeRenderTarget(m_RenderTarget);
            context::FreeSampler(&m_PresentSampler);
            FreePresentTarget();
        }

        void Canvas::Clear(const glm::vec4& color) {
            context::BindViewport(&m_ViewportBuffer.Item);
            context::BindTextureSlot(0);
            context::BindRenderTarget(m_RenderTarget.ColorTargetView, m_RenderTarget.DepthTargetView);

            context::ClearColorTarget(color);
            context::ClearDepthTarget(1.0f);
        }

        void Canvas::Present() {
            context::BindRenderTarget(m_PresentTarget.ColorTargetView, m_PresentTarget.DepthTargetView);
            context::BindViewport(&m_ViewportBuffer.Item);
            context::BindShader(m_Shader);
            context::BindTexture(m_RenderTarget.ColorTexture);
            context::BindSampler(&m_PresentSampler);

            context::ClearColorTarget(m_ClearColor);

            context::DrawQuad();

            context::Present();
        }

        void Canvas::WindowFrameResized(int width, int height) {
            context::ResizeRenderTarget(m_RenderTarget, width, height);
            context::ResizeSwapchain(m_PresentTarget, width, height);
            m_ViewportBuffer.Flush();
        }

        void Canvas::CreateRenderTarget(int width, int height) {
            m_ColorTexture.Width = width;
            m_ColorTexture.Height = height;
            m_ColorTexture.Format = Texture::eFormat::RGBA8;
            m_ColorTexture.InitializeData = false;
            m_ColorTexture.EnableRenderTarget = true;

            m_DepthTexture.Width = width;
            m_DepthTexture.Height = height;

            m_RenderTarget.ColorTexture = &m_ColorTexture;
            m_RenderTarget.DepthTexture = &m_DepthTexture;

            context::CreateRenderTarget(m_RenderTarget);
        }

        void Canvas::CreatePresentTarget() {
            m_PresentTarget.ColorTargetView = context::SwapchainTargetView;
        }

        void Canvas::CreatePresentSampler() {
            m_PresentSampler.Filter   = TextureSampler::eFilter::MIN_MAG_MIP_POINT;
            m_PresentSampler.AddressU = TextureSampler::eAddress::CLAMP;
            m_PresentSampler.AddressV = TextureSampler::eAddress::CLAMP;
            m_PresentSampler.AddressW = TextureSampler::eAddress::CLAMP;
            m_PresentSampler.AnisotropyLevel = 1;

            context::CreateSampler(m_PresentSampler);
        }

        void Canvas::FreePresentTarget() {
            context::FreeRenderTarget(m_PresentTarget);
        }

    }

}
