#pragma once

#include <core/rendering.hpp>
#include <core/shader.h>

namespace xpe
{
    namespace core
    {
        class ENGINE_API Canvas : public Object
        {
            public:
                Canvas(s32 width, s32 height, RenderingContext_Interface* context);
                Canvas(const glm::ivec2& size, RenderingContext_Interface* context);
                ~Canvas();

                void Clear(const glm::vec4& color);
                void Present();

                inline s32 GetWidth() { return _size.x; }
                inline s32 GetHeight() { return _size.y; }
                inline xRenderTarget* GetRenderTarget() { return &_rt; }
                inline Shader* GetShader() { return &m_QuadShader; }

            private:
                glm::ivec2 _size;
                RenderingContext_Interface* _context;
                xRenderTarget _rt;
                Shader m_QuadShader;
        };
    }
}