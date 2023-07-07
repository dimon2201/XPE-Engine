#include <rendering/canvas.hpp>

namespace xpe {

    namespace render {

        Canvas::Canvas(s32 width, s32 height, Context* context) {
            _size.x = width;
            _size.y = height;
            _context = context;
            _rt = _context->CreateRenderTarget(glm::ivec2(width, height), nullptr, nullptr, nullptr, nullptr);

            m_QuadShader = *ShaderManager::Builder()
                .AddVertexStageFromFile("engine_shaders/canvas.vs")
                .AddPixelStageFromFile("engine_shaders/canvas.ps")
                .Build();

            m_QuadShader.PrimitiveTopology = ePrimitiveTopology::TRIANGLE_STRIP;
        }

        Canvas::Canvas(const glm::ivec2& size, Context* context) : Canvas(size.x, size.y, context) {}

        Canvas::~Canvas() {
            _context->FreeRenderTarget(_rt);
        }

        void Canvas::Clear(const glm::vec4& color) {
            _context->BindViewport(glm::ivec4(0.0f, 0.0f, _rt.Dimensions.x, _rt.Dimensions.y), 0.0f, 1.0f);
            _context->BindTextureSlot(0);
            _context->BindRenderTarget(&_rt);
            _context->ClearRenderTarget(color, 1.0f);
        }

        void Canvas::Present() {
            glm::ivec2 swapChainDimensions = _context->GetSwapChainDimensions();
            _context->BindRenderTarget(nullptr);
            _context->BindViewport(glm::ivec4(0.0f, 0.0f, swapChainDimensions.x, swapChainDimensions.y), 0.0f, 1.0f);
            _context->BindShader(&m_QuadShader);
            _context->BindTexture(_rt.ColorTexture);
            _context->ClearRenderTarget(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f);
            _context->DrawQuad();
            _context->Present();
        }

    }

}
