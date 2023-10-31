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

        struct ENGINE_API sCPerspectiveCamera : sCCamera, PerspectiveMatrix
        {
            JsonClass(
                sCPerspectiveCamera,
                m_Tag
            )
        };

        struct ENGINE_API sCOrthoCamera : sCCamera, OrthoMatrix
        {
            JsonClass(
                sCOrthoCamera,
                m_Tag
            )
        };

        struct ENGINE_API sCText2D : sComponent
        {
            string Text;
            render::Font* Font;

            sCText2D(const string& text, render::Font* font) : Text(text), Font(font) {}

            JsonClass(
                sCText2D,
                Text
            )
        };

        struct ENGINE_API sCText3D : sComponent
        {
            string Text;
            render::Font* Font;

            sCText3D(const string& text, render::Font* font) : Text(text), Font(font) {}

            JsonClass(
                sCText3D,
                Text
            )
        };

        struct ENGINE_API sCMaterial : sComponent, Material
        {
            sCMaterial(const Material& material) : Material(material) {}
            sCMaterial(Material&& material) : Material(material) {}

            JsonClass(
                sCMaterial,
                m_Tag
            )
        };

        struct ENGINE_API sCDirectionalLight : sComponent, DirectLightData, OrthoMatrix
        {
            sCDirectionalLight(const glm::vec3& position, const glm::vec3& color)
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
                sCDirectionalLight,
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

        struct ENGINE_API sCPointLight : sComponent, PointLightData
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

        struct ENGINE_API sCSpotLight : sComponent, SpotLightData, OrthoMatrix
        {
            sCSpotLight(const glm::vec3& position, const glm::vec3& color)
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
                sCSpotLight,
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

        struct ENGINE_API sRenderState
        {
            bool Transparent = false;    // switch to transparency, that will draw object in transparent passes
            bool Visible = true;         // switch visibility, that will draw or not draw object
            bool CastShadow = true;      // switch shadow casting, that will draw or not draw shadow of object
        };
        Json(sRenderState, Transparent, Visible, CastShadow)

        struct ENGINE_API sCGeometry : sComponent, Geometry, sRenderState
        {
            vector<ecs::cEntity*> Entities;

            sCGeometry(const Geometry& geometry) : Geometry(geometry) {}
            sCGeometry(Geometry&& geometry) : Geometry(geometry) {}

            JsonClass(
                sCGeometry,
                m_Tag
            )
        };

        struct ENGINE_API sCModel : sComponent, Model, sRenderState
        {
            vector<ecs::cEntity*> Entities;

            sCModel(const Model& model) : Model(model) {}
            sCModel(Model&& model) : Model(model) {}

            JsonClass(
                sCModel,
                m_Tag
            )
        };

        struct ENGINE_API sCSkeletonModel : sComponent, Model, sRenderState
        {
            vector<ecs::cEntity*> Entities;
            anim::sSkeleton Skeleton;
            anim::sAnimation Animation;

            sCSkeletonModel(const Model& model) : Model(model) {}
            sCSkeletonModel(Model&& model) : Model(model) {}

            JsonClass(
                sCSkeletonModel,
                Skeleton,
                Animation
            )
        };

        struct ENGINE_API sCListener : sComponent
        {
            glm::vec3* Position;// = { 0.0f, 0.0f, 0.0f };
            glm::vec3 Velocity = { 0.0f, 0.0f, 0.0f };
            glm::vec3 Up = { 0.0f, 1.0f, 0.0f };
            glm::vec3* Look;

            JsonClass(
                sCListener,
                m_Tag
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

        struct ENGINE_API sCRidigBody : sComponent, physics::sActor
        {
            sCRidigBody(physics::sActor* actor)
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
                sCRidigBody,
                m_Tag
            )
        };
    }
}