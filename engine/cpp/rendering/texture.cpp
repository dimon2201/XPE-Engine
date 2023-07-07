#include <rendering/texture.h>
#include <rendering/context.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize.h>

namespace xpe {

    namespace render {

        Context* TextureManager::s_Context = nullptr;
        unordered_map<string, Texture> TextureManager::s_TextureTable;

        static const unordered_map<Texture::eFormat, int> s_ChannelTable = {

                { Texture::eFormat::R8, 1 },
                { Texture::eFormat::R16, 1 },
                { Texture::eFormat::R32, 1 },

                { Texture::eFormat::RG8, 2 },
                { Texture::eFormat::RG16, 2 },
                { Texture::eFormat::RG32, 2 },

                { Texture::eFormat::RGB8, 3 },
                { Texture::eFormat::RGB16, 3 },
                { Texture::eFormat::RGB32, 3 },

                { Texture::eFormat::RGBA8, 4 },
                { Texture::eFormat::RGBA16, 4 },
                { Texture::eFormat::RGBA32, 4 },

        };

        void TextureManager::Init(Context* context) {
            s_Context = context;
        }

        void TextureManager::Free() {
            for (auto& texture : s_TextureTable) {
                FreeTexture(texture.second);
            }
            s_TextureTable.clear();
        }

        Texture* TextureManager::LoadTexture(const char* filePath, const Texture::eFormat& format)
        {
            if (s_TextureTable.find(filePath) != s_TextureTable.end()) {
                return &s_TextureTable[filePath];
            }

            Texture texture = {};
            texture.Format = format;
            texture.OnMemoryPool = K_FALSE;

            int width = 0;
            int height = 0;
            int channels = 0;
            int desiredChannels = s_ChannelTable.at(format);

            switch (format) {

                case Texture::eFormat::R8:
                    texture.Pixels = stbi_load(filePath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::R16:
                    texture.Pixels = stbi_load_16(filePath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::R32:
                    texture.Pixels = stbi_loadf(filePath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RG8:
                    texture.Pixels = stbi_load(filePath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RG16:
                    texture.Pixels = stbi_load_16(filePath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RG32:
                    texture.Pixels = stbi_loadf(filePath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RGB8:
                    texture.Pixels = stbi_load(filePath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RGB16:
                    texture.Pixels = stbi_load_16(filePath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RGB32:
                    texture.Pixels = stbi_loadf(filePath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RGBA8:
                    texture.Pixels = stbi_load(filePath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RGBA16:
                    texture.Pixels = stbi_load_16(filePath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RGBA32:
                    texture.Pixels = stbi_loadf(filePath, &width, &height, &channels, desiredChannels);
                    break;

            }

            texture.Width = width;
            texture.Height = height;
            texture.Depth = 1;
            texture.ChannelCount = channels;

            s_Context->CreateTexture(texture, nullptr);

            s_TextureTable[filePath] = texture;

            return &s_TextureTable[filePath];
        }

        void TextureManager::WriteTexture(const char* filePath, const Texture& texture, const Texture::eFileFormat& fileFormat)
        {
            switch (fileFormat) {

                case Texture::eFileFormat::PNG:
                    stbi_write_png(filePath, texture.Width, texture.Height, texture.ChannelCount, texture.Pixels, 0);
                    break;

                case Texture::eFileFormat::JPG:
                    stbi_write_jpg(filePath, texture.Width, texture.Height, texture.ChannelCount, texture.Pixels, 3);
                    break;

                case Texture::eFileFormat::TGA:
                    stbi_write_tga(filePath, texture.Width, texture.Height, texture.ChannelCount, texture.Pixels);
                    break;

                case Texture::eFileFormat::HDR:
                    stbi_write_hdr(filePath, texture.Width, texture.Height, texture.ChannelCount, (float*) texture.Pixels);
                    break;

                case Texture::eFileFormat::BMP:
                    stbi_write_bmp(filePath, texture.Width, texture.Height, texture.ChannelCount, texture.Pixels);
                    break;

            }
        }

        void TextureManager::FreeTexture(Texture &texture)
        {
            if (texture.Pixels != nullptr) {
                stbi_image_free(texture.Pixels);
            }
            s_Context->FreeTexture(&texture);
        }

        Texture TextureManager::ResizeTexture(Texture& input, usize outputWidth, usize outputHeight)
        {
            Texture output = {};
            output.Width = outputWidth;
            output.Height = outputHeight;
            output.Depth = 1;
            output.ChannelCount = input.ChannelCount;
            output.OnMemoryPool = K_TRUE;
            output.Pixels = MemoryPoolManager::Allocate(outputWidth * outputHeight * input.ChannelCount);

            switch (input.Format) {

                case Texture::eFormat::R8:
                    stbir_resize_uint8(
                            (const unsigned char*)input.Pixels,
                            input.Width, input.Height, 0,
                            (unsigned char*)output.Pixels,
                            output.Width, output.Height, 0,
                            input.ChannelCount
                    );
                    break;

                case Texture::eFormat::R32:
                    stbir_resize_float(
                            (const float*)input.Pixels,
                            input.Width, input.Height, 0,
                            (float*)output.Pixels,
                            output.Width, output.Height, 0,
                            input.ChannelCount
                    );
                    break;

                case Texture::eFormat::RG8:
                    stbir_resize_uint8(
                            (const unsigned char*)input.Pixels,
                            input.Width, input.Height, 0,
                            (unsigned char*)output.Pixels,
                            output.Width, output.Height, 0,
                            input.ChannelCount
                    );
                    break;

                case Texture::eFormat::RG32:
                    stbir_resize_float(
                            (const float*)input.Pixels,
                            input.Width, input.Height, 0,
                            (float*)output.Pixels,
                            output.Width, output.Height, 0,
                            input.ChannelCount
                    );
                    break;

                case Texture::eFormat::RGB8:
                    stbir_resize_uint8(
                            (const unsigned char*)input.Pixels,
                            input.Width, input.Height, 0,
                            (unsigned char*)output.Pixels,
                            output.Width, output.Height, 0,
                            input.ChannelCount
                    );
                    break;

                case Texture::eFormat::RGB32:
                    stbir_resize_float(
                            (const float*)input.Pixels,
                            input.Width, input.Height, 0,
                            (float*)output.Pixels,
                            output.Width, output.Height, 0,
                            input.ChannelCount
                    );
                    break;

                case Texture::eFormat::RGBA8:
                    stbir_resize_uint8(
                            (const unsigned char*)input.Pixels,
                            input.Width, input.Height, 0,
                            (unsigned char*)output.Pixels,
                            output.Width, output.Height, 0,
                            input.ChannelCount
                    );
                    break;

                case Texture::eFormat::RGBA32:
                    stbir_resize_float(
                            (const float*)input.Pixels,
                            input.Width, input.Height, 0,
                            (float*)output.Pixels,
                            output.Width, output.Height, 0,
                            input.ChannelCount
                    );
                    break;

            }

            return output;
        }

    }

}