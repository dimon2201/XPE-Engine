#pragma once

#include <build.hpp>
#include <rendering/core/core.hpp>

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
            static cTexture* Load(const char* filepath, const eTextureFormat& format);
            static cTexture* LoadCube(const sTextureCubeFilepath& cubeFilepath, const eTextureFormat& format);
            static sTextureLayer LoadLayer(
                const char* filepath,
                const eTextureFormat& format,
                int& width, int& height, int& channels
            );
            static bool SaveLayer(const char* filepath, sTextureLayer* textureLayer, const eFileFormat& fileFormat);
        };

    }

}