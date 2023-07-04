#pragma once

#include <core/types.hpp>

namespace xpe
{
    namespace core
    {
        struct xImage
        {
            enum class eFileFormat
            {
                PNG = 0,
                JPEG = 1
            };

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
            usize ChannelCount;
            eFormat Format;
            eFileFormat FileFormat;
            boolean OnMemoryPool;
            void* PixelData;
        };

        class ENGINE_API cImageManager : public Object
        {
            public:
                cImageManager();
                ~cImageManager();

                static xImage Load(const char* filePath, const xImage::eFormat& format, const xImage::eFileFormat& fileFormat);
                static void Write(const char* filePath, const xImage& image);
                static void Free(xImage& image);
                static xImage Resize(xImage& input, usize outputWidth, usize outputHeight);
        };
    }
}