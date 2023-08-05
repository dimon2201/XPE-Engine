#include <rendering/text/text_renderer.hpp>
#include <rendering/text/ttf_manager.hpp>
#include <rendering/text/text_batching.hpp>
#include <rendering/draw/canvas.hpp>
#include <rendering/camera/camera_manager.h>
#include <rendering/transforming/transforming.h>

#include <geometry/quad_geometry.h>

namespace xpe {

    namespace render {

        TextBatchManager* TextRenderer::s_BatchManager = nullptr;
        InputLayout* TextRenderer::s_InputLayout = nullptr;
        Pipeline* TextRenderer::s_Pipeline = nullptr;

        static Quad* s_Quad = nullptr;

        void TextRenderer::Init(TextBatchManager* batchManager, Canvas* canvas)
        {
            s_BatchManager = batchManager;
            s_InputLayout = new InputLayout();
            s_Pipeline = new Pipeline();

            // Setup pipeline
            s_Pipeline->VSBuffers.emplace_back(CameraManager::GetBuffer());
            s_Pipeline->VSBuffers.emplace_back(TransformManager::GetBuffer());

            s_Pipeline->Shader = ShaderManager::CreateShader("text");
            ShaderManager::AddVertexStageFromFile(s_Pipeline->Shader, "shaders/text.vs");
            ShaderManager::AddPixelStageFromFile(s_Pipeline->Shader, "shaders/text.ps");
            ShaderManager::BuildShader(s_Pipeline->Shader);

            s_InputLayout->Format = Vertex3D::Format;
            s_Pipeline->InputLayout = *s_InputLayout;

            s_Pipeline->RenderTarget = canvas->GetRenderTarget();
            s_Pipeline->DepthStencilState.UseDepthTest = K_TRUE;
            s_Pipeline->BlendState.UseBlending = K_TRUE;

            s_Pipeline->Textures.push_back(nullptr);

            context::CreatePipeline(*s_Pipeline);

            s_Quad = new Quad();
        }

        void TextRenderer::Free()
        {
            delete s_InputLayout;
            delete s_Pipeline;
            delete s_Quad;
        }

        void TextRenderer::Draw(TextComponent& text, TransformComponent& transform, Font& font)
        {
            usize charsCount = text.Text.size();
            const char* chars = text.Text.c_str();
            string& usid = text.USID;

            // Bind font atlas
            font.Atlas.Slot = 0;
            s_Pipeline->Textures[0] = &font.Atlas;

            // Store text geometry
            if (!s_BatchManager->HasGeometryIndexed(usid)) {
                s_BatchManager->StoreGeometryIndexed(usid, *s_Quad, charsCount);
            }

            // Reserve char instances
            s_BatchManager->ClearInstances(usid);
            if (s_BatchManager->GetInstanceCapacity(usid) < charsCount) {
                s_BatchManager->ReserveInstances(usid, charsCount);
            }

            // Add instances to text batch manager
            f32 tempAdvanceY = 0.0f;
            glm::vec2 advance = glm::vec2(0.0f);
            for (core::usize i = 0; i < charsCount; i++)
            {
                char character = chars[i];
                auto it = font.AlphaBet.find(character);
                if (it == font.AlphaBet.end()) {
                    continue;
                }

                Font::Glyph glyph = it->second;

                TextGlyphInstance instance;
                instance.TransformIndex = transform.Index;
                instance.GlyphSize = font.GlyphSize;
                instance.Width = glyph.Width;
                instance.Height = glyph.Height;
                instance.Left = glyph.Left;
                instance.Top = glyph.Top;
                instance.AdvanceX = advance.x;
                instance.AdvanceY = advance.y;
                instance.AtlasXOffset = glyph.AtlasXOffset;
                instance.AtlasYOffset = glyph.AtlasYOffset;
                advance.x += (glyph.AdvanceX / 64.0f) * transform.Scale.x;

                // Tab
                if (character == '\t')
                {
                    advance.x += font.WhitespaceOffset;
                }
                    // New line
                else if (character == '\n')
                {
                    advance.x = 0.0f;
                    advance.y = (tempAdvanceY + font.NewLineOffset)*transform.Scale.y;
                    tempAdvanceY = 0.0f;
                }
                else
                {
                    if (instance.Height > advance.y) {
                        tempAdvanceY = instance.Height;
                    }

                    s_BatchManager->AddInstance(usid, instance);
                }
            }

            s_BatchManager->FlushInstances(usid);

            // Draw glyphs
            context::BindPipeline(s_Pipeline);
            s_BatchManager->DrawAll();
        }

    }

}