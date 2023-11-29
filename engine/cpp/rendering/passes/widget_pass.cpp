#include <rendering/passes/widget_pass.h>
#include <rendering/camera_manager.h>

#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        cWidgetPass::cWidgetPass(const vector<sRenderPassBinding>& bindings)
        : cRenderPass(eType::UI, bindings)
        {
            std::tie(m_Quad, m_QuadInfo) = cGeometryManager::AddGeometry(sQuad());
            m_Pipeline->PrimitiveTopology = m_QuadInfo.PrimitiveTopology;
            m_WidgetBuffer.Reserve(100);
            cWidgetManager::Bind(*m_Pipeline);
        }

        void cWidgetPass::DrawUI(cScene* scene)
        {
            sOrthoMatrix orthoMatrix;
            orthoMatrix.Left = cCameraManager::GetViewport()->Left;
            orthoMatrix.Top = cCameraManager::GetViewport()->Top;
            orthoMatrix.Right = cCameraManager::GetViewport()->Width;
            orthoMatrix.Bottom = cCameraManager::GetViewport()->Height;
            orthoMatrix.Near = -1.0f;
            orthoMatrix.Far = 1.0f;
            m_Projection2D = cMathManager::UpdateOrthoMatrix(orthoMatrix);
            m_Projection3D = cCameraManager::GetViewProjection();

            auto components = scene->GetComponents<CVisible, CSpace, CTransform, CButton>();
            for (auto [entity, visible, space, transform, button] : components.each())
            {
                UpdateWidget(
                        transform,
                        space.Space,
                        button.Color,
                        button.EnableTexture,
                        button.AtlasLocation,
                        button.FillFrame,
                        cInputManager::MousePressed(button.MousePressed) && button.EnablePress,
                        [&button](sWidgetData& widget) {
                            widget.Color = button.ColorPressed;
                            if (button.Pressed != nullptr) {
                                button.Pressed();
                            }
                        },
                        [&button](sWidgetData& widget) {
                            widget.Color = button.ColorHover;
                            if (button.Hovered != nullptr) {
                                button.Hovered();
                            }
                        }
                );
            }

            DrawWidgets();
        }

        void cWidgetPass::UpdateWidget(
                sTransform transform,
                eSpace space,
                const glm::vec4& color,
                bool enableTexture,
                const sAtlas::sLocation& atlasLocation,
                bool fillFrame,
                bool pressed,
                const std::function<void(sWidgetData&)>& pressedCallback,
                const std::function<void(sWidgetData&)>& hoveredCallback
        ) {
            sWidgetData widget;
            widget.HasTexture = enableTexture;
            widget.AtlasUV[0] = atlasLocation.UV[0];
            widget.AtlasUV[1] = atlasLocation.UV[1];
            widget.AtlasUV[2] = atlasLocation.UV[2];
            widget.AtlasUV[3] = atlasLocation.UV[3];
            bool isHovered;
            glm::mat4 modelMatrix;

            if (space == eSpace::SPACE_2D) {
                float frameWidth = (float) cWindowManager::GetFrameWidth();
                float frameHeight = (float) cWindowManager::GetFrameHeight();

                transform.Position = {
                        transform.Position.x * frameWidth,
                        transform.Position.y * frameHeight,
                        transform.Position.z
                };

                if (fillFrame) {
                    transform.Scale = {
                            transform.Scale.x * frameWidth,
                            transform.Scale.y * frameHeight,
                            transform.Scale.z
                    };
                }

                modelMatrix = cMathManager::UpdateModelMatrix(transform);
                widget.Projection = m_Projection2D;

                auto mousePos = cInputManager::GetMouseCursor().Position;
                // flip Y axis for mouse to match coordinate system with widget
                mousePos.y = (float) cWindowManager::GetFrameHeight() - mousePos.y;
                isHovered = IsHovered2D(modelMatrix, mousePos);
            }
            else {
                modelMatrix = cMathManager::UpdateModelMatrix(transform);
                widget.Projection = m_Projection3D;

                auto mousePos = cInputManager::GetMouseCursor().Position;
                // flip Y axis for mouse to match coordinate system with widget
                mousePos.y = (float) cWindowManager::GetFrameHeight() - mousePos.y;
                isHovered = IsHovered3D(modelMatrix, mousePos);
            }

            widget.ModelMatrix = modelMatrix;

            if (isHovered) {
                hoveredCallback(widget);
                if (pressed) {
                    pressedCallback(widget);
                }
            }
            else {
                widget.Color = color;
            }

            m_WidgetBuffer.Add(widget);
        }

        void cWidgetPass::DrawWidgets()
        {
            if (m_WidgetBuffer.Empty())
                return;

            m_WidgetBuffer.Flush();

            context::BindListBufferVS(m_WidgetBuffer);
            context::DrawIndexed(m_QuadInfo.IndexCount, m_WidgetBuffer.Size(), m_QuadInfo.VertexOffset, m_QuadInfo.IndexOffset);
            context::UnbindListBufferVS(m_WidgetBuffer);

            m_WidgetBuffer.Clear();
        }

        bool cWidgetPass::IsHovered2D(const glm::mat4 &modelMatrix, const glm::vec2 &mousePos)
        {
            glm::vec4 v1 = modelMatrix * glm::vec4(m_Quad.Vertices[0].Position, 1.0);
            glm::vec4 v2 = modelMatrix * glm::vec4(m_Quad.Vertices[1].Position, 1.0);
            glm::vec4 v3 = modelMatrix * glm::vec4(m_Quad.Vertices[2].Position, 1.0);
            glm::vec4 v4 = modelMatrix * glm::vec4(m_Quad.Vertices[3].Position, 1.0);

            float minX = min(min(min(v1.x, v2.x), v3.x), v4.x);
            float maxX = max(max(max(v1.x, v2.x), v3.x), v4.x);
            float minY = min(min(min(v1.y, v2.y), v3.y), v4.y);
            float maxY = max(max(max(v1.y, v2.y), v3.y), v4.y);

            bool isNotHovered = mousePos.x < minX || mousePos.x > maxX || mousePos.y < minY || mousePos.y > maxY;

            return !isNotHovered;
        }

        bool cWidgetPass::IsHovered3D(const glm::mat4 &modelMatrix, const glm::vec2 &mousePos)
        {
            return false;
        }

        cTextPass::cTextPass(const vector<sRenderPassBinding> &bindings)
        : cRenderPass(eType::UI, bindings)
        {
            std::tie(m_Quad, m_QuadInfo) = cGeometryManager::AddGeometry(sQuad());
            m_Pipeline->PrimitiveTopology = m_Quad.PrimitiveTopology;
            m_CharBuffer.Reserve(1000);
            m_TextBuffer.Reserve(100);

            m_FontSampler.Slot            = K_SLOT_FONT_SAMPLER;
            m_FontSampler.Filter          = sSampler::eFilter::MIN_MAG_MIP_LINEAR;
            m_FontSampler.AddressU        = sSampler::eAddress::WRAP;
            m_FontSampler.AddressV        = sSampler::eAddress::WRAP;
            m_FontSampler.AddressW        = sSampler::eAddress::WRAP;
            context::CreateSampler(m_FontSampler);

            m_Pipeline->Samplers.emplace_back(&m_FontSampler);
        }

        cTextPass::~cTextPass()
        {
            context::FreeSampler(m_FontSampler);
        }

        void cTextPass::DrawUI(cScene *scene)
        {
            sOrthoMatrix orthoMatrix;
            orthoMatrix.Left = cCameraManager::GetViewport()->Left;
            orthoMatrix.Top = cCameraManager::GetViewport()->Top;
            orthoMatrix.Right = cCameraManager::GetViewport()->Width;
            orthoMatrix.Bottom = cCameraManager::GetViewport()->Height;
            orthoMatrix.Near = -1.0f;
            orthoMatrix.Far = 1.0f;
            m_Projection2D = cMathManager::UpdateOrthoMatrix(orthoMatrix);
            m_Projection3D = cCameraManager::GetViewProjection();
            sTexture* fontAtlas = nullptr;

            auto components = scene->GetComponents<CVisible, CSpace, CTransform, CLabel>();
            for (auto [entity, visible, space, transform, label] : components.each())
            {
                fontAtlas = &label.Font->Atlas;
                UpdateText(
                        transform,
                        space.Space,
                        0,
                        *label.Font,
                        label.Text,
                        label.Color,
                        label.FillFrame
                );
            }

            if (fontAtlas != nullptr) {
                DrawTexts(*fontAtlas);
            }
        }

        void cTextPass::DrawTexts(sTexture& fontAtlas)
        {
            if (m_CharBuffer.Empty() || m_TextBuffer.Empty())
                return;

            m_CharBuffer.Flush();
            m_TextBuffer.Flush();

            context::BindListBufferVS(m_CharBuffer);
            context::BindListBufferVS(m_TextBuffer);
            context::BindTexture(fontAtlas, K_SLOT_FONT_ATLAS);
            context::DrawIndexed(m_QuadInfo.IndexCount, m_CharBuffer.Size(), m_QuadInfo.VertexOffset, m_QuadInfo.IndexOffset);
            context::UnbindTexture(fontAtlas);
            context::UnbindListBufferVS(m_TextBuffer);
            context::UnbindListBufferVS(m_CharBuffer);

            m_CharBuffer.Clear();
            m_TextBuffer.Clear();
        }

        void cTextPass::UpdateText(
            sTransform transform,
            eSpace space,
            u32 textIndex,
            sFont& font,
            const string& textString,
            const glm::vec4& textColor,
            bool fillFrame
        ) {
            if (textString.empty()) return;

            usize charsCount = textString.size();
            const char* chars = textString.c_str();
            glm::mat4 modelMatrix;
            sText text;
            text.Color = textColor;

            if (space == eSpace::SPACE_2D) {
                float frameWidth = (float) cWindowManager::GetFrameWidth();
                float frameHeight = (float) cWindowManager::GetFrameHeight();

                transform.Position = {
                        transform.Position.x * frameWidth,
                        transform.Position.y * frameHeight,
                        transform.Position.z
                };

                if (fillFrame) {
                    transform.Scale = {
                            transform.Scale.x * frameWidth,
                            transform.Scale.y * frameHeight,
                            transform.Scale.z
                    };
                }

                text.ModelMatrix = cMathManager::UpdateModelMatrix(transform);
                text.Projection = m_Projection2D;
            }
            else {
                text.ModelMatrix = cMathManager::UpdateModelMatrix(transform);
                text.Projection = m_Projection3D;
            }

            m_TextBuffer.Add(text);

            glm::vec2 advance = { 0, 0 };

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
                character.TextIndex = textIndex;
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
                    advance.x += font.WhitespaceOffset * transform.Scale.x;
                }

                // New line
                else if (c == '\n')
                {
                    advance.x = 0.0f;
                    advance.y += (font.MaxGlyphHeight + font.NewLineOffset) * transform.Scale.y;
                }

                else
                {
                    advance.x += (glyph.AdvanceX / 64.0f) * transform.Scale.x;
                    m_CharBuffer.Add(character);
                }
            }
        }

        bool cTextPass::IsHovered2D(const glm::mat4& modelMatrix, const glm::vec2 &mousePos) {
            glm::vec4 v1 = modelMatrix * glm::vec4(m_Quad.Vertices[0].Position, 1.0);
            glm::vec4 v2 = modelMatrix * glm::vec4(m_Quad.Vertices[1].Position, 1.0);
            glm::vec4 v3 = modelMatrix * glm::vec4(m_Quad.Vertices[2].Position, 1.0);
            glm::vec4 v4 = modelMatrix * glm::vec4(m_Quad.Vertices[3].Position, 1.0);

            float minX = min(min(min(v1.x, v2.x), v3.x), v4.x);
            float maxX = max(max(max(v1.x, v2.x), v3.x), v4.x);
            float minY = min(min(min(v1.y, v2.y), v3.y), v4.y);
            float maxY = max(max(max(v1.y, v2.y), v3.y), v4.y);

            bool isNotHovered = mousePos.x < minX || mousePos.x > maxX || mousePos.y < minY || mousePos.y > maxY;

            return !isNotHovered;
        }

        bool cTextPass::IsHovered3D(const glm::mat4 &modelMatrix, const glm::vec2 &mousePos)
        {
            return false;
        }

    }

}