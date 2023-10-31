#pragma once

namespace xpe {

    namespace math {

        struct ENGINE_API sRay final
        {
            glm::vec4 Origin = { 0, 0, 0, 1 };
            glm::vec4 Direction = { 0, 0, 0, 1 };
        };

    }

}