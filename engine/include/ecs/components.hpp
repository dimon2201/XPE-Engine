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
                m_Tag
            )
        };

        struct ENGINE_API sCOrthoCamera : sCCamera, sOrthoMatrix
        {
            JsonClass(
                sCOrthoCamera,
                m_Tag
            )
        };

        struct ENGINE_API sCText2D : sComponent
        {
            string Text;
            sFont* Font;

            sCText2D(const string& text, sFont* font) : Text(text), Font(font) {}

            JsonClass(
                sCText2D,
                Text
            )
        };

        struct ENGINE_API sCText3D : sComponent
        {
            string Text;
            sFont* Font;

            sCText3D(const string& text, sFont* font) : Text(text), Font(font) {}

            JsonClass(
                sCText3D,
                Text
            )
        };

        struct ENGINE_API sCMaterial : sComponent, sMaterial
        {
            sCMaterial(const sMaterial& material) : sMaterial(material) {}
            sCMaterial(sMaterial&& material) : sMaterial(material) {}

            JsonClass(
                sCMaterial,
                m_Tag
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
                View.Up = { 0, 0, 1 };
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

        struct ENGINE_API sCSpotLight : sComponent, sSpotLightData
        {
            sCSpotLight(const glm::vec3& position, const glm::vec3& color)
            {
                Position = position;
                Color = color;
            }

            JsonClass(
                sCSpotLight,
                Position,
                Direction,
                Color,
                Cutoff,
                Outer
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
                m_Tag
            )
        };

        struct ENGINE_API sCModel : sComponent, sModel, sRenderState
        {
            vector<cEntity*> Entities;

            sCModel(const sModel& model) : sModel(model) {}
            sCModel(sModel&& model) : sModel(model) {}

            JsonClass(
                sCModel,
                m_Tag
            )
        };

        struct ENGINE_API sCSkeletonModel : sComponent, sModel, sRenderState
        {
            vector<cEntity*> Entities;  
            sSkeleton Skeleton;
            sAnimation Animation;

            sCSkeletonModel(const sModel& model) : sModel(model) {}
            sCSkeletonModel(sModel&& model) : sModel(model) {}

            JsonClass(
                sCSkeletonModel,
                Skeleton,
                Animation
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
                m_Tag
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
                m_Tag
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
                m_Tag
            )
        };

        struct ENGINE_API MYsCVoice : sComponent
        {
            sAudioSource Source;

            //struct voice Voice;
            // /\ 
            // ||
            //vector<u32> BufferID;
            //vector<s16> Data;

            JsonClass(
                MYsCVoice,
                m_Tag
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
                m_Tag
            )
        };
    }
}