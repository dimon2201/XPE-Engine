#pragma once

#include <rendering/context.hpp>
#include <rendering/shader.h>
#include <rendering/render_target.h>

namespace xpe {

    namespace render {

        class ENGINE_API Canvas : public Object {

            public:
                Canvas(s32 width, s32 height, Context* context);
                Canvas(const glm::ivec2& size, Context* context);
                ~Canvas();

                void Clear(const glm::vec4& color);
                void Present();

                inline s32 GetWidth() const { return _size.x; }
                inline s32 GetHeight() const { return _size.y; }
                inline RenderTarget* GetRenderTarget() { return &_rt; }
                inline Shader* GetShader() { return &m_QuadShader; }

            private:
                glm::ivec2 _size;
                Context* _context;
                RenderTarget _rt;
                Shader m_QuadShader;
        };

    }
}