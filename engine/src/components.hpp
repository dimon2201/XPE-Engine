#pragma once

#include <ecs.hpp>
#include <material_manager.hpp>
#include <font_manager.hpp>
#include <math_manager.hpp>
#include <physics_manager.hpp>
#include <audio_manager.hpp>

namespace xpe
{
    namespace ecs
    {
        struct ENGINE_API CSpace2D final { u8 _pad0 = 0; };
        struct ENGINE_API CSpace3D final { u8 _pad0 = 0; };

        struct ENGINE_API CTag final
        {
            string Tag;
            CTag(const string& tag = {}) : Tag(tag) {}
            CTag(string&& tag) : Tag(tag) {}
        };
        JSON(CTag, Tag)

        struct ENGINE_API CTransform : public sModelMatrix
        {
            bool UpdatePhysics = false; // update flag for physics manager
        };
        JSON(CTransform, Position, Rotation, Scale, UpdatePhysics)

        struct ENGINE_API CVisible { bool Visible = true; };
        JSON(CVisible, Visible)

        struct ENGINE_API COpaque { bool Opaque = true; };
        JSON(COpaque, Opaque)

        struct ENGINE_API CTransparent { bool Transparent = true; };
        JSON(CTransparent, Transparent)

        struct ENGINE_API CShadow { bool HasShadow = true; };
        JSON(CShadow, HasShadow)

        enum class eCameraMode
        {
            GAME = -1,
            EDITOR = 1,
        };

        JSON_ENUM(eCameraMode,
        {
            { eCameraMode::GAME,   "GAME" },
            { eCameraMode::EDITOR, "EDITOR" }
        })

        struct ENGINE_API CPerspectiveCamera : public cJson
        {
            // Position.z = -1 is a default valid value for 2D orthographic view
            // If Position.z >= 0, 2D geometry will not be shown on screen
            glm::vec3 Position = { 0, 0, -1 };
            glm::vec3 Front = { 0, 0, 0 };
            glm::vec3 Up = { 0, 1, 0 };
            glm::vec3 Rotation = { 0, 0, 0 }; // (pitch, yaw, roll)

            sPerspectiveMatrix Projection;

            sViewport Viewport;
            float Exposure = 1.0f;
            float Gamma = 2.2f;

            eKey KeyMoveForward = eKey::W;
            eKey KeyMoveLeft = eKey::A;
            eKey KeyMoveBackward = eKey::S;
            eKey KeyMoveRight = eKey::D;

            float MoveSpeed = 0.1f;
            float ZoomAcceleration = 10.0f;
            float PanAcceleration = 10.0f;
            float HorizontalSensitivity = 0.01f;
            float VerticalSensitivity = 0.01f;
            float MinFovDegree = 1.0f;
            float MaxFovDegree = 45.0f;

            eCameraMode Mode = eCameraMode::GAME;
            bool EnableLook = true;

            JSON_CLASS(
                CPerspectiveCamera,
                Position,
                Front,
                Up,
                Rotation,
                Projection,
                KeyMoveForward,
                KeyMoveLeft,
                KeyMoveBackward,
                KeyMoveRight,
                MoveSpeed,
                ZoomAcceleration,
                PanAcceleration,
                HorizontalSensitivity,
                VerticalSensitivity,
                MinFovDegree,
                MaxFovDegree,
                Mode,
                EnableLook
            )
        };

        struct ENGINE_API COrthoCamera : public cJson
        {
            // Position.z = -1 is a default valid value for 2D orthographic view
            // If Position.z >= 0, 2D geometry will not be shown on screen
            glm::vec3 Position = { 0, 0, -1 };
            glm::vec3 Front = { 0, 0, 0 };
            glm::vec3 Up = { 0, 1, 0 };
            glm::vec3 Rotation = { 0, 0, 0 }; // (pitch, yaw, roll)

            sOrthoMatrix Projection;

            sViewport Viewport;
            float Exposure = 1.0f;
            float Gamma = 2.2f;

            eKey KeyMoveForward = eKey::W;
            eKey KeyMoveLeft = eKey::A;
            eKey KeyMoveBackward = eKey::S;
            eKey KeyMoveRight = eKey::D;

            float MoveSpeed = 0.1f;
            float ZoomAcceleration = 10.0f;
            float PanAcceleration = 10.0f;
            float HorizontalSensitivity = 0.01f;
            float VerticalSensitivity = 0.01f;

            eCameraMode Mode = eCameraMode::GAME;
            bool EnableLook = true;

            JSON_CLASS(
                COrthoCamera,
                Position,
                Front,
                Up,
                Rotation,
                Projection,
                KeyMoveForward,
                KeyMoveLeft,
                KeyMoveBackward,
                KeyMoveRight,
                MoveSpeed,
                ZoomAcceleration,
                PanAcceleration,
                HorizontalSensitivity,
                VerticalSensitivity,
                Mode,
                EnableLook
            )
        };

        struct ENGINE_API CMaterial : public cJson, public sMaterialData
        {
            static const sMaterialFormat k_AlbedoFormat;
            static const sMaterialFormat k_NormalFormat;
            static const sMaterialFormat k_ParallaxFormat;
            static const sMaterialFormat k_MetalFormat;
            static const sMaterialFormat k_RoughnessFormat;
            static const sMaterialFormat k_AoFormat;
            static const sMaterialFormat k_EmissionFormat;

            cTextureLayer* AlbedoMap = nullptr;
            cTextureLayer* NormalMap = nullptr;
            cTextureLayer* ParallaxMap = nullptr;
            cTextureLayer* MetalMap = nullptr;
            cTextureLayer* RoughnessMap = nullptr;
            cTextureLayer* AOMap = nullptr;
            cTextureLayer* EmissionMap = nullptr;

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

        struct ENGINE_API CMaterialInfo final
        {
            u32 MaterialIndex = 0;
        };

        struct ENGINE_API CDirectionalLight : public cJson
        {
            glm::vec3 Color = { 1, 1, 1 };
            glm::vec3 Intensity = { 1, 1, 1 };
            sOrthoMatrix Projection;
            sViewMatrix View;

            CDirectionalLight() = default;

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
                Intensity,
                Projection,
                View
            )
        };

        struct ENGINE_API CPointLight : public cJson, public sPointLightData
        {
            glm::vec3 Intensity = { 1, 1, 1 };

            CPointLight() = default;

            CPointLight(const glm::vec3& position, const glm::vec3& color)
            {
                Position = position;
                Color = color;
            }

            JSON_CLASS(
                CPointLight,
                Position,
                Color,
                Intensity,
                Constant,
                Linear,
                Quadratic
            )
        };

        struct ENGINE_API CSpotLight : public cJson
        {
            glm::vec3 Color;
            glm::vec3 Intensity = { 1, 1, 1 };
            float Cutoff = 0.90f;
            float Outer = 0.95f;
            sPerspectiveMatrix Projection;
            sViewMatrix View;

            CSpotLight(
                    const glm::vec3& position = { 0, 0, 0 },
                    const glm::vec3& direction = { 0, 0, 0 },
                    const glm::vec3& color = { 1, 1, 1 }
            )
            {
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
                Intensity,
                Cutoff,
                Outer,
                Projection,
                View
            )
        };

        struct ENGINE_API CGeometry : cJson
        {
            ePrimitiveTopology PrimitiveTopology = ePrimitiveTopology::DEFAULT;
            vector<sVertex> Vertices;
            vector<u32> Indices;
            vector<CGeometry> Children;

            inline CGeometry& operator [](u32 i) { return Children[i]; }

            JSON_CLASS(
                CGeometry,
                PrimitiveTopology,
                Vertices,
                Indices,
                Children
            )
        };

        struct ENGINE_API CGeometryInfo
        {
            ePrimitiveTopology PrimitiveTopology = ePrimitiveTopology::DEFAULT;
            usize VertexOffset = 0;
            usize IndexOffset = 0;
            usize VertexCount = 0;
            usize IndexCount = 0;
            vector<EntityID> Entities;
        };

        struct ENGINE_API sTriangle : public CGeometry
        {
            sTriangle();
        };

        struct ENGINE_API sLine : public CGeometry
        {
            sLine();
        };

        struct ENGINE_API sQuad : public CGeometry
        {
            sQuad();
        };

        struct ENGINE_API sCube : public CGeometry
        {
            sCube();
        };

        struct ENGINE_API sPlane : public CGeometry
        {
            sPlane(s32 size = 16);
        };

        struct ENGINE_API sSphere : public CGeometry
        {
            glm::vec3 Center;
            float Radius;

            sSphere(s32 xSegments = 64, s32 ySegments = 64);
        };

        struct ENGINE_API sKeyPosition final
        {
            float Timestamp;
            glm::vec3 Position;

            sKeyPosition() = default;
            sKeyPosition(float timestamp, const glm::vec3& position) : Timestamp(timestamp), Position(position) {}
        };
        JSON(sKeyPosition, Timestamp, Position)

        struct ENGINE_API sKeyRotate final
        {
            float Timestamp;
            glm::quat Orientation;

            sKeyRotate() = default;
            sKeyRotate(float timestamp, const glm::quat& orientation) : Timestamp(timestamp), Orientation(orientation) {}
        };
        JSON(sKeyRotate, Timestamp, Orientation)

        struct ENGINE_API sKeyScale final
        {
            float Timestamp;
            glm::vec3 Scale;

            sKeyScale() = default;
            sKeyScale(float timestamp, const glm::vec3& scale) : Timestamp(timestamp), Scale(scale) {}
        };
        JSON(sKeyScale, Timestamp, Scale)

        struct ENGINE_API sBone final
        {
            s32 ID;
            string Name;
            glm::mat4 Offset;
            glm::mat4 Transform = glm::mat4(1.0);
            vector<sKeyPosition> KeyPositions;
            vector<sKeyRotate> KeyRotations;
            vector<sKeyScale> KeyScales;
        };
        JSON(sBone, ID, Name, Offset, Transform, KeyPositions, KeyRotations, KeyScales)

        struct ENGINE_API CSkeleton : cJson
        {
            unordered_map<string, sBone> Bones;

            JSON_CLASS(
                CSkeleton,
                Bones
            )
        };

        struct ENGINE_API CSkeletonInfo final
        {
            u32 SkeletonIndex = 0;
        };

        struct ENGINE_API sAnimationNode final
        {
            string Name;
            glm::mat4 Transform;
            vector<sAnimationNode> Children;
        };
        JSON(sAnimationNode, Name, Transform, Children)

        struct ENGINE_API CAnimation : cJson
        {
            vector<cEntity*> Entities;
            CSkeleton* Skeleton = nullptr;
            bool Play;
            float Duration;
            int TicksPerSecond;
            sAnimationNode Root;

            JSON_CLASS(
                CAnimation,
                Duration,
                Play,
                TicksPerSecond,
                Root
            )
        };

        struct ENGINE_API sSkybox final
        {
            CGeometry Geometry;
            CGeometryInfo GeometryInfo;
            cTexture* Texture = nullptr;
        };
        JSON(sSkybox, Geometry)

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

        struct ENGINE_API CActor : public cJson
        {
            PxActor* Actor = nullptr;
            PxMaterial* Material = nullptr;
            PxShape* Shape = nullptr;
            eActorType ActorType = eActorType::NONE;
            sShapeDescriptor ShapeDesc = sShapeDescriptor(eShapeType::NONE);
            glm::vec3 LinearVelocity = glm::vec3(0.0f);
            f32 StaticFriction = 0.0f;
            f32 DynamicFriction = 0.0f;
            f32 Restitution = 0.0f;
            f32 ContactOffset = 0.0f;
            f32 RestOffset = 0.0f;

            CActor() = default;

            CActor(
                PxActor* actor,
                PxMaterial* material,
                PxShape* shape,
                const eActorType& actorType,
                sShapeDescriptor* shapeDesc,
                const glm::vec3& linearVelocity,
                f32 staticFriction,
                f32 dynamicFriction,
                f32 restitution,
                f32 contactOffset,
                f32 restOffset
            ) : Actor(actor),
                Material(material),
                Shape(shape),
                ActorType(actorType),
                ShapeDesc(*shapeDesc),
                LinearVelocity(linearVelocity),
                StaticFriction(staticFriction),
                DynamicFriction(dynamicFriction),
                Restitution(restitution),
                ContactOffset(contactOffset),
                RestOffset(restOffset)
                {
                }

            JSON_CLASS(
                CActor,
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

        struct ENGINE_API sWidget : public cXml
        {
            glm::mat4 Projection = glm::mat4(1);
        };

        struct ENGINE_API CLabel : public sWidget
        {
            sFont* Font = nullptr;
            string Text;
            glm::vec4 Color = { 1, 1, 1, 1 };
            bool FillFrame = true;

            CLabel() = default;

            CLabel(sFont* font, const string& text = "", const glm::vec4& color = { 1, 1, 1, 1 })
            : Font(font), Text(text), Color(color) {}

            XML_CLASS(
                "Label",
                Text,
                Color,
                FillFrame
            )
        };

        struct ENGINE_API CContainer : public sWidget
        {
            
        };

        struct ENGINE_API CButton : public sWidget
        {
            typedef void (*fHovered)();
            typedef void (*fPressed)();

            glm::vec4 Color = { 1, 1, 1, 1 };
            glm::vec4 ColorHover = { 0.5, 0.5, 0.5, 1.0 };
            glm::vec4 ColorPressed = { 0.25, 0.25, 0.25, 1.0 };
            bool EnableTexture = false;
            cAtlas::sLocation AtlasLocation;
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

        struct ENGINE_API CField : public sWidget
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