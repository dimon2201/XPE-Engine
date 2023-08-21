#pragma once

#include <ecs/component.h>

#include <rendering/materials/material.h>
#include <rendering/font/font.hpp>

#include <anim/skin.h>
#include <anim/skelet.h>

namespace xpe
{
    namespace ecs
    {
        using namespace core;
        using namespace render;
        using namespace math;
        using namespace anim;

        struct ENGINE_API CameraComponent : Component
        {
            // Position.z = -1 is a default valid value for 2D orthographic view
            // If Position.z >= 0, 2D geometry will not be shown on screen
            glm::vec3 Position = { 0, 0, -1 };
            glm::vec3 Front = { 0, 0, 0 };
            glm::vec3 Up = { 0, 1, 0 };

            float Gamma = 2.2f;
            float Exposure = 1.0f;

            JsonClass(
                CameraComponent,
                Position,
                Front,
                Up,
                Gamma,
                Exposure
            )
        };

        struct ENGINE_API PerspectiveCameraComponent : CameraComponent
        {
            math::PerspectiveMatrix Projection;

            JsonClass(
                PerspectiveCameraComponent,
                Projection
            )
        };

        struct ENGINE_API OrthoCameraComponent : CameraComponent
        {
            math::OrthoMatrix Projection;

            JsonClass(
                OrthoCameraComponent,
                Projection
            )
        };

        struct ENGINE_API Text2DComponent : Component
        {
            math::Transform Transform;
            string Text;
            Ref<Font> Font;
            string FontResFilepath;

            JsonClass(
                Text2DComponent,
                Text,
                FontResFilepath,
                Transform
            )
        };

        struct ENGINE_API Text3DComponent : Component
        {
            math::Transform Transform;
            string Text;
            Ref<Font> Font;
            string FontResFilepath;

            JsonClass(
                Text3DComponent,
                Text,
                FontResFilepath,
                Transform
            )
        };

        struct ENGINE_API TextureComponent : Component
        {
            Ref<Texture> Texture;
        };

        struct ENGINE_API DirectLightComponent : Component
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Color = { 1, 1, 1 };

            JsonClass(
                DirectLightComponent,
                Position,
                Color
            )
        };

        struct ENGINE_API PointLightComponent : Component
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Color = { 1, 1, 1 };
            float Constant;
            float Linear;
            float Quadratic;

            JsonClass(
                PointLightComponent,
                Position,
                Color,
                Constant,
                Linear,
                Quadratic
            )
        };

        struct ENGINE_API SpotLightComponent : Component
        {
            glm::vec3 Position = { 0, 0, 0 };
            glm::vec3 Direction = { 0, 0, 0 };
            glm::vec3 Color = { 1, 1, 1 };
            float Cutoff;
            float Outer;

            JsonClass(
                SpotLightComponent,
                Position,
                Direction,
                Color,
                Cutoff,
                Outer
            )
        };

        template<class GeometryType>
        struct GeometryComponent : Component
        {
            Ref<GeometryType> Geometry;
            MaterialInstance Instance;

            JsonClass(
                GeometryComponent,
                Instance
            )
        };

        typedef GeometryComponent<GeometryVertexed<Vertex2D>> GeometryVertexed2DComponent;
        typedef GeometryComponent<GeometryIndexed<Vertex2D>> GeometryIndexed2DComponent;
        typedef GeometryComponent<GeometryVertexed<Vertex3D>> GeometryVertexed3DComponent;
        typedef GeometryComponent<GeometryIndexed<Vertex3D>> GeometryIndexed3DComponent;

        template<class GeometryType>
        struct GeometryListComponent : Component
        {
            Ref<GeometryType> Geometry;
            vector<MaterialInstance> Instances;

            JsonClass(
                GeometryListComponent,
                Instances
            )
        };

        typedef GeometryListComponent<GeometryVertexed<Vertex2D>> GeometryVertexed2DListComponent;
        typedef GeometryListComponent<GeometryIndexed<Vertex2D>> GeometryIndexed2DListComponent;
        typedef GeometryListComponent<GeometryVertexed<Vertex3D>> GeometryVertexed3DListComponent;
        typedef GeometryListComponent<GeometryIndexed<Vertex3D>> GeometryIndexed3DListComponent;

        struct ENGINE_API MeshComponent : Component
        {
            math::Transform Transform;
            Ref<Mesh> Mesh;

            JsonClass(
                MeshComponent,
                Transform
            )
        };

        struct ENGINE_API MeshListComponent : Component
        {
            vector<Transform> Transforms;
            Ref<Mesh> Mesh;

            JsonClass(
                MeshListComponent,
                Transforms
            )
        };

        struct ENGINE_API ModelComponent : Component
        {
            math::Transform Transform;
            Ref<Model3D> Model;

            JsonClass(
                ModelComponent,
                Transform
            )
        };

        struct ENGINE_API ModelListComponent : Component
        {
            vector<Transform> Transforms;
            Ref<Model3D> Model;

            JsonClass(
                ModelListComponent,
                Transforms
            )
        };

        struct ENGINE_API SkinComponent : Component
        {
            math::Transform Transform;
            Ref<Skin> Skin;
            Ref<Skelet> Skelet;

            JsonClass(
                SkinComponent,
                Transform
            )
        };

        struct ENGINE_API SkinListComponent : Component
        {
            vector<Transform> Transforms;
            Ref<Skin> Skin;
            Ref<Skelet> Skelet;

            JsonClass(
                SkinListComponent,
                Transforms
            )
        };

        struct ENGINE_API SkinModelComponent : Component
        {
            math::Transform Transform;
            Ref<SkinModel> Model;
            Ref<Skelet> Skelet;

            JsonClass(
                SkinModelComponent,
                Transform
            )
        };

        struct ENGINE_API SkinModelListComponent : Component
        {
            vector<Transform> Transforms;
            Ref<SkinModel> Model;
            Ref<Skelet> Skelet;

            JsonClass(
                SkinModelListComponent,
                Transforms
            )
        };

        struct ENGINE_API SkeletalAnimationComponent : Component
        {
            Ref<Skelet> Skelet;
            Ref<Animation> Animation;
            bool Play = false;

            JsonClass(
                SkeletalAnimationComponent,
                m_Tag
            )
        };

    }
}