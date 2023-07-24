#include <ttf/text_renderer.hpp>
#include <ttf/text.h>

xpe::ttf::TextRenderer* xpe::ttf::TextRenderer::s_Instance = nullptr;
xpe::render::Context* xpe::ttf::TextRenderer::s_Context = nullptr;
xpe::render::TextBatchManager* xpe::ttf::TextRenderer::s_Manager = nullptr;

void xpe::ttf::TextRenderer::Init(xpe::render::Context* context, xpe::render::TextBatchManager* manager) {
    s_Instance = new TextRenderer();
    s_Context = context;
    s_Manager = manager;
}

void xpe::ttf::TextRenderer::Free() {
    delete s_Instance;
}

void xpe::ttf::TextRenderer::DrawText(const Font* font, const xpe::render::TransformComponent* transform, const char* chars)
{
    Text text;
    text.Chars = core::string(chars);
    text.Transform = (xpe::render::TransformComponent*)transform;
    text.TextFont = (Font*)font;

    s_Manager->ClearInstances("Glyph");
    s_Manager->AddText(text);

    m_Texts.push_back(text);
}