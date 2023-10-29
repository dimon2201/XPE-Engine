#pragma once

#include <build.h>

#include <rendering/core/texture.h>

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

        class LOADER_API TextureLoader final
        {
        public:
            static void Init();
            static void Free();
            static Texture* Load(const char* filepath, const eTextureFormat& format);
            static Texture* LoadCube(const TextureCubeFilepath& cubeFilepath, const eTextureFormat& format);
            static TextureLayer LoadLayer(
                    const char* filepath,
                    const eTextureFormat& format,
                    int& width, int& height, int& channels
            );
            static bool Save(const char* filepath, const Texture& texture, const Texture::eFileFormat& fileFormat);
            static bool SaveLayer(const char* filepath, const TextureLayer& textureLayer, const Texture::eFileFormat& fileFormat);
        };

    }

}