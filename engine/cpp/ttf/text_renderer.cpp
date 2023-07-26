#include <ttf/text_renderer.hpp>
#include <ttf/ttf_manager.hpp>
#include <core/types.hpp>
#include <core/camera.h>
#include <rendering/transforming.h>
#include <rendering/materials/material.h>
#include <rendering/lighting/light_manager.h>
#include <rendering/shader.h>

xpe::ttf::TextRenderer* xpe::ttf::TextRenderer::s_Instance = nullptr;

void xpe::ttf::TextRenderer::Init(xpe::render::Context* context, xpe::render::TextBatchManager* manager, xpe::render::Canvas* canvas) {
    s_Instance = new TextRenderer();
    s_Instance->m_Context = context;
    s_Instance->m_Manager = manager;
    s_Instance->m_InputLayout = new xpe::render::InputLayout();
    s_Instance->m_Pipeline = new xpe::render::Pipeline();

    // Setup pipeline
    s_Instance->m_Pipeline->VSBuffers.emplace_back(xpe::core::CameraManager::GetCameraBuffer());
    s_Instance->m_Pipeline->VSBuffers.emplace_back(xpe::render::TransformManager::GetBuffer());

    s_Instance->m_Pipeline->Shader = xpe::render::ShaderManager::Builder()
        .AddVertexStageFromFile("shaders/text.vs")
        .AddPixelStageFromFile("shaders/text.ps")
        .Build("window");
    s_Instance->m_Pipeline->Shader->PrimitiveTopology = xpe::render::ePrimitiveTopology::TRIANGLE_LIST;

    s_Instance->m_InputLayout->PrimitiveTopology = xpe::render::ePrimitiveTopology::TRIANGLE_LIST;
    s_Instance->m_InputLayout->Format = xpe::render::Vertex3D::Format;
    s_Instance->m_Pipeline->InputLayout = *s_Instance->m_InputLayout;

    s_Instance->m_Pipeline->RenderTarget = canvas->GetRenderTarget();
    s_Instance->m_Pipeline->DepthStencilState.UseDepthTest = xpe::core::K_TRUE;

    s_Instance->m_Pipeline->Textures.push_back(nullptr);

    // Init pipeline
    context->CreateRenderPipeline(*s_Instance->m_Pipeline);
}

void xpe::ttf::TextRenderer::Free() {
    delete s_Instance;
}

void xpe::ttf::TextRenderer::Draw(xpe::ttf::Font* font, const xpe::render::TransformComponent* transform, const char* chars)
{
    // Bind font atlas
    font->Atlas.Slot = 0;
    m_Pipeline->Textures[0] = &font->Atlas;

    // Prepare text batch manager for drawing
    m_Manager->ClearInstances("Glyph");

    // Add instances to text batch manager
    xpe::core::usize charsCount = strlen(chars);
    m_Manager->ReserveInstances("Glyph", charsCount);
    for (core::usize i = 0; i < charsCount; i++)
    {
        char character = chars[i];
        auto it = font->AlphaBet.find(character);
        if (it == font->AlphaBet.end()) {
            continue;
        }

        Font::Glyph glyph = it->second;

        xpe::render::TransformComponent glyphTransform("GlyphTransform" + i);
        glyphTransform.Position.x = (float)i;

        xpe::render::TransformManager::UpdateTransform(i, glyphTransform);

        xpe::render::TextGlyphInstance glyphInstance;
        glyphInstance.TransformIndex = i;
        glyphInstance.Left = glyph.Left;
        glyphInstance.Top = glyph.Top;
        glyphInstance.Advance = glyph.Advance;

        m_Manager->AddInstance("Glyph", glyphInstance);
    }

    m_Manager->FlushInstances("Glyph");

    // Draw glyphs
    m_Context->BindRenderPipeline(m_Pipeline);
    m_Manager->DrawAll();
}