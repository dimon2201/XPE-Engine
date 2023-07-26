#pragma once

#include <string>
#include <rendering/batching.h>
#include <rendering/pipeline.h>
#include <rendering/canvas.hpp>

namespace xpe
{
    namespace ttf
    {
        class Font;

        class ENGINE_API TextRenderer : public core::Object
        {

        public:
            static void Init(
                xpe::render::Context* context,
                xpe::render::TextBatchManager* manager,
                xpe::render::Canvas* canvas
            );
            static void Free();

            static TextRenderer& Get() {
                return *s_Instance;
            }

            void Draw(Font* font, const xpe::render::TransformComponent* transform, const char* chars);

        private:
            static TextRenderer* s_Instance;
            xpe::render::Context* m_Context;
            xpe::render::TextBatchManager* m_Manager;
            xpe::render::InputLayout* m_InputLayout;
            xpe::render::Pipeline* m_Pipeline;
        };
    }
}