#pragma once

namespace xpe {

    namespace math {

        using namespace core;

        struct ENGINE_API sTransform : public cObject
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Rotation = { 0, 0, 0 };
            glm::vec3 Scale = { 1, 1, 1 };
        };
        Json(sTransform, Position, Rotation, Scale)

    }

}