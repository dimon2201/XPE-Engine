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
    s_Instance->m_Canvas = canvas;
    s_Instance->m_InputLayout = new render::InputLayout();
    s_Instance->m_RTInfo = new render::StructureBuffer<glm::vec4>(context, 1, 7, core::K_FALSE);
    s_Instance->m_Pipeline2D = new render::Pipeline();
    s_Instance->m_Pipeline3D = new render::Pipeline();
    
    // Setup pipeline for 2D
    s_Instance->m_Pipeline2D->VSBuffers.emplace_back(render::CameraManager::GetBuffer());
    s_Instance->m_Pipeline2D->VSBuffers.emplace_back(render::TransformManager::GetBuffer());
    s_Instance->m_Pipeline3D->VSBuffers.emplace_back(s_Instance->m_RTInfo);

    s_Instance->m_Pipeline2D->Shader = render::ShaderManager::CreateShader("text2d");
    xpe::render::ShaderManager::AddVertexStageFromFile(s_Instance->m_Pipeline2D->Shader, "shaders/text2d.vs");
    xpe::render::ShaderManager::AddPixelStageFromFile(s_Instance->m_Pipeline2D->Shader, "shaders/text2d.ps");
    xpe::render::ShaderManager::BuildShader(s_Instance->m_Pipeline2D->Shader);

    s_Instance->m_InputLayout->Format = render::Vertex3D::Format;
    s_Instance->m_Pipeline2D->InputLayout = *s_Instance->m_InputLayout;

    s_Instance->m_Pipeline2D->RenderTarget = canvas->GetRenderTarget();
    s_Instance->m_Pipeline2D->DepthStencilState.UseDepthTest = core::K_TRUE;
    s_Instance->m_Pipeline2D->BlendState.UseBlending = core::K_TRUE;

    s_Instance->m_Pipeline2D->Textures.push_back(nullptr);

    context->CreatePipeline(*s_Instance->m_Pipeline2D);

    // Setup pipeline for 3D
    s_Instance->m_Pipeline3D->VSBuffers.emplace_back(render::CameraManager::GetBuffer());
    s_Instance->m_Pipeline3D->VSBuffers.emplace_back(render::TransformManager::GetBuffer());

    s_Instance->m_Pipeline3D->Shader = render::ShaderManager::CreateShader("text3d");
    xpe::render::ShaderManager::AddVertexStageFromFile(s_Instance->m_Pipeline3D->Shader, "shaders/text3d.vs");
    xpe::render::ShaderManager::AddPixelStageFromFile(s_Instance->m_Pipeline3D->Shader, "shaders/text3d.ps");
    xpe::render::ShaderManager::BuildShader(s_Instance->m_Pipeline3D->Shader);

    s_Instance->m_InputLayout->Format = render::Vertex3D::Format;
    s_Instance->m_Pipeline3D->InputLayout = *s_Instance->m_InputLayout;

    s_Instance->m_Pipeline3D->RenderTarget = canvas->GetRenderTarget();
    s_Instance->m_Pipeline3D->DepthStencilState.UseDepthTest = core::K_TRUE;
    s_Instance->m_Pipeline3D->BlendState.UseBlending = core::K_TRUE;

    s_Instance->m_Pipeline3D->Textures.push_back(nullptr);

    context->CreatePipeline(*s_Instance->m_Pipeline3D);

    // Store quad geometry to batch manager
    render::Quad quad;
    s_Instance->m_BatchManager->StoreGeometryIndexed("__TextQuad", quad, 1);
}

void xpe::text::TextRenderer::Free() {
    delete s_Instance;
}

void xpe::text::TextRenderer::Draw2D(xpe::text::Font* font, const xpe::ecs::TransformComponent* transform, const xpe::ecs::Text2DComponent* text)
{
    xpe::core::usize charsCount = text->Text.size();
    const char* chars = text->Text.c_str();

    // Bind font atlas
    font->Atlas.Slot = 0;
    m_Pipeline2D->Textures[0] = &font->Atlas;

    // Prepare text batch manager for drawing
    m_BatchManager->ClearInstances("__TextQuad");
    m_BatchManager->ReserveInstances("__TextQuad", charsCount);

    xpe::render::TransformManager::AddTransform(*(xpe::ecs::TransformComponent*)transform);
    xpe::render::TransformManager::FlushTransforms();

    // Add instances to text batch manager
    glm::vec2 advance = glm::vec2(0.0f);
    for (core::usize i = 0; i < charsCount; i++)
    {
        char character = chars[i];
        char prevCharacter = i > 0 ? chars[i - 1] : 0;
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

        // Tab
        if (character == '\t')
        {
            advance.x += font->WhitespaceOffset * transform->Scale.x;
        }
        // New line
        else if (character == '\n')
        {
            advance.x = 0.0f;
            advance.y += (font->MaxGlyphHeight + font->NewLineOffset) * transform->Scale.y;
        }
        else
        {
            advance.x += (glyph.AdvanceX / 64.0f) * transform->Scale.x;

            m_BatchManager->AddInstance("__TextQuad", instance);
        }
    }

    m_BatchManager->FlushInstances("__TextQuad");

    // Update render target info buffer
    s_Instance->m_RTInfo->Slot = 7;
    glm::vec2 rtSize = glm::vec2(m_Canvas->GetRenderTarget()->ColorTexture->Width, m_Canvas->GetRenderTarget()->ColorTexture->Height);
    m_RTInfo->m_List[0] = glm::vec4(1.0f);
    m_RTInfo->Flush();

    // Draw glyphs
    m_Context->BindPipeline(m_Pipeline2D);
    m_BatchManager->DrawAll();

    xpe::render::TransformManager::ClearTransforms();

    //m_RTInfo->Clear();
}

void xpe::text::TextRenderer::Draw3D(xpe::text::Font* font, const xpe::ecs::TransformComponent* transform, const xpe::ecs::Text3DComponent* text)
{
    xpe::core::usize charsCount = text->Text.size();
    const char* chars = text->Text.c_str();

    // Bind font atlas
    font->Atlas.Slot = 0;
    m_Pipeline3D->Textures[0] = &font->Atlas;

    // Prepare text batch manager for drawing
    m_BatchManager->ClearInstances("__TextQuad");
    m_BatchManager->ReserveInstances("__TextQuad", charsCount);

    xpe::render::TransformManager::AddTransform(*(xpe::ecs::TransformComponent*)transform);
    xpe::render::TransformManager::FlushTransforms();

    // Add instances to text batch manager
    glm::vec2 advance = glm::vec2(0.0f);
    for (core::usize i = 0; i < charsCount; i++)
    {
        char character = chars[i];
        char prevCharacter = i > 0 ? chars[i - 1] : 0;
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

        // Tab
        if (character == '\t')
        {
            advance.x += font->WhitespaceOffset * transform->Scale.x;
        }
        // New line
        else if (character == '\n')
        {
            advance.x = 0.0f;
            advance.y += (font->MaxGlyphHeight + font->NewLineOffset) * transform->Scale.y;
        }
        else
        {
            advance.x += (glyph.AdvanceX / 64.0f) * transform->Scale.x;

            m_BatchManager->AddInstance("__TextQuad", instance);
        }
    }

    m_BatchManager->FlushInstances("__TextQuad");

    // Draw glyphs
    m_Context->BindPipeline(m_Pipeline3D);
    m_BatchManager->DrawAll();

    xpe::render::TransformManager::ClearTransforms();
}