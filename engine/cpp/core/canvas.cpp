#include <core/canvas.hpp>

xpe::core::Canvas::Canvas(s32 width, s32 height, RenderingContext_Interface* context)
{
    _size.x = width;
    _size.y = height;
    _context = context;
    _rt = _context->CreateRenderTarget(glm::ivec2(width, height), nullptr, nullptr, nullptr, nullptr);

    m_QuadShader = *ShaderManager::Builder()
            .AddVertexStageFromFile("engine_shaders/canvas.vs")
            .AddPixelStageFromFile("engine_shaders/canvas.ps")
            .Build("canvas");

    m_QuadShader.PrimitiveTopology = ePrimitiveTopology::TRIANGLE_STRIP;
}

xpe::core::Canvas::Canvas(const glm::ivec2& size, RenderingContext_Interface* context) :
    xpe::core::Canvas(size.x, size.y, context)
{
}

xpe::core::Canvas::~Canvas()
{
    _context->FreeRenderTarget(_rt);
}

void xpe::core::Canvas::Clear(const glm::vec4& color)
{
    _context->BindViewport(glm::ivec4(0.0f, 0.0f, _rt.Dimensions.x, _rt.Dimensions.y), 0.0f, 1.0f);
    _context->BindTexture(nullptr, eShaderType::PIXEL, 0);
    _context->BindRenderTarget(&_rt);
    _context->ClearRenderTarget(color, 1.0f);
}

void xpe::core::Canvas::Present()
{
    glm::ivec2 swapChainDimensions = _context->GetSwapChainDimensions();
    _context->BindRenderTarget(nullptr);
    _context->BindViewport(glm::ivec4(0.0f, 0.0f, swapChainDimensions.x, swapChainDimensions.y), 0.0f, 1.0f);
    _context->BindShader(&m_QuadShader);
    _context->BindTexture(_rt.ColorTexture, eShaderType::PIXEL, 0);
    _context->ClearRenderTarget(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f);
    _context->DrawQuad();
    _context->Present();
}