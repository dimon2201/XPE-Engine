#pragma once

#include <rendering/core/texture.h>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace render;

        struct MeshResource final
        {

        };

        struct TextureResource final
        {
            int Width = 0;
            int Height = 0;
            Texture::eFormat Format = Texture::eFormat::RGB8;
            void* Pixels = nullptr;

            TextureResource() = default;

            TextureResource(const Texture::eFormat format) : Format(format) {}
        };

        struct MaterialResource final
        {
            TextureResource Albedo = Texture::eFormat::RGB8;
            TextureResource Bump = Texture::eFormat::RGB8;
            TextureResource Parallax = Texture::eFormat::R8;
            TextureResource Metallic = Texture::eFormat::R8;
            TextureResource Roughness = Texture::eFormat::R8;
            TextureResource AO = Texture::eFormat::R8;
            TextureResource Emission = Texture::eFormat::RGB8;
        };

        struct ModelResource final
        {
            vector<MeshResource> Meshes;
            unordered_map<u32, MaterialResource> Materials;
        };

        struct ModelLoader final
        {

            enum class eOption
            {
                TRIANGULATE,
                FLIP_UV,
                CALC_TANGENTS,
                OPTIMIZE_MESHES
            };

        public:
            ModelResource Load(const char* filepath, const vector<eOption>& options);

        };

    }

}