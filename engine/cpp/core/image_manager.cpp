#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize.h>

#include <core/image_manager.hpp>
#include <core/memory_pool.hpp>

xpe::core::cImageManager::cImageManager()
{
}

xpe::core::cImageManager::~cImageManager()
{
}

xpe::core::xImage xpe::core::cImageManager::Load(const char* filePath, const xImage::eFormat& format, const xImage::eFileFormat& fileFormat)
{
    xImage image = {};
    image.Format = format;
    image.FileFormat = fileFormat;
    image.OnMemoryPool = K_FALSE;

    int width = 0;
    int height = 0;
    int channels = 0;
    int desiredChannels = 0;
    int channelByteSize = 1;

    if (format == xImage::eFormat::RGB8 || format == xImage::eFormat::RGB16 || format == xImage::eFormat::RGB32)
    {
        desiredChannels = 3;
    }
    else
    {
        desiredChannels = 4;
    }

    image.PixelData = stbi_load(filePath, &width, &height, &channels, desiredChannels);
    image.Width = width;
    image.Height = height;
    image.Depth = 1;
    image.ChannelCount = channels;

    return image;
}

void xpe::core::cImageManager::Write(const char* filePath, const xImage& image)
{
    if (image.FileFormat == xImage::eFileFormat::PNG)
    {
        stbi_write_png(filePath, image.Width, image.Height, image.ChannelCount, image.PixelData, 0);
    }
    else if (image.FileFormat == xImage::eFileFormat::JPEG)
    {
        stbi_write_jpg(filePath, image.Width, image.Height, image.ChannelCount, image.PixelData, 3);
    }
}

void xpe::core::cImageManager::Free(xImage& image)
{
    if (image.PixelData != nullptr)
    {
        stbi_image_free(image.PixelData);
    }
}

xpe::core::xImage xpe::core::cImageManager::Resize(xImage& input, usize outputWidth, usize outputHeight)
{
    xImage output = {};
    output.Width = outputWidth;
    output.Height = outputHeight;
    output.Depth = 1;
    output.ChannelCount = input.ChannelCount;
    output.OnMemoryPool = K_TRUE;
    output.PixelData = nullptr;

    if (input.Format == xImage::eFormat::RGB8 || input.Format == xImage::eFormat::RGBA8)
    {
        output.PixelData = MemoryPoolManager::Allocate(outputWidth * outputHeight * input.ChannelCount);
        stbir_resize_uint8((const unsigned char*)input.PixelData, input.Width, input.Height, 0, (unsigned char*)output.PixelData, output.Width, output.Height, 0, input.ChannelCount);
    }

    return output;
}