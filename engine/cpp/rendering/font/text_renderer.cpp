//#include <rendering/text/text_renderer.hpp>
//#include <rendering/text/ttf_manager.hpp>
//#include <rendering/text/text_batching.hpp>
//#include <rendering/passess/canvas.hpp>
//#include <rendering/camera/camera_manager.h>
//#include <rendering/transforming/transforming.h>
//
//#include <geometry/quad_geometry.h>
//
//namespace xpe {
//
//    namespace render {
//
//        Text2DRenderer::Text2DRenderer(TextBatchManager *batchManager, Canvas *canvas)
//        {
//            Shader* shader = ShaderManager::CreateShader("text2d");
//            ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/text/text2d.vs");
//            ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/text/text2d.ps");
//            ShaderManager::BuildShader(shader);
//            Init(batchManager, canvas, shader);
//        }
//
//        Text2DRenderer::~Text2DRenderer()
//        {
//            Free();
//        }
//
//        void Text2DRenderer::Draw(Text2DComponent &textComponent, TransformComponent &transform, Font &font)
//        {
//            DrawText(textComponent.USID, textComponent.Text, transform, font);
//        }
//
//        Text3DRenderer::Text3DRenderer(TextBatchManager *batchManager, Canvas *canvas)
//        {
//            Shader* shader = ShaderManager::CreateShader("text3d");
//            ShaderManager::AddVertexStageFromFile(shader, "engine_shaders/text/text3d.vs");
//            ShaderManager::AddPixelStageFromFile(shader, "engine_shaders/text/text3d.ps");
//            ShaderManager::BuildShader(shader);
//            Init(batchManager, canvas, shader);
//        }
//
//        Text3DRenderer::~Text3DRenderer()
//        {
//            Free();
//        }
//
//        void Text3DRenderer::Draw(Text3DComponent &textComponent, TransformComponent &transform, Font &font)
//        {
//            DrawText(textComponent.USID, textComponent.Text, transform, font);
//        }
//
//        void TextRenderer::Init(TextBatchManager* batchManager, Canvas* canvas, Shader* shader)
//        {
//            m_BatchManager = batchManager;
//            m_InputLayout = new InputLayout();
//            m_Pipeline = new Pipeline();
//
//            // Setup pipeline
//            m_Pipeline->VSBuffers.emplace_back(CameraManager::GetBuffer());
//            m_Pipeline->VSBuffers.emplace_back(TransformManager::GetBuffer());
//            m_Pipeline->VSBuffers.emplace_back(canvas->GetBuffer());
//
//            m_Pipeline->Shader = shader;
//
//            m_InputLayout->Format = Vertex3D::Format;
//            m_Pipeline->InputLayout = *m_InputLayout;
//
//            m_Pipeline->RenderTarget = canvas->GetRenderTarget();
//            m_Pipeline->DepthStencilState.UseDepthTest = K_TRUE;
//            m_Pipeline->BlendState.UseBlending = K_TRUE;
//
//            m_Pipeline->Textures.push_back(nullptr);
//
//            context::CreatePipeline(*m_Pipeline);
//
//            m_Quad = new Quad();
//        }
//
//        void TextRenderer::Free()
//        {
//            delete m_InputLayout;
//            delete m_Pipeline;
//            delete m_Quad;
//        }
//
//        void TextRenderer::DrawText(const string& usid, string& text, TransformComponent& transform, Font& font)
//        {
//            usize charsCount = text.size();
//            const char* chars = text.c_str();
//
//            // Bind font atlas
//            font.Atlas.Slot = 0;
//            m_Pipeline->Textures[0] = &font.Atlas;
//
//            // Store text geometry
//            if (!m_BatchManager->HasGeometryIndexed(usid)) {
//                m_BatchManager->StoreGeometryIndexed(usid, *m_Quad, charsCount);
//            }
//
//            // Reserve char instances
//            m_BatchManager->ClearInstances(usid);
//            if (m_BatchManager->GetInstanceCapacity(usid) < charsCount) {
//                m_BatchManager->ReserveInstances(usid, charsCount);
//            }
//
//            // Add instances to text batch manager
//            glm::vec2 advance = glm::vec2(0.0f);
//            for (core::usize i = 0; i < charsCount; i++)
//            {
//                char character = chars[i];
//                // todo unused variable
////                char prevCharacter = i > 0 ? chars[i - 1] : 0;
//                auto it = font.AlphaBet.find(character);
//                if (it == font.AlphaBet.end()) {
//                    continue;
//                }
//
//                Font::Glyph glyph = it->second;
//
//                xpe::render::TextGlyphInstance instance;
//                instance.TransformIndex = transform.Index;
//                instance.GlyphSize = font.GlyphSize;
//                instance.Width = glyph.Width;
//                instance.Height = glyph.Height;
//                instance.Left = glyph.Left;
//                instance.Top = glyph.Top;
//                instance.AdvanceX = advance.x;
//                instance.AdvanceY = advance.y;
//                instance.AtlasXOffset = glyph.AtlasXOffset;
//                instance.AtlasYOffset = glyph.AtlasYOffset;
//
//                // Tab
//                if (character == '\t')
//                {
//                    advance.x += font.WhitespaceOffset * transform.Scale.x;
//                }
//
//                // New line
//                else if (character == '\n')
//                {
//                    advance.x = 0.0f;
//                    advance.y += (font.MaxGlyphHeight + font.NewLineOffset) * transform.Scale.y;
//                }
//
//                else
//                {
//                    advance.x += (glyph.AdvanceX / 64.0f) * transform.Scale.x;
//
//                    m_BatchManager->AddInstance(usid, instance);
//                }
//            }
//
//            m_BatchManager->FlushInstances(usid);
//
//            // Draw glyphs
//            context::BindPipeline(m_Pipeline);
//            m_BatchManager->DrawAll();
//        }
//
//    }
//
//}