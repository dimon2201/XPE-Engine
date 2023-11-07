#pragma once

#include <rendering/passes/render_pass.h>
#include <rendering/buffers/widget_buffer.h>
#include <rendering/geometry/geometry_manager.h>
#include <rendering/widget_manager.h>

namespace xpe {

    namespace render {

        using namespace core;

        class ENGINE_API cWidgetPass : public cRenderPass
        {

        public:
            cWidgetPass(const vector<sRenderPassBinding>& bindings);

            void DrawUI(cScene* scene) override final;

        private:
            void UpdateWidget(sWidget& widget);

            sGeometry m_Quad;
            sWidgetBuffer m_WidgetBuffer;
        };

    }

}