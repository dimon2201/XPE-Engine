#pragma once

#include <string>
#include <ttf/text.h>
#include <rendering/batching.h>
#include <rendering/pipeline.h>
#include <rendering/canvas.hpp>

namespace xpe
{
    namespace ttf
    {
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

        public:
            void DrawText(Font* font, const xpe::render::TransformComponent* transform, const char* chars);

        private:
            static TextRenderer* s_Instance;
            static xpe::render::Context* s_Context;
            static xpe::render::TextBatchManager* s_Manager;
            static xpe::render::InputLayout* s_InputLayout;
            static xpe::render::Pipeline* s_Pipeline;
        };
    }
}