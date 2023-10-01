#include <rendering/passes/text_pass.h>
#include <rendering/storages/geometry_storage.h>
#include <rendering/font/font.hpp>

#include <ecs/scenes.hpp>

namespace xpe {

    namespace render {

        TextPass::TextPass(
            const core::vector<RenderPassBinding>& bindings,
            RenderTarget* output,
            GeometryStorage* geometryStorage
        ) : RenderPass(bindings, output)
        {
            m_TextBuffer.Reserve(1000);
            m_TransformBuffer.Reserve(1);

            auto& quad = geometryStorage->GetGeometryIndexed3D("TextQuad");
            if (quad.Get() == nullptr) {
                quad = geometryStorage->AddGeometryIndexed3D("TextQuad", Quad());
            }
            m_Pipeline->InputLayout.Format = Vertex3D::Format;
            m_Pipeline->PrimitiveTopology = quad->PrimitiveTopology;
            m_Pipeline->VertexBuffer = &quad->Vertices;
            m_Pipeline->IndexBuffer = &quad->Indices;
            m_Pipeline->Textures.emplace_back(nullptr);
            m_Pipeline->VSBuffers.emplace_back(&m_TransformBuffer);
        }

        TextPass::~TextPass() {}

        void TextPass::DrawText(const Transform &transform, const string &text, const Ref<Font> &font)
        {
            if (text.empty()) return;
            
            usize charsCount = text.size();
            const char* chars = text.c_str();

            glm::vec2 advance = { 0, 0 };
            m_TextBuffer.Clear();
            for (usize i = 0; i < charsCount; i++)
            {
                char c = chars[i];
                // todo unused variable
//                char prevCharacter = i > 0 ? chars[i - 1] : 0;
                auto it = font->AlphaBet.find(c);
                if (it == font->AlphaBet.end()) {
                    continue;
                }

                Font::Glyph glyph = it->second;

                xpe::render::Character character;
                character.TransformIndex = 0;
                character.CameraIndex = 0;
                character.GlyphSize = font->GlyphSize;
                character.Width = glyph.Width;
                character.Height = glyph.Height;
                character.Left = glyph.Left;
                character.Top = glyph.Top;
                character.AdvanceX = advance.x;
                character.AdvanceY = advance.y;
                character.AtlasXOffset = glyph.AtlasXOffset;
                character.AtlasYOffset = glyph.AtlasYOffset;

                // Tab
                if (c == '\t')
                {
                    advance.x += font->WhitespaceOffset * transform.Scale.x;
                }

                // New line
                else if (c == '\n')
                {
                    advance.x = 0.0f;
                    advance.y += (font->MaxGlyphHeight + font->NewLineOffset) * transform.Scale.y;
                }

                else
                {
                    advance.x += (glyph.AdvanceX / 64.0f) * transform.Scale.x;
                    m_TextBuffer.Add(character);
                }
            }
            m_TextBuffer.Flush();

            m_TransformBuffer.Clear();
            m_TransformBuffer.AddTransform(transform);
            m_TransformBuffer.Flush();

            m_Pipeline->Textures[0] = &font->Atlas;

            context::BindVSBuffer(m_TextBuffer);
            context::DrawIndexed(0, 0, 6, charsCount);
            context::UnbindVSBuffer(m_TextBuffer);
        }

    }

}