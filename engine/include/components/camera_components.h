#pragma once

namespace xpe {

    namespace ecs {

        using namespace core;

        struct ENGINE_API CameraComponent : public Component
        {
            u32 Index = 0;
            glm::vec3 Position = { 0, 0, 10 };
            glm::vec3 Front = { 0, 0, -1 };
            glm::vec3 Up = { 0, 1, 0 };

            CameraComponent(const string& usid) : Component(usid)
            {}
        };

        struct ENGINE_API PerspectiveCameraComponent : public CameraComponent
        {
            math::PerspectiveMatrix Projection;

            PerspectiveCameraComponent(const string& usid) : CameraComponent(usid)
            {}
        };

        struct ENGINE_API OrthoCameraComponent : public CameraComponent
        {
            math::OrthoMatrix Projection;

            OrthoCameraComponent(const string& usid) : CameraComponent(usid)
            {}
        };

    }

}