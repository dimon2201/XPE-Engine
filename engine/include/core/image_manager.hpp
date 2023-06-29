#pragma once

#include <core/types.hpp>

namespace xpe
{
    namespace core
    {
        struct xImage
        {
            enum class eFormat
            {
                RGB8 = 0,
                RGB16 = 1,
                RGB32 = 2,
                RGBA8 = 3,
                RGBA16 = 4,
                RGBA32 = 5
            };

            usize Width;
            usize Height;
            usize Depth;
            eFormat Format;
            void* PixelData;
        };

        class ENGINE_API cImageManager : public Object
        {
            public:
                cImageManager();
                ~cImageManager();

                xImage Load(const char* filePath, const xImage::eFormat& format);
        };
    }
}