#include <core/types.hpp>
#include <ecs/components.hpp>
#include <rendering/camera/camera_manager.h>
#include <rendering/transforming/transforming.h>
#include <rendering/materials/material.h>
#include <rendering/lighting/light_manager.h>
#include <rendering/text/text_renderer.hpp>
#include <rendering/text/ttf_manager.hpp>
#include <rendering/text/text_batching.hpp>
#include <rendering/core/shader.h>
#include <rendering/dx11/d3d11_context.hpp>
#include <rendering/draw/canvas.hpp>
#include <geometry/quad_geometry.h>

xpe::text::TextRenderer* xpe::text::TextRenderer::s_Instance = nullptr;

void xpe::text::TextRenderer::Init(xpe::render::Context* context, xpe::render::TextBatchManager* batchManager, xpe::render::Canvas* canvas) {
    s_Instance = new TextRenderer();
    s_Instance->m_Context = context;
    s_Instance->m_BatchManager = batchManager;
    s_Instance->m_InputLayout = new render::InputLayout();
    s_Instance->m_Pipeline = new render::Pipeline();

    // Setup pipeline
    s_Instance->m_Pipeline->VSBuffers.emplace_back(render::CameraManager::GetBuffer());
    s_Instance->m_Pipeline->VSBuffers.emplace_back(render::TransformManager::GetBuffer());

    s_Instance->m_Pipeline->Shader = render::ShaderManager::CreateShader("text");
    xpe::render::ShaderManager::AddVertexStageFromFile(s_Instance->m_Pipeline->Shader, "shaders/text.vs");
    xpe::render::ShaderManager::AddPixelStageFromFile(s_Instance->m_Pipeline->Shader, "shaders/text.ps");
    xpe::render::ShaderManager::BuildShader(s_Instance->m_Pipeline->Shader);

    s_Instance->m_InputLayout->Format = render::Vertex3D::Format;
    s_Instance->m_Pipeline->InputLayout = *s_Instance->m_InputLayout;

    s_Instance->m_Pipeline->RenderTarget = canvas->GetRenderTarget();
    s_Instance->m_Pipeline->DepthStencilState.UseDepthTest = core::K_TRUE;
    s_Instance->m_Pipeline->BlendState.UseBlending = core::K_TRUE;

    s_Instance->m_Pipeline->Textures.push_back(nullptr);

    // Init pipeline
    context->CreatePipeline(*s_Instance->m_Pipeline);

    // Store quad geometry to batch manager
    render::Quad quad;
    s_Instance->m_BatchManager->StoreGeometryIndexed("__TextQuad", quad, 1);
    
}

void xpe::text::TextRenderer::Free() {
    delete s_Instance;
}

void xpe::text::TextRenderer::Draw(xpe::text::Font* font, const xpe::ecs::TransformComponent* transform, const xpe::ecs::TextComponent* text)
{
    xpe::core::usize charsCount = text->Text.size();
    const char* chars = text->Text.c_str();

    // Bind font atlas
    font->Atlas.Slot = 0;
    m_Pipeline->Textures[0] = &font->Atlas;

    // Prepare text batch manager for drawing
    m_BatchManager->ClearInstances("__TextQuad");
    m_BatchManager->ReserveInstances("__TextQuad", charsCount);

    xpe::render::TransformManager::AddTransform(*(xpe::ecs::TransformComponent*)transform);
    xpe::render::TransformManager::FlushTransforms();

    // Add instances to text batch manager
    core::f32 tempAdvanceY = 0.0f;
    glm::vec2 advance = glm::vec2(0.0f);
    for (core::usize i = 0; i < charsCount; i++)
    {
        char character = chars[i];
        auto it = font->AlphaBet.find(character);
        if (it == font->AlphaBet.end()) {
            continue;
        }

        Font::Glyph glyph = it->second;

        xpe::render::TextGlyphInstance instance;
        instance.TransformIndex = transform->Index;
        instance.GlyphSize = font->GlyphSize;
        instance.Width = glyph.Width;
        instance.Height = glyph.Height;
        instance.Left = glyph.Left;
        instance.Top = glyph.Top;
        instance.AdvanceX = advance.x;
        instance.AdvanceY = advance.y;
        instance.AtlasXOffset = glyph.AtlasXOffset;
        instance.AtlasYOffset = glyph.AtlasYOffset;
        advance.x += (glyph.AdvanceX / 64.0f) * transform->Scale.x;

        if (character == '\n')
        {
            advance.x = 0.0f;
            advance.y = (tempAdvanceY + font->GlyphNewLineExtraOffset)*transform->Scale.y;
            tempAdvanceY = 0.0f;
        }
        else
        {
            if (instance.Height > advance.y) {
                tempAdvanceY = instance.Height;
            }

            m_BatchManager->AddInstance("__TextQuad", instance);
        }
    }

    m_BatchManager->FlushInstances("__TextQuad");

    // Draw glyphs
    m_Context->BindPipeline(m_Pipeline);
    m_BatchManager->DrawAll();

    xpe::render::TransformManager::ClearTransforms();
}