#pragma once

#include <rendering/geometry/geometries.h>
#include <rendering/material/material.h>
#include <rendering/buffers.h>
#include <rendering/font/font.hpp>
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

        struct ENGINE_API CSpace2D final { u8 _pad0 = 0; };
        struct ENGINE_API CSpace3D final { u8 _pad0 = 0; };

        struct ENGINE_API CTag final
        {
            string Tag;
            CTag(const string& tag = {}) : Tag(tag) {}
            CTag(string&& tag) : Tag(tag) {}
        };
        JSON(CTag, Tag)

        struct ENGINE_API CTransform : sTransform
        {
            CTransform(const sTransform& transform = {}) : sTransform(transform) {}
            CTransform(sTransform&& transform) : sTransform(transform) {}
        };
        JSON(CTransform, Position, Rotation, Scale)

        struct ENGINE_API CVisible { bool Visible = true; };
        JSON(CVisible, Visible)

        struct ENGINE_API COpaque { bool Opaque = true; };
        JSON(COpaque, Opaque)

        struct ENGINE_API CTransparent { bool Transparent = true; };
        JSON(CTransparent, Transparent)

        struct ENGINE_API CHasShadow { bool HasShadow = true; };
        JSON(CHasShadow, HasShadow)

        struct ENGINE_API CCamera : cJson
        {
            // Position.z = -1 is a default valid value for 2D orthographic view
            // If Position.z >= 0, 2D geometry will not be shown on screen
            glm::vec3 Position = { 0, 0, -1 };
            glm::vec3 Front = { 0, 0, 0 };
            glm::vec3 Up = { 0, 1, 0 };

            JSON_CLASS(
                CCamera,
                Position,
                Front,
                Up
            )
        };

        struct ENGINE_API CPerspectiveCamera : CCamera, sPerspectiveMatrix
        {
            JSON_CLASS(
                CPerspectiveCamera,
                Position,
                Front,
                Up,
                FovDegree,
                AspectRatio,
                Near,
                Far
            )
        };

        struct ENGINE_API COrthoCamera : CCamera, sOrthoMatrix
        {
            JSON_CLASS(
                COrthoCamera,
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

        struct ENGINE_API CMaterial : cJson, sMaterial
        {
            CMaterial(const sMaterial& material = {}) : sMaterial(material) {}
            CMaterial(sMaterial&& material) : sMaterial(material) {}

            JSON_CLASS(
                CMaterial,
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

        struct ENGINE_API CDirectionalLight : cJson
        {
            glm::vec3 Color = { 1, 1, 1 };
            sOrthoMatrix Projection;
            sViewMatrix View;

            CDirectionalLight(const glm::vec3& position, const glm::vec3& color)
            {
                Color = color;
                Projection.Left = -20.0f;
                Projection.Right = 20.0f;
                Projection.Bottom = -20.0f;
                Projection.Top = 20.0f;
                Projection.Near = 0;
                Projection.Far = 100.0f;
                View.Position = position;
                View.Front = { 0, 0, 0 };
                View.Up = { 0, 1, 0 };
            }

            JSON_CLASS(
                CDirectionalLight,
                Color,
                Projection,
                View
            )
        };

        struct ENGINE_API CPointLight : cJson, sPointLightData
        {
            CPointLight(const glm::vec3& position, const glm::vec3& color)
            {
                Position = position;
                Color = color;
            }

            JSON_CLASS(
                CPointLight,
                Position,
                Color,
                Constant,
                Linear,
                Quadratic
            )
        };

        struct ENGINE_API CSpotLight : cJson
        {
            glm::vec3 Color;
            float Cutoff = 0.90f;
            float Outer = 0.95f;
            sPerspectiveMatrix Projection;
            sViewMatrix View;

            CSpotLight(
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

            JSON_CLASS(
                CSpotLight,
                Color,
                Cutoff,
                Outer,
                Projection,
                View
            )
        };

        struct ENGINE_API CGeometry : sGeometry, cJson
        {
            CGeometry(const sGeometry& geometry = {}) : sGeometry(geometry) {}
            CGeometry(sGeometry&& geometry) : sGeometry(geometry) {}

            JSON_CLASS(
                CGeometry,
                Vertices,
                Indices
            )
        };

        struct ENGINE_API CGeometryInfo : sGeometryInfo
        {
            CGeometryInfo(const sGeometryInfo& geometryInfo = {}) : sGeometryInfo(geometryInfo) {}
            CGeometryInfo(sGeometryInfo&& geometryInfo) : sGeometryInfo(geometryInfo) {}
        };

        struct ENGINE_API CSkeleton : sSkeleton, cJson
        {
            CSkeleton(const sSkeleton& skeleton = {}) : sSkeleton(skeleton) {}
            CSkeleton(sSkeleton&& skeleton) : sSkeleton(skeleton) {}

            JSON_CLASS(
                CSkeleton,
                Bones
            )
        };

        struct ENGINE_API CSkeletonInfo : sSkeletonInfo
        {
            CSkeletonInfo(const sSkeletonInfo& skeletonInfo = {}) : sSkeletonInfo(skeletonInfo) {}
            CSkeletonInfo(sSkeletonInfo&& skeletonInfo) : sSkeletonInfo(skeletonInfo) {}
        };

        struct ENGINE_API CAnimation : cJson
        {
            vector<cEntity*> Entities;
            sSkeleton Skeleton;
            vector<sAnimation> Animations;

            JSON_CLASS(
                CAnimation,
                Animations
            )
        };

        struct ENGINE_API CAudio : cJson
        {
            sAudioSource Source;
            eAudioState State = eAudioState::INITIAL;
            u32 BufferID = 0;
            sAudioFile* File = nullptr;

            JSON_CLASS(
                CAudio,
                Source,
                State,
                BufferID
            )
        };

        struct ENGINE_API CStreamAudio : cJson
        {
            sAudioSource Source;
            eAudioState State = eAudioState::INITIAL;

            u32 NumBuffers = 4;
            u32 BufferSamples = 8192;
            s64 CurrentFrame = 0;

            vector<u32> BufferID;
            vector<s16> Data;

            sAudioFile* File = nullptr;

            JSON_CLASS(
                CStreamAudio,
                Source,
                State,
                NumBuffers,
                BufferSamples,
                CurrentFrame,
                BufferID
            )
        };

        struct ENGINE_API CVoice : cJson
        {
            u32 SourceID = 0;
        
            eAudioState State = eAudioState::INITIAL;
        
            vector<u32> BufferID;
            vector<s16> Data;
        
            JSON_CLASS(
                CVoice,
                SourceID,
                State,
                BufferID,
                Data
            )
        };

        struct ENGINE_API CPhysicsActor : physics::sActor, cJson
        {
            CPhysicsActor(physics::sActor* actor)
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

            JSON_CLASS(
                CPhysicsActor,
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

        struct ENGINE_API CLabel : sWidget
        {
            sFont* Font = nullptr;
            string Text;
            glm::vec4 Color = { 1, 1, 1, 1 };
            bool FillFrame = true;

            CLabel(sFont* font, const string& text = "", const glm::vec4& color = { 1, 1, 1, 1 })
            : Font(font), Text(text), Color(color) {}

            XML_CLASS(
                "Label",
                Text,
                Color,
                FillFrame
            )
        };

        struct ENGINE_API CContainer : sWidget
        {
            
        };

        struct ENGINE_API CButton : sWidget
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

            XML_CLASS(
                "Button",
                Color,
                ColorHover,
                ColorPressed,
                EnableTexture,
                MousePressed,
                FillFrame
            )
        };

        struct ENGINE_API CField : sWidget
        {
            sFont* Font = nullptr;
            string Text;
            glm::vec4 Color = { 1, 1, 1, 1 };
            bool FillFrame = true;

            CField(sFont* font, const string& text = "", const glm::vec4& color = { 1, 1, 1, 1 })
            : Font(font), Text(text), Color(color) {}

            XML_CLASS(
                "Field",
                Text,
                Color,
                FillFrame
            )
        };

    }
}