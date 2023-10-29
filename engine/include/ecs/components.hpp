#pragma once

#include <rendering/geometry/geometries.h>
#include <rendering/material/material.h>
#include <rendering/font/font.hpp>
#include <rendering/buffers/light_buffers.h>

#include <anim/skeleton.h>

#include <audio/core/context.h>

namespace xpe
{
    namespace physics
    {
        struct sActor;
        struct sShape;
    }

    namespace ecs
    {
        using namespace core;
        using namespace render;
        using namespace math;
        using namespace anim;
        using namespace audio;

        struct ENGINE_API CameraComponent : Component
        {
            // Position.z = -1 is a default valid value for 2D orthographic view
            // If Position.z >= 0, 2D geometry will not be shown on screen
            glm::vec3 Position = { 0, 0, -1 };
            glm::vec3 Front = { 0, 0, 0 };
            glm::vec3 Up = { 0, 1, 0 };

            JsonClass(
                CameraComponent,
                Position,
                Front,
                Up
            )
        };

        struct ENGINE_API PerspectiveCameraComponent : CameraComponent, PerspectiveMatrix
        {
            JsonClass(
                PerspectiveCameraComponent,
                m_Tag
            )
        };

        struct ENGINE_API OrthoCameraComponent : CameraComponent, OrthoMatrix
        {
            JsonClass(
                OrthoCameraComponent,
                m_Tag
            )
        };

        struct ENGINE_API Text2DComponent : Component
        {
            string Text;
            render::Font* Font;

            Text2DComponent(const string& text, render::Font* font) : Text(text), Font(font) {}

            JsonClass(
                Text2DComponent,
                Text
            )
        };

        struct ENGINE_API Text3DComponent : Component
        {
            string Text;
            render::Font* Font;

            Text3DComponent(const string& text, render::Font* font) : Text(text), Font(font) {}

            JsonClass(
                Text3DComponent,
                Text
            )
        };

        struct ENGINE_API MaterialComponent : Component, Material
        {
            MaterialComponent(const Material& material) : Material(material) {}
            MaterialComponent(Material&& material) : Material(material) {}

            JsonClass(
                MaterialComponent,
                m_Tag
            )
        };

        struct ENGINE_API DirectLightComponent : Component, DirectLightData, OrthoMatrix
        {
            DirectLightComponent(const glm::vec3& position, const glm::vec3& color)
            {
                Position = position;
                Color = color;
                Left = -10.0f;
                Right = 10.0f;
                Bottom = -10.0f;
                Top = 10.0f;
                Near = 1.0f;
                Far = 25.0f;
            }

            JsonClass(
                DirectLightComponent,
                Position,
                Color,
                Left,
                Right,
                Bottom,
                Top,
                Near,
                Far
            )
        };

        struct ENGINE_API PointLightComponent : Component, PointLightData
        {
            PointLightComponent(const glm::vec3& position, const glm::vec3& color)
            {
                Position = position;
                Color = color;
            }

            JsonClass(
                PointLightComponent,
                Position,
                Color,
                Constant,
                Linear,
                Quadratic
            )
        };

        struct ENGINE_API SpotLightComponent : Component, SpotLightData, OrthoMatrix
        {
            SpotLightComponent(const glm::vec3& position, const glm::vec3& color)
            {
                Position = position;
                Color = color;
                Left = -10.0f;
                Right = 10.0f;
                Bottom = -10.0f;
                Top = 10.0f;
                Near = 1.0f;
                Far = 25.0f;
            }

            JsonClass(
                SpotLightComponent,
                Position,
                Direction,
                Color,
                Cutoff,
                Outer,
                Left,
                Right,
                Bottom,
                Top,
                Near,
                Far
            )
        };

        struct ENGINE_API RenderState
        {
            bool Transparent = false;    // switch to transparency, that will draw object in transparent passes
            bool Visible = true;         // switch visibility, that will draw or not draw object
            bool CastShadow = true;      // switch shadow casting, that will draw or not draw shadow of object
            bool HasSkeleton = false;    // switch skeleton state, that will draw it animated or not animated
        };
        Json(RenderState, Transparent, Visible, CastShadow, HasSkeleton)

        struct ENGINE_API GeometryComponent : Component, Geometry, RenderState
        {
            vector<ecs::Entity*> Entities;

            GeometryComponent(const Geometry& geometry) : Geometry(geometry) {}
            GeometryComponent(Geometry&& geometry) : Geometry(geometry) {}

            JsonClass(
                GeometryComponent,
                m_Tag
            )
        };

        struct ENGINE_API ModelComponent : Component, Model, RenderState
        {
            vector<ecs::Entity*> Entities;

            ModelComponent(const Model& model) : Model(model) {}
            ModelComponent(Model&& model) : Model(model) {}

            JsonClass(
                ModelComponent,
                m_Tag
            )
        };

        struct ENGINE_API SkeletonComponent : Component, Skeleton
        {
            SkeletonComponent(const Skeleton& skeleton) : Skeleton(skeleton) {}
            SkeletonComponent(Skeleton&& skeleton) : Skeleton(skeleton) {}

            JsonClass(
                SkeletonComponent,
                m_Tag
            )
        };

        struct ENGINE_API AnimationComponent : Component, Animation
        {
            AnimationComponent(const Animation& animation) : Animation(animation) {}
            AnimationComponent(Animation&& animation) : Animation(animation) {}

            JsonClass(
                AnimationComponent,
                m_Tag
            )
        };

        struct ENGINE_API ListenerComponent : Component
        {
            glm::vec3* Position;// = { 0.0f, 0.0f, 0.0f };
            glm::vec3 Velocity = { 0.0f, 0.0f, 0.0f };
            glm::vec3 Up = { 0.0f, 1.0f, 0.0f };
            glm::vec3* Look;

            JsonClass(
                ListenerComponent,
                m_Tag
            )
        };

        struct ENGINE_API AudioComponent : Component
        {
            AudioSource Source;

            eAudioState State = eAudioState::INITIAL;

            u32 BufferID = 0;

            AudioFile* File = nullptr;

            JsonClass(
                AudioComponent,
                m_Tag
            )
        };

        struct ENGINE_API StreamAudioComponent : Component
        {
            AudioSource Source;

            eAudioState State = eAudioState::INITIAL;

            u32 NumBuffers = 4;
            u32 BufferSamples = 8192;
            s64 CurrentFrame = 0;

            vector<u32> BufferID;
            vector<s16> Data;

            AudioFile* File = nullptr;

            JsonClass(
                StreamAudioComponent,
                m_Tag
            )
        };

        struct ENGINE_API VoiceComponent : Component
        {
            u32 SourceID = 0;

            eAudioState State = eAudioState::INITIAL;

            vector<u32> BufferID;

            u32 NumBuffers = 4;
            s32 Samples = 0;
            s32 Frames = 0;

            bool Recording = false;

            vector<s16> Data;

            JsonClass(
                VoiceComponent,
                m_Tag
            )
        };

        struct ENGINE_API RigidBodyComponent : Component, physics::sActor
        {
            RigidBodyComponent(physics::sActor* actor)
            {
                Actor = actor->Actor;
                Material = actor->Material;
                Shape = actor->Shape;
                ActorType = actor->ActorType;
                ShapeType = actor->ShapeType;
                LinearVelocity = actor->LinearVelocity;
                StaticFriction = actor->StaticFriction;
                DynamicFriction = actor->DynamicFriction;
                Restitution = actor->Restitution;
                ContactOffset = actor->ContactOffset;
                RestOffset = actor->RestOffset;
            }

            JsonClass(
                RigidBodyComponent,
                m_Tag
            )
        };
    }
}