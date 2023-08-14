#pragma once

namespace xpe {

    namespace math {

        using namespace core;

        struct ENGINE_API Transform : public Object
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Rotation = { 0, 0, 0 };
            glm::vec3 Scale = { 1, 1, 1 };
        };

        Json(Transform, Position, Rotation, Scale)

        struct ENGINE_API Transform2D : public Object
        {
            glm::vec2 Position = { 0, 0 };
            float Rotation = 0;
            glm::vec2 Scale = { 1, 1 };
        };

        Json(Transform2D, Position, Rotation, Scale)

        struct ENGINE_API RigidBody : public Object
        {
            glm::vec3 Position;
            glm::vec3 Rotation;
        };

        Json(RigidBody, Position, Rotation)

    }

}