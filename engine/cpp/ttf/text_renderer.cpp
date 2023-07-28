#include <ttf/text_renderer.hpp>
#include <ttf/ttf_manager.hpp>
#include <core/types.hpp>
#include <core/camera.h>
#include <rendering/transforming/transforming.h>
#include <rendering/materials/material.h>
#include <rendering/lighting/light_manager.h>
#include <rendering/core/shader.h>
#include <rendering/dx11/d3d11_context.hpp>
#include <rendering/draw/canvas.hpp>
#include <rendering/draw/batching.h>
#include <geometry/sphere_geometry.h>

xpe::ttf::TextRenderer* xpe::ttf::TextRenderer::s_Instance = nullptr;

void xpe::ttf::TextRenderer::Init(xpe::render::Context* context, xpe::render::TextBatchManager* batchManager, xpe::render::Canvas* canvas) {
    s_Instance = new TextRenderer();
    s_Instance->m_Context = context;
    s_Instance->m_BatchManager = batchManager;
    s_Instance->m_InputLayout = new xpe::render::InputLayout();
    s_Instance->m_Pipeline = new xpe::render::Pipeline();

    // Setup pipeline
    s_Instance->m_Pipeline->VSBuffers.emplace_back(xpe::core::CameraManager::GetCameraBuffer());
    s_Instance->m_Pipeline->VSBuffers.emplace_back(xpe::render::TransformManager::GetBuffer());

    s_Instance->m_Pipeline->Shader = xpe::render::ShaderManager::CreateShader("text");
    xpe::render::ShaderManager::AddVertexStageFromFile(s_Instance->m_Pipeline->Shader, "shaders/text.vs");
    xpe::render::ShaderManager::AddPixelStageFromFile(s_Instance->m_Pipeline->Shader, "shaders/text.ps");
    xpe::render::ShaderManager::BuildShader(s_Instance->m_Pipeline->Shader);

    s_Instance->m_InputLayout->Format = xpe::render::Vertex3D::Format;
    s_Instance->m_Pipeline->InputLayout = *s_Instance->m_InputLayout;

    s_Instance->m_Pipeline->RenderTarget = canvas->GetRenderTarget();
    s_Instance->m_Pipeline->DepthStencilState.UseDepthTest = xpe::core::K_TRUE;

    s_Instance->m_Pipeline->Textures.push_back(nullptr);

    // Init pipeline
    context->CreatePipeline(*s_Instance->m_Pipeline);

    // Store quad geometry to batch manager
    xpe::render::SphereGeometry quad;
    s_Instance->m_BatchManager->StoreGeometryIndexed("__TextQuad", quad, 1);
    
}

void xpe::ttf::TextRenderer::Free() {
    delete s_Instance;
}

void xpe::ttf::TextRenderer::Draw(xpe::ttf::Font* font, const xpe::core::TransformComponent* transform, const char* chars)
{
    // Bind font atlas
    font->Atlas.Slot = 0;
    m_Pipeline->Textures[0] = &font->Atlas;

    // Prepare text batch manager for drawing
    m_BatchManager->ClearInstances("__TextQuad");

    // Add instances to text batch manager
    xpe::core::usize charsCount = strlen(chars);
    m_BatchManager->ReserveInstances("__TextQuad", charsCount);
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

        xpe::render::TransformManager::AddTransform(glyphTransform);

        xpe::render::TextGlyphInstance glyphInstance;
        glyphInstance.TransformIndex = i;
        glyphInstance.Left = glyph.Left;
        glyphInstance.Top = glyph.Top;
        glyphInstance.Advance = glyph.Advance;

        m_BatchManager->AddInstance("__TextQuad", glyphInstance);
    }

    m_BatchManager->FlushInstances("__TextQuad");

    xpe::render::TransformManager::FlushTransforms();

    // Draw glyphs
    m_Context->BindPipeline(m_Pipeline);
    m_BatchManager->DrawAll();

    xpe::render::TransformManager::ClearTransforms();
}