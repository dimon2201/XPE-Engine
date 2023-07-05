#pragma once

#include <core/image_manager.hpp>

#include <rendering/texture.h>

namespace xpe {

    namespace render {

        using namespace xpe::core;

        class Context;

        struct ENGINE_API MaterialBufferData final {
            // base color
            glm::vec4 BaseColor = { 1, 1, 1, 1 };
            bool EnableAlbedo = false;
            // bumping
            bool EnableBumping = false;
            // parallax
            bool EnableParallax = false;
            float HeightScale = 0.1;
            float ParallaxMinLayers = 8;
            float ParallaxMaxLayers = 32;
            // metallic
            float MetallicFactor = 0.5f;
            bool EnableMetallic = false;
            // roughness
            float RoughnessFactor = 0.5f;
            bool EnableRoughness = false;
            // ambient occlusion
            float AOFactor = 0.5f;
            bool EnableAO = false;
            // emission
            glm::vec3 EmissionColor = { 0, 0, 0 };
            bool EnableEmission = false;
        };

        struct ENGINE_API Material final {
            TextureSampler Sampler;
            Texture Albedo;
            Texture Bumping;
            Texture Parallax;
            Texture Metallic;
            Texture Roughness;
            Texture AO;
            Texture Emission;
            MaterialBufferData Data;
        };

        struct ENGINE_API cMaterialComponent : public cComponent {

            cMaterialComponent(const string& usid) : cComponent(usid) {}

            Material* Material = nullptr;

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

        class ENGINE_API MaterialManager final {

        };

    }

}