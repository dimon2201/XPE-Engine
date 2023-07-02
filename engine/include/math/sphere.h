#pragma once

namespace xpe {

    namespace math {

        struct ENGINE_API Sphere final {
            glm::vec3 Center;
            float Radius;

            Sphere(const glm::vec3& center = { 0, 0, 0 }, const float radius = 1.0f)
            : Center(center), Radius(radius) {}
        };

    }

}