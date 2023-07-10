#pragma once

namespace xpe {

    namespace render {

        struct ENGINE_API DirectLight final {

        };

        struct ENGINE_API cDirectLightComponent : public cComponent {

            cDirectLightComponent(const string& usid) : cComponent(usid) {}

            DirectLight

        };

    }

}