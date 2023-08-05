#pragma once

#include <rendering/core/texture.h>
#include <rendering/buffers/material_buffer.h>

namespace xpe {

    namespace render {

        using namespace xpe::core;

        class Context;

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

            u32 AlbedoIndex = 0;
            TextureLayer* Albedo = nullptr;

            u32 BumpingIndex = 0;
            TextureLayer* Bumping = nullptr;

            u32 ParallaxIndex = 0;
            TextureLayer* Parallax = nullptr;

            u32 MetallicIndex = 0;
            TextureLayer* Metallic = nullptr;

            u32 RoughnessIndex = 0;
            TextureLayer* Roughness = nullptr;

            u32 AOIndex = 0;
            TextureLayer* AO = nullptr;

            u32 EmissionIndex = 0;
            TextureLayer* Emission = nullptr;

            MaterialBufferData* Data = nullptr;
        };

        struct ENGINE_API MaterialStorage : public Object {
            MaterialBuffer Buffer;
            MaterialTextures Textures;
            unordered_map<string, Material> Table;

            MaterialStorage(usize count);
            ~MaterialStorage();

            void AddMaterial(const string& name, const Material& material);
            void RemoveMaterial(const string& name);
            Material* GetMaterial(const string& name);

        private:
            void InitMaterialTextures();
            void FreeMaterialTextures();
            void InitMaterialSampler();
            void InitTextureArray(Texture& textureArray, const Texture::eFormat& format, usize width, usize height, u32 slot);
        };

        class ENGINE_API MaterialManager final {

        public:
            static const usize K_MATERIALS_COUNT = 1000;

        public:
            static void Init();
            static void Free();

            static Material* CreateMaterial(const string& usid);
            static void CreateMaterial(Material& material);

            static void AddAlbedoFromFile(Material& material, const char* filepath);
            static void AddBumpFromFile(Material& material, const char* filepath);
            static void AddParallaxFromFile(Material& material, const char* filepath);
            static void AddMetallicFromFile(Material& material, const char* filepath);
            static void AddRoughnessFromFile(Material& material, const char* filepath);
            static void AddAOFromFile(Material& material, const char* filepath);
            static void AddEmissionFromFile(Material& material, const char* filepath);

            static u32 AddMaterial(const string& usid, const Material& material);

            static void RemoveMaterial(const string& usid);

            static void FlushMaterial(Material& material);

            static void FlushMaterials();

            static MaterialBuffer* GetBuffer();
            static MaterialStorage* GetStorage();
            static MaterialBufferData* GetMaterialData(u32 index);

        private:
            static TextureLayer* AddTextureFromFile(
                    const char *filepath,
                    Texture& textureArray,
                    u32 textureLayerIndex
            );

        private:
            static MaterialStorage* s_Storage;
        };

    }

}