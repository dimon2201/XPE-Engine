#include <rendering/passes/widget_pass.h>

#include <ecs/components.hpp>

namespace xpe {

    namespace render {

        cWidgetPass::cWidgetPass(const vector<sRenderPassBinding>& bindings)
        : cRenderPass(cRenderPass::eType::UI, bindings)
        {
            m_Quad = cGeometryManager::AddGeometry(sQuad());
            m_WidgetBuffer.Reserve(100);
            m_Pipeline->PrimitiveTopology = m_Quad.PrimitiveTopology;
            cWidgetManager::Bind(*m_Pipeline);
        }

        void cWidgetPass::DrawUI(cScene* scene)
        {
            m_WidgetBuffer.Clear();

            scene->ForLoop<sCWidget>([this](sCWidget* component)
            {
                if (component->Visible) {
                    UpdateWidget(*component);
                }
            });

            if (m_WidgetBuffer.Empty())
                return;

            m_WidgetBuffer.Flush();

            context::BindVSBuffer(m_WidgetBuffer);
            context::DrawIndexed(m_Quad.Indices.size(), m_WidgetBuffer.Size(), m_Quad.VertexOffset, m_Quad.IndexOffset);
            context::UnbindVSBuffer(m_WidgetBuffer);
        }

        void cWidgetPass::UpdateWidget(sWidget& widget)
        {
            sTransform transform = widget.Transform;

            transform.Position = {
                    transform.Position.x * (float) cWindowManager::GetFrameWidth(),
                    transform.Position.y * (float) cWindowManager::GetFrameHeight(),
                    transform.Position.z
            };

            transform.Scale = {
                    transform.Scale.x * (float) cWindowManager::GetFrameWidth(),
                    transform.Scale.y * (float) cWindowManager::GetFrameHeight(),
                    transform.Scale.z
            };

            sWidgetData widgetData;
            widgetData.ModelMatrix = MathManager::UpdateModelMatrix(transform);

            glm::vec2 x1y1 = transform.Position;
            glm::vec2 x2y1 = { transform.Position.x * transform.Scale.x, transform.Position.y };
            glm::vec2 x1y2 = { transform.Position.x,                     transform.Position.y * transform.Scale.y };
            glm::vec2 x2y2 = { transform.Position.x * transform.Scale.x, transform.Position.y * transform.Scale.y };

            auto& mousePos = cInputManager::GetMouseCursor().Position;
            bool mousePressed = cInputManager::MousePressed(eMouse::ButtonRight);
            bool isHovered = false;

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

            widgetData.EnableTexture = widget.EnableTexture;

            widget.CallbackUpdate(widgetData);

            m_WidgetBuffer.Add(widgetData);

            for (auto* child : widget.Children)
            {
                if (child->Visible) {
                    UpdateWidget(*child);
                }
            }
        }

    }

}