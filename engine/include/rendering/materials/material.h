#pragma once

#include <rendering/texture.h>
#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        using namespace xpe::core;

        class Context;

        struct ENGINE_API MaterialBufferData final {
            // base color
            glm::vec4 BaseColor = { 1, 1, 0, 1 };
            Bool EnableAlbedo = false;
            // bumping
            Bool EnableBumping = false;
            // parallax
            Bool EnableParallax = false;
            float HeightScale = 0.1;
            float ParallaxMinLayers = 8;
            float ParallaxMaxLayers = 32;
            // metallic
            float MetallicFactor = 0.5f;
            Bool EnableMetallic = false;
            // roughness
            float RoughnessFactor = 0.5f;
            Bool EnableRoughness = false;
            // ambient occlusion
            float AOFactor = 0.5f;
            Bool EnableAO = false;
            // emission
            glm::vec3 EmissionColor = { 0, 0, 0 };
            Bool EnableEmission = false;
        };

        struct ENGINE_API MaterialTextures final {
            TextureSampler Sampler;
            Texture AlbedoArray;
            Texture BumpArray;
            Texture ParallaxArray;
            Texture MetallicArray;
            Texture RoughnessArray;
            Texture AOArray;
            Texture EmissionArray;
        };

        struct ENGINE_API Material final {
            u32 Index = 0;
            MaterialTextures* Textures = nullptr;
            TextureLayer* Albedo = nullptr;
            TextureLayer* Bumping = nullptr;
            TextureLayer* Parallax = nullptr;
            TextureLayer* Metallic = nullptr;
            TextureLayer* Roughness = nullptr;
            TextureLayer* AO = nullptr;
            TextureLayer* Emission = nullptr;
            MaterialBufferData* Data = nullptr;
        };

        struct ENGINE_API MaterialComponent : public Component {

            MaterialComponent(const string& usid) : Component(usid) {}
            MaterialComponent(const string& usid, Material* material) : Component(usid), Material(material) {}

            Material* Material = nullptr;

        };

        class ENGINE_API MaterialBuffer : public StructureBuffer<MaterialBufferData> {

        public:
            MaterialBuffer() = default;
            MaterialBuffer(Context* context, usize size) : StructureBuffer<MaterialBufferData>(context, size, K_SLOT_MATERIALS, K_FALSE) {}

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
            TextureLayer* AddTextureFromFile(const char* filepath, Texture& textureArray);

        private:
            Context* m_Context = nullptr;
            Material* m_Material = nullptr;
        };

        class ENGINE_API MaterialManager final {

        public:
            static const usize K_MATERIALS_COUNT = 1000;

        public:
            static void Init(Context* context);
            static void Free();

            static MaterialBuilder& Builder();
            static MaterialTextures& Textures();
            static vector<MaterialBufferData>& List();

            static void InitMaterial(Material& material);
            static void FreeMaterial(Material& material);

            static void BindMaterials();

            static void UpdateMaterials();
            static void UpdateMaterial(Material& material);

            static MaterialBuffer* GetBuffer();

            static void AddMaterial(const string& name, const Material& material);
            static void RemoveMaterial(const string& name);
            static Material* GetMaterial(const string& name);

            static MaterialBufferData* GetMaterialData(u32 index);

        private:
            static void InitMaterialList();
            static void FreeMaterialList();

            static void InitTextureArray(Texture& textureArray, const Texture::eFormat& format, usize width, usize height, u32 slot);

        private:
            static Context* s_Context;
            static unordered_map<string, Material> s_MaterialTable;
            static MaterialBuilder s_MaterialBuilder;
            static Material s_TempMaterial;
            static MaterialBuffer s_MaterialBuffer;
            static MaterialTextures s_MaterialTextures;
        };

    }

}