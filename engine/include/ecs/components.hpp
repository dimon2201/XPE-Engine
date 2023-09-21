#pragma once

#include <ecs/component.h>

#include <rendering/materials/material.h>
#include <rendering/font/font.hpp>
#include <rendering/buffers/light_buffers.h>

#include <anim/skin.h>
#include <anim/skelet.h>

#include <audio/core/openal_context.h> // temporarily

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

        struct ENGINE_API DirectLightComponent : Component, DirectLightBufferData
        {
            JsonClass(
                DirectLightComponent,
                Position,
                Color
            )
        };

        struct ENGINE_API PointLightComponent : Component, PointLightBufferData
        {
            JsonClass(
                PointLightComponent,
                Position,
                Color,
                Constant,
                Linear,
                Quadratic
            )
        };

        struct ENGINE_API SpotLightComponent : Component, SpotLightBufferData
        {
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

            JsonClass(
                AudioFileComponent,
                m_Tag
            )
        };

        struct ENGINE_API SourceAudioComponent : Component
        {
            u32 SourceID = 0;
            s32 State = 0;

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

            u32 Status = AUDIO_INITIAL;

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

            u32 Status = AUDIO_INITIAL;

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
            s32 State = AUDIO_INITIAL;

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