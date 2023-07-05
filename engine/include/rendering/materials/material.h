#pragma once

#include <core/image_manager.hpp>

#include <rendering/texture.h>

namespace xpe {

    namespace render {

        using namespace xpe::core;

        class Context;

        struct ENGINE_API Material final {
            // base color
            glm::vec4 BaseColor = { 1, 1, 1, 1 };
            TextureSampler AlbedoSampler;
            bool EnableAlbedo = false;
            // bumping
            TextureSampler BumpSampler;
            bool EnableBumping = false;
            // parallax
            TextureSampler ParallaxSampler;
            bool EnableParallax = false;
            float heightScale = 0.1;
            float parallaxMinLayers = 8;
            float parallaxMaxLayers = 32;
            // metallic
            float Metallic = 0.5f;
            TextureSampler MetallicSampler;
            bool EnableMetallic = false;
            // roughness
            float Roughness = 0.5f;
            TextureSampler RoughnessSampler;
            bool EnableRoughness = false;
            // ambient occlusion
            float AmbientOcclussion = 0.5f;
            TextureSampler AmbientOcclusionSampler;
            bool EnableAmbientOcclusion = false;
            // emission
            glm::vec3 Emission = { 0, 0, 0 };
            TextureSampler EmissionSampler;
            bool EnableEmission = false;
        };

        struct ENGINE_API cMaterialComponent : public cComponent {

            cMaterialComponent(const string& usid) : cComponent(usid) {}

            Material* Material = nullptr;

        };

        struct ENGINE_API MaterialBufferData final {
            Material Value;
        };

        class ENGINE_API MaterialBuffer : public Buffer {

        public:
            void Init(Context* context);
            void Free();

            void Flush();

            void SetMaterial(const cMaterialComponent* materialComponent);

        private:
            Context* m_Context = nullptr;
            MaterialBufferData m_Data;
        };

    }

}