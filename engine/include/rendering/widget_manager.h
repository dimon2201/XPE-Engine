#pragma once

namespace xpe {

    namespace render {

        using namespace res;

        struct ENGINE_API sWidget : public cXml
        {
            glm::mat4 Projection = glm::mat4(1);
        };

        class ENGINE_API cWidgetManager final
        {
        public:
            static void Init();
            static void Free();
        };

    }

}