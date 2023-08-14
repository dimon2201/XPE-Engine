#pragma once

#include <build.h>

#include <rendering/storages/texture_storage.h>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace render;
        using namespace math;

        struct LOADER_API TextureCubeFilepath final
        {
            string Name;
            string FrontFilepath;
            string BackFilepath;
            string RightFilepath;
            string LeftFilepath;
            string TopFilepath;
            string BottomFilepath;
        };

        class LOADER_API TextureLoader : public Object
        {

        public:

            enum class eOption
            {
                TRIANGULATE,
                FLIP_UV,
                CALC_TANGENTS,
                OPTIMIZE_MESHES
            };

            TextureLoader(TextureStorage* textureStorage) : m_Storage(textureStorage) {}

            Ref<Texture> Load(const char* filepath, const eTextureFormat& format);

            Ref<Texture> LoadCube(const TextureCubeFilepath& cubeFilepath, const eTextureFormat& format);

            static TextureLayer LoadLayer(
                    const char* filepath,
                    const eTextureFormat& format,
                    int& width, int& height, int& channels
            );

            void Save(const char* filepath, const Texture& texture, const Texture::eFileFormat& fileFormat);

        private:
            TextureStorage* m_Storage = nullptr;
        };

    }

}