#pragma once

#include <string>
#include <ttf/text.h>
#include <rendering/batching.h>
#include <rendering/pipeline.h>

namespace xpe
{
    namespace ttf
    {
        class ENGINE_API TextRenderer : public core::Object
        {

        public:
            static void Init(xpe::render::Context* context, xpe::render::TextBatchManager* manager);
            static void Free();

            static TextRenderer& Get() {
                return *s_Instance;
            }

        public:
            void DrawText(const Font* font, const xpe::render::TransformComponent* transform, const char* chars);

        private:
            static TextRenderer* s_Instance;
            static xpe::render::Context* s_Context;
            static xpe::render::TextBatchManager* s_Manager;
            core::vector<Text> m_Texts;
            render::Pipeline m_Pipeline;
        };
    }
}