#include <rendering/renderer.h>
#include <rendering/passes/render_pass.h>
#include <rendering/monitor.h>
#include <rendering/buffers/camera_buffer.h>
#include <rendering/buffers/light_buffers.h>
#include <rendering/shadow/shadow.h>
#include <rendering/core/viewport.h>
#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        Renderer::Renderer(Viewport* viewport, core::Boolean useMSAA, core::usize msaaSampleCount)
        {
            context::Init();
            ShaderManager::Init();
            Monitor::Init();
            Shadow::Init();

            CameraBuffer = new render::CameraBuffer();

            DirectLightBuffer = new render::DirectLightBuffer();
            DirectLightBuffer->Reserve(1000);

            PointLightBuffer = new render::PointLightBuffer();
            PointLightBuffer->Reserve(1000);

            SpotLightBuffer = new render::SpotLightBuffer();
            SpotLightBuffer->Reserve(1000);

            m_UseMSAA = useMSAA;
            m_MSAASampleCount = msaaSampleCount;

            Texture* color = new Texture();
            color->Width = viewport->Width;
            color->Height = viewport->Height;
            color->Format = eTextureFormat::RGBA8;
            color->SampleCount = m_UseMSAA == core::K_TRUE ? m_MSAASampleCount : 1;
            color->InitializeData = false;
            color->EnableRenderTarget = true;
            color->Init();

            Texture* accum = new Texture();
            accum->Width = viewport->Width;
            accum->Height = viewport->Height;
            accum->Format = eTextureFormat::RGBA16;
            accum->SampleCount = m_UseMSAA == core::K_TRUE ? m_MSAASampleCount : 1;
            accum->InitializeData = false;
            accum->EnableRenderTarget = true;
            accum->Init();

            Texture* reveal = new Texture();
            reveal->Width = viewport->Width;
            reveal->Height = viewport->Height;
            reveal->Format = eTextureFormat::R8;
            reveal->SampleCount = m_UseMSAA == core::K_TRUE ? m_MSAASampleCount : 1;
            reveal->InitializeData = false;
            reveal->EnableRenderTarget = true;
            reveal->Init();

            Texture* position = new Texture();
            position->Width = viewport->Width;
            position->Height = viewport->Height;
            position->Format = eTextureFormat::RGBA32;
            position->SampleCount = m_UseMSAA == core::K_TRUE ? m_MSAASampleCount : 1;
            position->InitializeData = false;
            position->EnableRenderTarget = true;
            position->Init();

            Texture* normal = new Texture();
            normal->Width = viewport->Width;
            normal->Height = viewport->Height;
            normal->Format = eTextureFormat::RGBA16;
            normal->SampleCount = m_UseMSAA == core::K_TRUE ? m_MSAASampleCount : 1;
            normal->InitializeData = false;
            normal->EnableRenderTarget = true;
            normal->Init();

            Texture* depth = new Texture();
            depth->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            depth->Width = viewport->Width;
            depth->Height = viewport->Height;
            depth->Format = eTextureFormat::R32_TYPELESS;
            depth->SampleCount = m_UseMSAA == core::K_TRUE ? m_MSAASampleCount : 1;
            depth->InitializeData = false;
            depth->EnableRenderTarget = true;
            depth->Init();

            m_Main = new RenderTarget({ color, accum, reveal, position, normal }, depth, *viewport);
        }

        Renderer::~Renderer()
        {
            delete CameraBuffer;
            delete DirectLightBuffer;
            delete PointLightBuffer;
            delete SpotLightBuffer;

            for (RenderPass* pass : m_RenderPasses) {
                delete pass;
            }
            m_RenderPasses.clear();

            for (auto texture : m_Main->Colors) {
                delete texture;
            }
            delete m_Main->DepthStencil;

            Shadow::Free();
            Monitor::Free();
            ShaderManager::Free();
            context::Free();
        }

        void Renderer::RemoveRenderPass(RenderPass* pass)
        {
            auto it = std::find(m_RenderPasses.begin(), m_RenderPasses.end(), pass);
            if (it != m_RenderPasses.end()) {
                m_RenderPasses.erase(it);
                delete it.operator->();
            }
        }

        void Renderer::Render(Scene* scene)
        {
            FlushLights(scene);
            for (RenderPass* rp : m_RenderPasses)
            {
                rp->Update(scene);
                rp->Bind();
                rp->Draw(scene);
                rp->Unbind();
            }
        }

        void Renderer::ClearRenderTargets()
        {
            for (auto pass : m_RenderPasses)
            {
                for (core::s32 i = 0; i < pass->GetRenderTarget()->Colors.size(); i++) {
                    pass->GetRenderTarget()->ClearColor(i, glm::vec4(0.0f));
                }

                pass->GetRenderTarget()->ClearDepth(1.0f);
            }
        }

        RenderTarget* Renderer::GetRenderTarget()
        {
            return m_Main;
        }

        void Renderer::FlushLights(Scene* scene)
        {
            this->DirectLightBuffer->Clear();
            scene->EachComponent<DirectLightComponent>([this](DirectLightComponent* component)
            {
                this->DirectLightBuffer->Add(*component);
            });
            this->DirectLightBuffer->Flush();

            this->PointLightBuffer->Clear();
            scene->EachComponent<PointLightComponent>([this](PointLightComponent* component)
            {
                this->PointLightBuffer->Add(*component);
            });
            this->PointLightBuffer->Flush();

            this->SpotLightBuffer->Clear();
            scene->EachComponent<SpotLightComponent>([this](SpotLightComponent* component)
            {
                this->SpotLightBuffer->Add(*component);
            });
            this->SpotLightBuffer->Flush();
        }

    }

}