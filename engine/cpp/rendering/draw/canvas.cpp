#include <rendering/render_passes/canvas.hpp>

#include <core/input.h>

namespace xpe {

    namespace render {

        Canvas::Canvas(s32 width, s32 height, Shader* shader) : m_Shader(shader)
        {
            Viewport viewport;
            viewport.Width = width;
            viewport.Height = height;
            m_ViewportBuffer.Add(viewport);
            m_ViewportBuffer.Flush();

            CreateRenderTarget(width, height);
            CreatePresentTarget();
            CreatePresentSampler();

            AddWindowFrameResized(Canvas, 1);
        }

        Canvas::Canvas(const glm::ivec2& size, Shader* shader) : Canvas(size.x, size.y, shader) {}

        Canvas::~Canvas()
        {
            context::FreeSampler(m_PresentSampler);
        }

        void Canvas::Clear(const glm::vec4& color)
        {
            context::BindTextureSlot(0);
            m_RenderTarget->Bind();
            m_RenderTarget->ClearColor(m_BoundTargetIndex, color);
            m_RenderTarget->ClearDepth(1.0f);
        }

        void Canvas::Present()
        {
            context::BindRenderTarget(m_PresentTarget->ColorViews, m_PresentTarget->DepthStencilView, m_PresentTarget->Viewports);
            context::BindShader(*m_Shader);
            context::BindSampler(m_PresentSampler);
            m_RenderTarget->BindColor(m_BoundTargetIndex);

            context::ClearColorTarget(m_PresentTarget->ColorViews[m_BoundTargetIndex], m_ClearColor);

            context::DrawQuad();

            context::Present();
        }

        void Canvas::WindowFrameResized(s32 width, s32 height)
        {
            context::ResizeSwapchain(*m_PresentTarget, width, height);
        }

        void Canvas::CreateRenderTarget(int width, int height)
        {
            Texture* color = new Texture();
            color->Width = width;
            color->Height = height;
            color->Format = eTextureFormat::RGBA8;
            color->InitializeData = false;
            color->EnableRenderTarget = true;
            color->Init();

            Texture* depth = new Texture();
            depth->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            depth->Width = width;
            depth->Height = height;
            depth->Format = eTextureFormat::R32_TYPELESS;
            depth->InitializeData = false;
            depth->EnableRenderTarget = true;
            depth->Init();

            m_RenderTarget.Create(
                vector<Texture*> { color },
                depth,
                m_ViewportBuffer.GetList()
            );
        }

        void Canvas::CreatePresentTarget()
        {
            m_PresentTarget.Create(
                    vector<void*> { context::SwapchainTargetView },
                    m_ViewportBuffer.GetList()
            );
        }

        void Canvas::CreatePresentSampler()
        {
            m_PresentSampler.Filter   = TextureSampler::eFilter::MIN_MAG_MIP_POINT;
            m_PresentSampler.AddressU = TextureSampler::eAddress::CLAMP;
            m_PresentSampler.AddressV = TextureSampler::eAddress::CLAMP;
            m_PresentSampler.AddressW = TextureSampler::eAddress::CLAMP;
            m_PresentSampler.AnisotropyLevel = 1;

            context::CreateSampler(m_PresentSampler);
        }

    }

}
