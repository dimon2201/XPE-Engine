#pragma once

#include <core/ecs.hpp>
#include <rendering/materials/material.h>

namespace xpe
{
    namespace ecs
    {
        using namespace core;
        using namespace render;

        struct ENGINE_API TransformComponent : public Component
        {
            TransformComponent(const string& usid) : Component(usid)
            {}

            TransformComponent(const string& usid, u32 index) : Component(usid), Index(index)
            {}

            u32 Index = 0;
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Rotation = { 0, 0, 0 };
            glm::vec3 Scale = { 1, 1, 1 };
        };

        struct ENGINE_API Transform2DComponent : public Component
        {
            Transform2DComponent(const string& usid) : Component(usid)
            {}

            Transform2DComponent(const string& usid, u32 index) : Component(usid), Index(index)
            {}

            u32 Index = 0;
            glm::vec2 Position = { 0, 0 };
            float Rotation = 0;
            glm::vec2 Scale = { 1, 1 };
        };

        struct ENGINE_API RigidBodyComponent : public Component
        {
            RigidBodyComponent(const string& usid) : Component(usid)
            {}

            glm::vec3 Position;
            glm::vec3 Rotation;
        };

        struct ENGINE_API CameraComponent : public Component
        {
            u32 Index = 0;
            // Position.z = -1 is a default valid value for 2D orthographic view
            // If Position.z >= 0, 2D geometry will not be shown on screen
            glm::vec3 Position = { 0, 0, -1 };
            glm::vec3 Front = { 0, 0, 0 };
            glm::vec3 Up = { 0, 1, 0 };

            float Gamma = 2.2f;
            float Exposure = 1.0f;

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

        struct ENGINE_API MaterialComponent : public Component {

            MaterialComponent(const string& usid) : Component(usid) {}
            MaterialComponent(const string& usid, Material* material) : Component(usid), Material(material) {}

            Material* Material = nullptr;

        };

        struct ENGINE_API Text2DComponent : public Component
        {
            Text2DComponent(const string& usid) : Component(usid) {}

            string Text;
        };

        struct ENGINE_API Text3DComponent : public Component
        {
            Text3DComponent(const string& usid) : Component(usid) {}

            string Text;
        };
    }
}