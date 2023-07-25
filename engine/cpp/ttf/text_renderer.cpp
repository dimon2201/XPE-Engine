#include <ttf/text_renderer.hpp>
#include <ttf/text.h>
#include <core/types.hpp>
#include <core/camera.h>
#include <rendering/transforming.h>
#include <rendering/materials/material.h>
#include <rendering/lighting/light_manager.h>
#include <rendering/shader.h>

xpe::ttf::TextRenderer* xpe::ttf::TextRenderer::s_Instance = nullptr;
xpe::render::Context* xpe::ttf::TextRenderer::s_Context = nullptr;
xpe::render::TextBatchManager* xpe::ttf::TextRenderer::s_Manager = nullptr;
xpe::render::InputLayout* xpe::ttf::TextRenderer::s_InputLayout = nullptr;
xpe::render::Pipeline* xpe::ttf::TextRenderer::s_Pipeline = nullptr;

void xpe::ttf::TextRenderer::Init(xpe::render::Context* context, xpe::render::TextBatchManager* manager, xpe::render::Canvas* canvas) {
    s_Instance = new TextRenderer();
    s_Context = context;
    s_Manager = manager;
    s_InputLayout = new xpe::render::InputLayout();
    s_Pipeline = new xpe::render::Pipeline();

    // Setup pipeline
    s_Pipeline->VSBuffers.emplace_back(xpe::core::CameraManager::GetCameraBuffer());
    s_Pipeline->VSBuffers.emplace_back(xpe::render::TransformManager::GetBuffer());
    s_Pipeline->VSBuffers.emplace_back(xpe::render::MaterialManager::GetBuffer());
    s_Pipeline->PSBuffers.emplace_back(xpe::render::LightManager::GetDirectBuffer());
    s_Pipeline->PSBuffers.emplace_back(xpe::render::LightManager::GetPointBuffer());
    s_Pipeline->PSBuffers.emplace_back(xpe::render::LightManager::GetSpotBuffer());

    s_Pipeline->Shader = xpe::render::ShaderManager::Builder()
        .AddVertexStageFromFile("shaders/text.vs")
        .AddPixelStageFromFile("shaders/text.ps")
        .Build("window");
    s_Pipeline->Shader->PrimitiveTopology = xpe::render::ePrimitiveTopology::TRIANGLE_LIST;

    s_InputLayout->PrimitiveTopology = xpe::render::ePrimitiveTopology::TRIANGLE_LIST;
    s_InputLayout->Format = xpe::render::Vertex3D::Format;
    s_Pipeline->InputLayout = *s_InputLayout;

    s_Pipeline->RenderTarget = canvas->GetRenderTarget();
    s_Pipeline->DepthStencilState.UseDepthTest = xpe::core::K_TRUE;

    // Init pipeline
    context->CreateRenderPipeline(*s_Pipeline);
}

void xpe::ttf::TextRenderer::Free() {
    delete s_Instance;
}

void xpe::ttf::TextRenderer::DrawText(Font* font, const xpe::render::TransformComponent* transform, const char* chars)
{
    Text text;
    text.Chars = core::string(chars);
    text.Transform = (xpe::render::TransformComponent*)transform;
    text.TextFont = (Font*)font;

    // Bind font atlas
    font->Atlas.Slot = 0;
    s_Pipeline->Textures.emplace_back(&font->Atlas);

    // Prepare text batch manager for drawing
    s_Manager->ClearInstances("Glyph");
    s_Manager->AddText(text);

    // Draw glyphs
    s_Manager->DrawAll();
}