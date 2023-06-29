#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <core/image_manager.hpp>
#include <core/memory_pool.hpp>

xpe::core::cImageManager::cImageManager()
{
}

xpe::core::cImageManager::~cImageManager()
{
}

xpe::core::xImage xpe::core::cImageManager::Load(const char* filePath, const xImage::eFormat& format)
{
    xImage image = {};

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

    void* pixelData = stbi_load(filePath, &width, &height, &channels, desiredChannels);

    usize pixelDataByteSize = image.Width * image.Height * desiredChannels * channelByteSize;
    image.PixelData = MemoryPoolManager::Allocate(pixelDataByteSize);
    memcpy(image.PixelData, pixelData, pixelDataByteSize);

    stbi_image_free(pixelData);

    return image;
}