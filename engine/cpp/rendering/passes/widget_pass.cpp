#include <rendering/passes/widget_pass.h>

#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        cWidgetPass::cWidgetPass(eType type, const vector<sRenderPassBinding>& bindings)
        : cRenderPass(type, bindings)
        {
            m_Quad = cGeometryManager::AddGeometry(sQuad());
            m_WidgetBuffer.Reserve(100);
            m_CharBuffer.Reserve(1000);
            m_Pipeline->PrimitiveTopology = m_Quad.PrimitiveTopology;
            cWidgetManager::Bind(*m_Pipeline);
        }

        void cWidgetPass::DrawWidgets()
        {
            if (m_WidgetBuffer.Empty())
                return;

            m_WidgetBuffer.Flush();

            sText text;
            text.Color = { -1, -1, -1, -1 };
            m_TextBuffer.Item = text;
            m_TextBuffer.Flush();

            context::BindVSBuffer(m_WidgetBuffer);
            context::BindVSBuffer(m_CharBuffer);
            context::BindVSBuffer(m_TextBuffer);
            context::DrawIndexed(m_Quad.Indices.size(), m_WidgetBuffer.Size(), m_Quad.VertexOffset, m_Quad.IndexOffset);
            context::UnbindVSBuffer(m_TextBuffer);
            context::UnbindVSBuffer(m_CharBuffer);
            context::UnbindVSBuffer(m_WidgetBuffer);

            m_WidgetBuffer.Clear();
        }

        void cWidgetPass::UpdateWidget(sWidget& widget, bool is2DSpace)
        {
            sTransform transform = widget.Transform;

            if (is2DSpace) {
                float frameWidth = (float) cWindowManager::GetFrameWidth();
                float frameHeight = (float) cWindowManager::GetFrameHeight();

                transform.Position = {
                        transform.Position.x * frameWidth,
                        transform.Position.y * frameHeight,
                        transform.Position.z
                };

                if (widget.FillFrame) {
                    transform.Scale = {
                            transform.Scale.x * frameWidth,
                            transform.Scale.y * frameHeight,
                            transform.Scale.z
                    };
                }
            }

            sWidgetData widgetData;
            widgetData.ModelMatrix = cMathManager::UpdateModelMatrix(transform);
            widgetData.EnableTexture = widget.EnableTexture;

            widget.CallbackUpdate(widgetData);

            bool isText = widget.Font != nullptr;

            if (isText) {
                sText text;
                text.ModelMatrix = widgetData.ModelMatrix;
                text.Color = widgetData.Color;
                DrawText(text, *widget.Font, widget.Text, transform.Scale);
            }
            else {
                auto mousePos = cInputManager::GetMouseCursor().Position;
                // flip Y axis for mouse to match coordinate system with widget
                mousePos.y = (float) cWindowManager::GetFrameHeight() - mousePos.y;

                bool mousePressed = cInputManager::MousePressed(widget.MousePressed);

                bool isHovered = IsHovered(widgetData, mousePos);

                if (isHovered) {
                    widgetData.Color = widget.ColorHover;
                    widget.CallbackHover();
                    if (mousePressed) {
                        widgetData.Color = widget.ColorPressed;
                        widget.CallbackPressed();
                    }
                }
                else {
                    widgetData.Color = widget.Color;
                    widget.CallbackReleased();
                }

                m_WidgetBuffer.Add(widgetData);
            }

            for (auto* child : widget.Children)
            {
                if (child->Visible) {
                    UpdateWidget(*child, is2DSpace);
                }
            }
        }

        void cWidgetPass::DrawText(const sText &text, sFont &font, const string& textString, const glm::vec3& textScale)
        {
            if (textString.empty()) return;

            usize charsCount = textString.size();
            const char* chars = textString.c_str();

            glm::vec2 advance = { 0, 0 };
            m_CharBuffer.Clear();
            for (usize i = 0; i < charsCount; i++)
            {
                char c = chars[i];

                // find glyph by char
                auto it = font.AlphaBet.find(c);
                if (it == font.AlphaBet.end()) {
                    continue;
                }
                sFont::sGlyph glyph = it->second;

                xpe::render::sChar character;
                character.GlyphSize = font.GlyphSize;
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
                    advance.x += font.WhitespaceOffset * textScale.x;
                }

                // New line
                else if (c == '\n')
                {
                    advance.x = 0.0f;
                    advance.y += (font.MaxGlyphHeight + font.NewLineOffset) * textScale.y;
                }

                else
                {
                    advance.x += (glyph.AdvanceX / 64.0f) * textScale.x;
                    m_CharBuffer.Add(character);
                }
            }
            m_CharBuffer.Flush();

            m_TextBuffer.Item = text;
            m_TextBuffer.Flush();

            context::BindVSBuffer(m_WidgetBuffer);
            context::BindVSBuffer(m_CharBuffer);
            context::BindVSBuffer(m_TextBuffer);
            context::BindTexture(font.Atlas, K_SLOT_FONT_ATLAS);
            context::DrawIndexed(m_Quad.Indices.size(), charsCount, m_Quad.VertexOffset, m_Quad.IndexOffset);
            context::UnbindTexture(font.Atlas);
            context::UnbindVSBuffer(m_TextBuffer);
            context::UnbindVSBuffer(m_CharBuffer);
            context::UnbindVSBuffer(m_WidgetBuffer);
        }

        bool cWidgetPass::IsHovered(const sWidgetData &widgetData, const glm::vec2 &mousePos)
        {
            glm::vec4 v1 = widgetData.ModelMatrix * glm::vec4(m_Quad.Vertices[0].Position, 1.0);
            glm::vec4 v2 = widgetData.ModelMatrix * glm::vec4(m_Quad.Vertices[1].Position, 1.0);
            glm::vec4 v3 = widgetData.ModelMatrix * glm::vec4(m_Quad.Vertices[2].Position, 1.0);
            glm::vec4 v4 = widgetData.ModelMatrix * glm::vec4(m_Quad.Vertices[3].Position, 1.0);

            float minX = min(min(min(v1.x, v2.x), v3.x), v4.x);
            float maxX = max(max(max(v1.x, v2.x), v3.x), v4.x);
            float minY = min(min(min(v1.y, v2.y), v3.y), v4.y);
            float maxY = max(max(max(v1.y, v2.y), v3.y), v4.y);

            bool isNotHovered = mousePos.x < minX || mousePos.x > maxX || mousePos.y < minY || mousePos.y > maxY;

            return !isNotHovered;
        }

        cWidgetPass2D::cWidgetPass2D(const vector<sRenderPassBinding> &bindings) : cWidgetPass(eType::UI, bindings) {}

        void cWidgetPass2D::DrawUI(cScene* scene)
        {
            scene->ForLoop<sCWidget2D>([this](sCWidget2D* component)
            {
               if (component->Visible) {
                   UpdateWidget(*component, true);
               }
            });
            DrawWidgets();
        }

        cWidgetPass3D::cWidgetPass3D(const vector<sRenderPassBinding> &bindings) : cWidgetPass(eType::UI, bindings) {}

        void cWidgetPass3D::DrawUI(cScene *scene)
        {
            scene->ForLoop<sCWidget3D>([this](sCWidget3D* component)
            {
               if (component->Visible) {
                   UpdateWidget(*component, false);
               }
            });
            DrawWidgets();
        }

    }

}