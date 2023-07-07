#pragma once

#include <rendering/texture.h>

namespace xpe {

    namespace render {

        using namespace xpe::core;

        class Context;

        struct ENGINE_API MaterialBufferData final {
            // base color
            glm::vec4 BaseColor = { 1, 1, 0, 1 };
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

            float padding_0 = 0;
            float padding_1 = 0;
        };

        struct ENGINE_API Material final {
            TextureSampler Sampler;
            Texture* Albedo = nullptr;
            Texture* Bumping = nullptr;
            Texture* Parallax = nullptr;
            Texture* Metallic = nullptr;
            Texture* Roughness = nullptr;
            Texture* AO = nullptr;
            Texture* Emission = nullptr;
            MaterialBufferData Data;
        };

        struct ENGINE_API cMaterialComponent : public cComponent {

            cMaterialComponent(const string& usid) : cComponent(usid) {}
            cMaterialComponent(const string& usid, Material* material) : cComponent(usid), Material(material) {}

            Material* Material = nullptr;

        };

        class ENGINE_API MaterialBuffer : public Buffer {

        public:
            void Init(Context* context);
            void Free();

            void Flush();

            void SetMaterial(const Material& material);

        private:
            Context* m_Context = nullptr;
            MaterialBufferData m_Data;
        };

        class ENGINE_API MaterialBuilder final {

        public:
            MaterialBuilder() = default;

            MaterialBuilder(Context* context, Material* material)
            : m_Context(context), m_Material(material) {}

            ~MaterialBuilder() = default;

        public:
            Material* Build(const string& name);

            MaterialBuilder& AddAlbedoFromFile(const char* filepath);
            MaterialBuilder& AddBumpFromFile(const char* filepath);
            MaterialBuilder& AddParallaxFromFile(const char* filepath);
            MaterialBuilder& AddMetallicFromFile(const char* filepath);
            MaterialBuilder& AddRoughnessFromFile(const char* filepath);
            MaterialBuilder& AddAOFromFile(const char* filepath);
            MaterialBuilder& AddEmissionFromFile(const char* filepath);

        private:
            Context* m_Context = nullptr;
            Material* m_Material = nullptr;
        };

        class ENGINE_API MaterialManager final {

        public:
            static void Init(Context* context);
            static void Free();

            static MaterialBuilder& Builder();

            static void InitMaterial(Material& material);
            static void FreeMaterial(Material& material);
            static void BindMaterial(Material& material, const eShaderType& shaderType, u32 slot);
            static void UpdateMaterial(Material& material);

            static MaterialBuffer* GetBuffer();

            static void AddMaterial(const string& name, const Material& material);
            static void RemoveMaterial(const string& name);
            static Material* GetMaterial(const string& name);

        private:
            static Context* s_Context;
            static unordered_map<string, Material> s_MaterialTable;
            static MaterialBuilder s_MaterialBuilder;
            static Material s_TempMaterial;
            static MaterialBuffer s_MaterialBuffer;
        };

    }

}