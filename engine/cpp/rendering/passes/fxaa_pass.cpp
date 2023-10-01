#include "rendering/passes/fxaa_pass.hpp"
#include "rendering/core/context.hpp"
#include "rendering/storages/geometry_storage.h"
#include "ecs/scene.h"

namespace xpe
{
    namespace render
    {
        FXAAPass::FXAAPass(
            const core::vector<RenderPassBinding>& bindings,
            const glm::vec2& canvasSize,
            Viewport* canvasViewport
        ) : RenderPass(bindings, nullptr)
        {
            Texture* color = new Texture();
            color->Width = canvasSize.x;
            color->Height = canvasSize.y;
            color->Format = eTextureFormat::RGBA8;
            color->InitializeData = false;
            color->EnableRenderTarget = true;
            color->Init();

            Texture* depth = new Texture();
            depth->Type = Texture::eType::TEXTURE_2D_DEPTH_STENCIL;
            depth->Width = canvasSize.x;
            depth->Height = canvasSize.y;
            depth->Format = eTextureFormat::R32_TYPELESS;
            depth->InitializeData = false;
            depth->EnableRenderTarget = true;
            depth->Init();

            m_Pipeline->RenderTarget = new RenderTarget({ color }, depth, *canvasViewport);

            m_Sampler.Slot = 0;
            context::CreateSampler(m_Sampler);
            m_Pipeline->Samplers.emplace_back(&m_Sampler);

            context::CreatePipeline(*m_Pipeline);
        }

        FXAAPass::~FXAAPass()
        {
            delete m_Pipeline->RenderTarget;
        }

        void FXAAPass::Update(Scene* scene)
        {
        }

        void FXAAPass::Draw(Scene* scene)
        {
            context::DrawQuad();
        }
    }
}