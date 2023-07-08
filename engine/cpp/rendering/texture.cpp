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

        // channels count table for each texture format
        static const unordered_map<Texture::eFormat, int> s_TextureChannelTable = {

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

        // bytes per pixel table for each texture format
        static const unordered_map<Texture::eFormat, int> s_TextureBPPTable = {
                { Texture::eFormat::R8, 1 },
                { Texture::eFormat::R16, 2 },
                { Texture::eFormat::R32, 4 },

                { Texture::eFormat::RG8, 2 },
                { Texture::eFormat::RG16, 4 },
                { Texture::eFormat::RG32, 8 },

                { Texture::eFormat::RGB8, 3 },
                { Texture::eFormat::RGB16, 6 },
                { Texture::eFormat::RGB32, 12 },

                { Texture::eFormat::RGBA8, 4 },
                { Texture::eFormat::RGBA16, 8 },
                { Texture::eFormat::RGBA32, 16 },
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

        Texture* TextureManager::ReadTextureFile(const char *filepath, const Texture::eFormat &format) {
            if (s_TextureTable.find(filepath) != s_TextureTable.end()) {
                return &s_TextureTable[filepath];
            }

            Texture texture = {};
            texture.Format = format;
            texture.OnMemoryPool = K_FALSE;

            int width = 0;
            int height = 0;
            int channels = 0;
            int desiredChannels = s_TextureChannelTable.at(format);

            switch (format) {

                case Texture::eFormat::R8:
                    texture.Pixels = stbi_load(filepath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::R16:
                    texture.Pixels = stbi_load_16(filepath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::R32:
                    texture.Pixels = stbi_loadf(filepath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RG8:
                    texture.Pixels = stbi_load(filepath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RG16:
                    texture.Pixels = stbi_load_16(filepath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RG32:
                    texture.Pixels = stbi_loadf(filepath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RGB8:
                    texture.Pixels = stbi_load(filepath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RGB16:
                    texture.Pixels = stbi_load_16(filepath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RGB32:
                    texture.Pixels = stbi_loadf(filepath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RGBA8:
                    texture.Pixels = stbi_load(filepath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RGBA16:
                    texture.Pixels = stbi_load_16(filepath, &width, &height, &channels, desiredChannels);
                    break;

                case Texture::eFormat::RGBA32:
                    texture.Pixels = stbi_loadf(filepath, &width, &height, &channels, desiredChannels);
                    break;

            }

            texture.Width = width;
            texture.Height = height;
            texture.Depth = 1;
            texture.ChannelCount = channels;
            texture.Layers = { texture.Layers };

            s_TextureTable[filepath] = texture;

            return &s_TextureTable[filepath];
        }

        Texture* TextureManager::LoadTextureFile(const char* filePath, const Texture::eFormat& format)
        {
            Texture* texture = ReadTextureFile(filePath, format);
            InitTexture(*texture);
            return texture;
        }

        Texture* TextureManager::ReadTextureCubeFile(const TextureCubeFile &cubeFile, const Texture::eFormat &format) {
            if (s_TextureTable.find(cubeFile.Name) != s_TextureTable.end()) {
                return &s_TextureTable[cubeFile.Name];
            }

            Texture* front = ReadTextureFile(cubeFile.FrontFilepath, format);
            Texture* back = ReadTextureFile(cubeFile.BackFilepath, format);
            Texture* right = ReadTextureFile(cubeFile.RightFilepath, format);
            Texture* left = ReadTextureFile(cubeFile.LeftFilepath, format);
            Texture* top = ReadTextureFile(cubeFile.TopFilepath, format);
            Texture* bottom = ReadTextureFile(cubeFile.BottomFilepath, format);

            Texture textureCube;
            textureCube = *front;
            textureCube.Type = Texture::eType::TEXTURE_CUBE;
            textureCube.Pixels = front->Pixels;
            textureCube.Layers = {
                front->Pixels,
                back->Pixels,
                right->Pixels,
                left->Pixels,
                top->Pixels,
                bottom->Pixels,
            };

            s_TextureTable[cubeFile.Name] = textureCube;

            return &s_TextureTable[cubeFile.Name];
        }

        Texture* TextureManager::LoadTextureCubeFile(const TextureCubeFile &cubeFile, const Texture::eFormat &format) {
            Texture* texture = ReadTextureCubeFile(cubeFile, format);
            InitTextureCube(*texture);
            return texture;
        }

        void TextureManager::WriteTextureFile(const char* filePath, const Texture& texture, const Texture::eFileFormat& fileFormat)
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

        void TextureManager::InitTexture(Texture &texture) {
            s_Context->CreateTexture(texture, nullptr);
        }

        void TextureManager::InitTextureCube(Texture &texture) {
            s_Context->CreateTextureCube(texture, nullptr);
        }

        void TextureManager::BindTexture(Texture &texture) {
            s_Context->BindTexture(&texture);
        }

        void TextureManager::FreeTexture(Texture &texture)
        {
            for (void* layer : texture.Layers) {
                stbi_image_free(layer);
            }
            texture.Layers.clear();
            s_Context->FreeTexture(&texture);
        }

        void TextureManager::WriteTexture(Texture &texture) {
            s_Context->WriteTexture(texture, texture.Pixels, texture.Width * texture.Height * texture.ChannelCount);
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
            output.Layers = input.Layers;

            switch (input.Format) {

                case Texture::eFormat::R8:
                    ResizeTextureU8(input, output);
                    break;

                case Texture::eFormat::R32:
                    ResizeTextureFloat(input, output);
                    break;

                case Texture::eFormat::RG8:
                    ResizeTextureU8(input, output);
                    break;

                case Texture::eFormat::RG32:
                    ResizeTextureFloat(input, output);
                    break;

                case Texture::eFormat::RGB8:
                    ResizeTextureU8(input, output);
                    break;

                case Texture::eFormat::RGB32:
                    ResizeTextureFloat(input, output);
                    break;

                case Texture::eFormat::RGBA8:
                    ResizeTextureU8(input, output);
                    break;

                case Texture::eFormat::RGBA32:
                    ResizeTextureFloat(input, output);
                    break;

            }

            return output;
        }

        void TextureManager::ResizeTextureU8(Texture &input, Texture &output) {
            usize layerSize = input.Layers.size();
            for (usize i = 0 ; i < layerSize ; i++) {
                stbir_resize_uint8(
                        (const unsigned char*)input.Layers[i],
                        input.Width, input.Height, 0,
                        (unsigned char*)output.Layers[i],
                        output.Width, output.Height, 0,
                        input.ChannelCount
                );
            }
        }

        void TextureManager::ResizeTextureFloat(Texture &input, Texture &output) {
            usize layerSize = input.Layers.size();
            for (usize i = 0 ; i < layerSize ; i++) {
                stbir_resize_float(
                        (const float*)input.Layers[i],
                        input.Width, input.Height, 0,
                        (float*)output.Layers[i],
                        output.Width, output.Height, 0,
                        input.ChannelCount
                );
            }
        }

        void TextureManager::FlipTexture(Texture &texture) {
            int bpp = s_TextureBPPTable.at(texture.Format);
            for (void* layer : texture.Layers) {
                stbi__vertical_flip(layer, texture.Width, texture.Height, bpp);
            }
        }

    }

}