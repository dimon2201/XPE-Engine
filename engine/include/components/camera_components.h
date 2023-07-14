#pragma once

namespace xpe {

    namespace ecs {

        using namespace core;

        struct ENGINE_API CameraComponent : public Component
        {

            CameraComponent(const string& usid) : Component(usid)
            {}

            u32 Index = 0;

            glm::vec3 Position = { 0, 0, 10 };
            glm::vec3 Front = { 0, 0, -1 };
            glm::vec3 Up = { 0, 1, 0 };

            float Pitch = 0;
            float Yaw = 0;
            float Roll = 0;

            float Distance = 0;
            glm::vec3 FocalPoint = { 0, 0, 0 };

            glm::vec3 GetUpDirection() const;
            glm::vec3 GetRightDirection() const;
            glm::vec3 GetForwardDirection() const;
            glm::vec3 CalculatePosition() const;
            glm::quat GetOrientation() const;

        };

        struct ENGINE_API PerspectiveCameraComponent : public CameraComponent
        {
            PerspectiveCameraComponent(const string& usid) : CameraComponent(usid)
            {}

            math::PerspectiveMatrix Projection;
        };

        struct ENGINE_API OrthoCameraComponent : public CameraComponent
        {
            OrthoCameraComponent(const string& usid) : CameraComponent(usid)
            {}

            math::OrthoMatrix Projection;
        };

    }

}