#pragma once

#include <rendering/geometry/geometries.h>
#include <rendering/material/material.h>
#include <rendering/buffers/light_buffers.h>
#include <rendering/widget_manager.h>

#include <anim/skeleton.h>

#include <audio/core/context.h>

namespace xpe
{
    namespace physics
    {
        struct sActor;
    }

    namespace ecs
    {
        using namespace core;
        using namespace render;
        using namespace math;
        using namespace anim;
        using namespace audio;
        using namespace physics;

        struct ENGINE_API sCCamera : sComponent
        {
            // Position.z = -1 is a default valid value for 2D orthographic view
            // If Position.z >= 0, 2D geometry will not be shown on screen
            glm::vec3 Position = { 0, 0, -1 };
            glm::vec3 Front = { 0, 0, 0 };
            glm::vec3 Up = { 0, 1, 0 };

            JsonClass(
                sCCamera,
                Position,
                Front,
                Up
            )
        };

        struct ENGINE_API sCPerspectiveCamera : sCCamera, sPerspectiveMatrix
        {
            JsonClass(
                sCPerspectiveCamera,
                Position,
                Front,
                Up,
                FovDegree,
                AspectRatio,
                Near,
                Far
            )
        };

        struct ENGINE_API sCOrthoCamera : sCCamera, sOrthoMatrix
        {
            JsonClass(
                sCOrthoCamera,
                Position,
                Front,
                Up,
                Left,
                Right,
                Bottom,
                Top,
                Near,
                Far
            )
        };

        struct ENGINE_API sCMaterial : sComponent, sMaterial
        {
            sCMaterial(const sMaterial& material) : sMaterial(material) {}
            sCMaterial(sMaterial&& material) : sMaterial(material) {}

            JsonClass(
                sCMaterial,
                Albedo,
                EnableAlbedoMap,
                EnableNormalMap,
                EnableParallaxMap,
                ParallaxHeightScale,
                ParallaxMinLayers,
                ParallaxMaxLayers,
                Metallness,
                EnableMetalMap,
                Roughness,
                EnableRoughnessMap,
                AO,
                EnableAOMap,
                Emission,
                EnableEmissionMap
            )
        };

        struct ENGINE_API sCDirectionalLight : sComponent
        {
            glm::vec3 Color = { 1, 1, 1 };
            sOrthoMatrix Projection;
            sViewMatrix View;

            sCDirectionalLight(const glm::vec3& position, const glm::vec3& color)
            {
                Color = color;
                Projection.Left = -35.0f;
                Projection.Right = 35.0f;
                Projection.Bottom = -35.0f;
                Projection.Top = 35.0f;
                Projection.Near = 0.1f;
                Projection.Far = 75.0f;
                View.Position = position;
                View.Front = { 0, 0, 0 };
                View.Up = { 0, 1, 0 };
            }

            JsonClass(
                sCDirectionalLight,
                Color,
                Projection,
                View
            )
        };

        struct ENGINE_API sCPointLight : sComponent, sPointLightData
        {
            sCPointLight(const glm::vec3& position, const glm::vec3& color)
            {
                Position = position;
                Color = color;
            }

            JsonClass(
                sCPointLight,
                Position,
                Color,
                Constant,
                Linear,
                Quadratic
            )
        };

        struct ENGINE_API sCSpotLight : sComponent
        {
            glm::vec3 Color;
            float Cutoff = 0.90f;
            float Outer = 0.95f;
            sPerspectiveMatrix Projection;
            sViewMatrix View;

            sCSpotLight(
                    const glm::vec3& position = { 0, 0, 0 },
                    const glm::vec3& direction = { 0, 0, 0 },
                    const glm::vec3& color = { 1, 1, 1 }
            ) {
                Color = color;
                Projection.FovDegree = 90.0f;
                Projection.AspectRatio = 1.0;
                Projection.Near = 0.1f;
                Projection.Far = 75.0f;
                View.Position = position;
                View.Front = direction;
                View.Up = { 0, 1, 0 };
            }

            JsonClass(
                sCSpotLight,
                Color,
                Cutoff,
                Outer,
                Projection,
                View
            )
        };

        struct ENGINE_API sRenderState
        {
            bool Transparent = false;    // switch to transparency, that will draw object in transparent passes
            bool CastShadow = true;      // switch shadow casting, that will draw or not draw shadow of object
        };
        Json(sRenderState, Transparent, CastShadow)

        struct ENGINE_API sCGeometry : sComponent, sGeometry, sRenderState
        {
            vector<cEntity*> Entities;

            sCGeometry(const sGeometry& geometry) : sGeometry(geometry) {}
            sCGeometry(sGeometry&& geometry) : sGeometry(geometry) {}

            JsonClass(
                sCGeometry,
                PrimitiveTopology,
                VertexOffset,
                IndexOffset,
                Vertices,
                Indices
            )
        };

        struct ENGINE_API sCSkeletonModel : sComponent, sGeometry, sRenderState
        {
            vector<cEntity*> Entities;  
            sSkeleton Skeleton;
            vector<sAnimation> Animations;

            sCSkeletonModel(const sGeometry& geometry) : sGeometry(geometry) {}
            sCSkeletonModel(sGeometry&& geometry) : sGeometry(geometry) {}

            JsonClass(
                sCSkeletonModel,
                Skeleton,
                Animations,
                PrimitiveTopology,
                Vertices,
                Indices
            )
        };

        struct ENGINE_API sCAudio : sComponent
        {
            sAudioSource Source;
            eAudioState State = eAudioState::INITIAL;
            u32 BufferID = 0;
            sAudioFile* File = nullptr;

            JsonClass(
                sCAudio,
                Source,
                State,
                BufferID
            )
        };

        struct ENGINE_API sCStreamAudio : sComponent
        {
            sAudioSource Source;
            eAudioState State = eAudioState::INITIAL;

            u32 NumBuffers = 4;
            u32 BufferSamples = 8192;
            s64 CurrentFrame = 0;

            vector<u32> BufferID;
            vector<s16> Data;

            sAudioFile* File = nullptr;

            JsonClass(
                sCStreamAudio,
                Source,
                State,
                NumBuffers,
                BufferSamples,
                CurrentFrame,
                BufferID
            )
        };

        struct ENGINE_API sCVoice : sComponent
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
                sCVoice,
                SourceID,
                State,
                BufferID,
                NumBuffers,
                Samples,
                Frames,
                Recording,
                Data
            )
        };

        struct ENGINE_API sCPhysicsActor : sComponent, physics::sActor
        {
            sCPhysicsActor(physics::sActor* actor)
            {
                Actor = actor->Actor;
                Material = actor->Material;
                Shape = actor->Shape;
                ActorType = actor->ActorType;
                ShapeDesc = actor->ShapeDesc;
                LinearVelocity = actor->LinearVelocity;
                StaticFriction = actor->StaticFriction;
                DynamicFriction = actor->DynamicFriction;
                Restitution = actor->Restitution;
                ContactOffset = actor->ContactOffset;
                RestOffset = actor->RestOffset;
            }

            JsonClass(
                sCPhysicsActor,
                ActorType,
                ShapeDesc,
                LinearVelocity,
                StaticFriction,
                DynamicFriction,
                Restitution,
                ContactOffset,
                RestOffset
            )
        };

        struct ENGINE_API sCLabel : sComponent, sWidget
        {
            sFont* Font = nullptr;
            string Text;
            glm::vec4 Color = { 1, 1, 1, 1 };
            bool FillFrame = true;

            sCLabel(sFont* font, const string& text = "", const glm::vec4& color = { 1, 1, 1, 1 })
            : Font(font), Text(text), Color(color) {}

            XmlClass(
                "Label",
                Space,
                Text,
                Color,
                FillFrame
            )

            JsonClass(
                sCLabel,
                Color
            )
        };

        struct ENGINE_API sCButton : sComponent, sWidget
        {
            typedef void (*fHovered)();
            typedef void (*fPressed)();

            glm::vec4 Color = { 1, 1, 1, 1 };
            glm::vec4 ColorHover = { 0.5, 0.5, 0.5, 1.0 };
            glm::vec4 ColorPressed = { 0.25, 0.25, 0.25, 1.0 };
            bool EnableTexture = false;
            sAtlas::sLocation AtlasLocation;
            bool EnablePress = true;
            eMouse MousePressed = eMouse::ButtonLeft;
            bool FillFrame = true;
            fHovered Hovered = nullptr;
            fPressed Pressed = nullptr;

            XmlClass(
                "Button",
                Space,
                Color,
                ColorHover,
                ColorPressed,
                EnableTexture,
                MousePressed,
                FillFrame
            )

            JsonClass(
                sCButton,
                Color
            )
        };

        struct ENGINE_API sCField : sComponent, sWidget
        {
            sFont* Font = nullptr;
            string Text;
            glm::vec4 Color = { 1, 1, 1, 1 };
            bool FillFrame = true;

            sCField(sFont* font, const string& text = "", const glm::vec4& color = { 1, 1, 1, 1 })
            : Font(font), Text(text), Color(color) {}

            XmlClass(
                "Field",
                Space,
                Text,
                Color,
                FillFrame
            )

            JsonClass(
                sCField,
                Color
            )
        };

    }
}