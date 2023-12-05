#pragma once

#include <rendering/buffers/widget_buffers.h>
#include <rendering/font/font.hpp>

namespace xpe {

    namespace render {

        using namespace math;
        using namespace res;

        struct ENGINE_API sWidget : public cXml {};

        class ENGINE_API cWidgetManager final
        {

        public:
            static void Init();
            static void Free();
            static void Bind(sVertexPipeline& pipeline);

            static sAtlas* GetAtlas();

        private:
            static sAtlas* s_WidgetAtlas;
            static sSampler s_WidgetSampler;

        };

    }

}