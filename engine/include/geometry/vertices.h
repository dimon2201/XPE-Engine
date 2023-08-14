#pragma once

#include <rendering/core/vertex.h>

namespace xpe {

    namespace math {

        using namespace render;

        struct ENGINE_API Vertex2D final
        {
            static VertexFormat Format;

            glm::vec2 Position = { 0, 0 };
            glm::vec2 UV = { 0, 0 };
        };

        Json(
            Vertex2D,
            Position,
            UV
        )

        struct ENGINE_API Vertex3D final
        {
            static VertexFormat Format;

            glm::vec3 Position = { 0, 0, 0 };
            glm::vec2 UV = { 0, 0 };
            glm::vec3 Normal = { 0, 0, 0 };
            glm::vec3 Tangent = { 0, 0, 0 };
        };

        Json(
            Vertex3D,
            Position,
            UV,
            Normal,
            Tangent
        )

    }

}