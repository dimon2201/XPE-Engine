#pragma once

#include <ecs/ecs.h>

#include <rendering/geometry/geometries.h>
#include <rendering/material/material.h>
#include <rendering/font/font.hpp>
#include <rendering/buffers/light_buffers.h>

#include <anim/skeleton.h>

#include <audio/core/context.h>

namespace xpe
{
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
            string Text;
            Ref<Font> Font;
            string FontResFilepath;

            Text2DComponent(const string& tag, const Ref<render::Font>& font, const string& text)
            : Component(tag), Font(font), Text(text) {}

            JsonClass(
                Text2DComponent,
                Text,
                FontResFilepath
            )
        };

        struct ENGINE_API Text3DComponent : Component
        {
            string Text;
            Ref<Font> Font;
            string FontResFilepath;

            Text3DComponent(const string& tag, const Ref<render::Font>& font, const string& text)
            : Component(tag), Font(font), Text(text) {}

            JsonClass(
                Text3DComponent,
                Text,
                FontResFilepath
            )
        };

        struct ENGINE_API MaterialComponent : Component
        {
            Ref<render::Material> Material;

            MaterialComponent(const string& tag, const Ref<render::Material>& material)
            : Component(tag), Material(material) {}

            JsonClass(
                MaterialComponent,
                m_Tag
            )
        };

        struct ENGINE_API DirectLightComponent : Component, DirectLightData
        {
            OrthoMatrix Projection;

            DirectLightComponent(const string& tag, const glm::vec3& position, const glm::vec3& color)
            : Component(tag) {
                Position = position;
                Color = color;
            }

            JsonClass(
                DirectLightComponent,
                Position,
                Color
            )
        };

        struct ENGINE_API PointLightComponent : Component, PointLightData
        {
            PointLightComponent(const string& tag, const glm::vec3& position, const glm::vec3& color)
            : Component(tag) {
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

        struct ENGINE_API SpotLightComponent : Component, SpotLightData
        {
            OrthoMatrix Projection;

            SpotLightComponent(const string& tag, const glm::vec3& position, const glm::vec3& color)
            : Component(tag) {
                Position = position;
                Color = color;
            }

            JsonClass(
                SpotLightComponent,
                Position,
                Direction,
                Color,
                Cutoff,
                Outer
            )
        };

        struct ENGINE_API RenderState
        {
            bool Transparent = false;    // switch to transparency, that will draw object in transparent passes
            bool Visible = true;         // switch visibility, that will draw or not draw object
            bool EmbeddedShadow = false; // switch to embedded shadow, that casts shadow only once and doesn't update
        };
        Json(RenderState, Transparent, Visible, EmbeddedShadow)

        struct ENGINE_API GeometryComponent : Component, RenderState
        {
            Ref<render::Geometry> Geometry;
            vector<ecs::Entity*> Entities;

            GeometryComponent(const string& tag, const Ref<render::Geometry>& geometry, const vector<ecs::Entity*>& entities = {})
            : Component(tag), Geometry(geometry), Entities(entities) {}

            JsonClass(
                GeometryComponent,
                m_Tag
            )
        };

        struct ENGINE_API ModelComponent : Component, RenderState
        {
            Ref<render::Model> Model;
            vector<ecs::Entity*> Entities;

            ModelComponent(const string& tag, const Ref<render::Model>& model, const vector<ecs::Entity*>& entities = {})
            : Component(tag), Model(model), Entities(entities) {}

            JsonClass(
                ModelComponent,
                m_Tag
            )
        };

        struct ENGINE_API SkinModelComponent : Component, RenderState
        {
            Ref<render::Model> Model;
            Ref<anim::Skeleton> Skeleton;
            vector<ecs::Entity*> Entities;

            SkinModelComponent(const string& tag, const Ref<render::Model>& model, const Ref<anim::Skeleton>& skeleton, const vector<ecs::Entity*>& entities = {})
            : Component(tag), Model(model), Skeleton(skeleton), Entities(entities) {}

            JsonClass(
                SkinModelComponent,
                m_Tag
            )
        };

        struct ENGINE_API SkeletonAnimationComponent : Component
        {
            Ref<anim::Skeleton> Skeleton;
            Ref<anim::Animation> Animation;
            bool Play = false;

            SkeletonAnimationComponent(const string& tag, const Ref<anim::Skeleton>& skeleton, const Ref<anim::Animation>& animation)
            : Component(tag), Skeleton(skeleton), Animation(animation) {}

            JsonClass(
                    SkeletonAnimationComponent,
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

        struct ENGINE_API AudioFileComponent : Component
        {
            Ref<AudioFile> File = nullptr;

            AudioFileComponent(const string& tag, const Ref<AudioFile>& file)
            : Component(tag), File(file) {}

            JsonClass(
                AudioFileComponent,
                m_Tag
            )
        };

        struct ENGINE_API SourceAudioComponent : Component
        {
            u32 SourceID = 0;

            eAudioState State = eAudioState::INITIAL;

            glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
            glm::vec3 Velocity = { 0.0f, 0.0f, 0.0f };

            f32 Gain = 1;                   // affects audio volume
            f32 Pitch = 1;                  // affects playback speed 

            f32 RefDistance = 1.0f;         // Distance at which the sound is heard clearly   
            f32 MaxDistance = 100.0f;       // Maximum distance at which the sound is heard
            f32 RollOffFactor = 1.0f;       // Volume reduction factor with distance
            f32 ConeInnerAngle = 180.0f;    // Angle of the inner zone of the cone
            f32 ConeOuterAngle = 180.0f;    // Angle of the outer zone of the cone                            

            bool Looping = false;

            JsonClass(
                SourceAudioComponent,
                SourceID,
                State,
                Position, 
                Velocity, 
                Gain, 
                Pitch, 
                RefDistance, 
                MaxDistance,
                m_Tag
            )
        };

        struct ENGINE_API AudioComponent : Component
        {
            SourceAudioComponent* Source = nullptr;

            eAudioState State = eAudioState::INITIAL;

            u32 BufferID = 0;
            Ref<AudioFile> File = nullptr;

            JsonClass(
                AudioComponent,
                m_Tag
            )
        };

        struct ENGINE_API StreamAudioComponent : Component
        {
            SourceAudioComponent* Source = nullptr;

            eAudioState State = eAudioState::INITIAL;

            u32 NumBuffers = 4;
            u32 BufferSamples = 8192;
            s64 CurrentFrame = 0;

            vector<u32> BufferID;
            vector<short> Data;
            Ref<AudioFile> File = nullptr;

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

            vector<short> Data;

            JsonClass(
                VoiceComponent,
                m_Tag
            )
        };

    }
}