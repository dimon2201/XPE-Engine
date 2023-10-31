#pragma once

#include <build.h>

#include <rendering/core/texture.h>

namespace xpe {

    namespace res {

        using namespace core;
        using namespace render;
        using namespace math;

        struct LOADER_API sTextureCubeFilepath final
        {
            string Name;
            string FrontFilepath;
            string BackFilepath;
            string RightFilepath;
            string LeftFilepath;
            string TopFilepath;
            string BottomFilepath;
        };

        class LOADER_API cTextureLoader final
        {
        public:
            static void Init();
            static void Free();
            static sTexture* Load(const char* filepath, const eTextureFormat& format);
            static sTexture* LoadCube(const sTextureCubeFilepath& cubeFilepath, const eTextureFormat& format);
            static sTextureLayer LoadLayer(
                    const char* filepath,
                    const eTextureFormat& format,
                    int& width, int& height, int& channels
            );
            static bool Save(const char* filepath, const sTexture& texture, const sTexture::eFileFormat& fileFormat);
            static bool SaveLayer(const char* filepath, const sTextureLayer& textureLayer, const sTexture::eFileFormat& fileFormat);
        };

    }

}