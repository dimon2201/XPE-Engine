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

        unordered_map<Texture::eFormat, int> TextureManager::ChannelTable = {

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
        unordered_map<Texture::eFormat, int> TextureManager::BPPTable = {
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
            LogInfo("TextureManager::Init()");
            s_Context = context;
            LogInfo("TextureManager initialized");
        }

        void TextureManager::Free() {
            LogInfo("TextureManager::Free()");
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
            texture.Depth = 1;
            TextureLayer layer = ReadTextureLayerFile(filepath, format, texture.Width, texture.Height, texture.ChannelCount);
            texture.Layers.emplace_back(layer);

            s_TextureTable[filepath] = texture;

            return &s_TextureTable[filepath];
        }

        Texture* TextureManager::LoadTextureFile(const char* filePath, const Texture::eFormat& format)
        {
            Texture* texture = ReadTextureFile(filePath, format);
            InitTexture(*texture);
            return texture;
        }

        TextureLayer TextureManager::ReadTextureLayerFile(const char *filepath, const Texture::eFormat &format, usize& width, usize& height, usize& channels) {
            int desiredChannels = ChannelTable.at(format);
            TextureLayer layer;
            int w;
            int h;
            int c;

            switch (format) {

                case Texture::eFormat::R8:
                    layer.Pixels = stbi_load(filepath, &w, &h, &c, desiredChannels);
                    break;

                case Texture::eFormat::R16:
                    layer.Pixels = stbi_load_16(filepath, &w, &h, &c, desiredChannels);
                    break;

                case Texture::eFormat::R32:
                    layer.Pixels = stbi_loadf(filepath, &w, &h, &c, desiredChannels);
                    break;

                case Texture::eFormat::RG8:
                    layer.Pixels = stbi_load(filepath, &w, &h, &c, desiredChannels);
                    break;

                case Texture::eFormat::RG16:
                    layer.Pixels = stbi_load_16(filepath, &w, &h, &c, desiredChannels);
                    break;

                case Texture::eFormat::RG32:
                    layer.Pixels = stbi_loadf(filepath, &w, &h, &c, desiredChannels);
                    break;

                case Texture::eFormat::RGB8:
                    layer.Pixels = stbi_load(filepath, &w, &h, &c, desiredChannels);
                    break;

                case Texture::eFormat::RGB16:
                    layer.Pixels = stbi_load_16(filepath, &w, &h, &c, desiredChannels);
                    break;

                case Texture::eFormat::RGB32:
                    layer.Pixels = stbi_loadf(filepath, &w, &h, &c, desiredChannels);
                    break;

                case Texture::eFormat::RGBA8:
                    layer.Pixels = stbi_load(filepath, &w, &h, &c, desiredChannels);
                    break;

                case Texture::eFormat::RGBA16:
                    layer.Pixels = stbi_load_16(filepath, &w, &h, &c, desiredChannels);
                    break;

                case Texture::eFormat::RGBA32:
                    layer.Pixels = stbi_loadf(filepath, &w, &h, &c, desiredChannels);
                    break;

            }

            width = w;
            height = h;
            channels = c;

            return layer;
        }

        Texture* TextureManager::ReadTextureCubeFile(const TextureCubeFile &cubeFile, const Texture::eFormat &format) {
            if (s_TextureTable.find(cubeFile.Name) != s_TextureTable.end()) {
                return &s_TextureTable[cubeFile.Name];
            }

            Texture textureCube;
            textureCube.Type = Texture::eType::TEXTURE_CUBE;
            textureCube.Format = format;

            TextureLayer front = ReadTextureLayerFile(cubeFile.FrontFilepath, format, textureCube.Width, textureCube.Height, textureCube.ChannelCount);
            TextureLayer back = ReadTextureLayerFile(cubeFile.BackFilepath, format, textureCube.Width, textureCube.Height, textureCube.ChannelCount);
            TextureLayer right = ReadTextureLayerFile(cubeFile.RightFilepath, format, textureCube.Width, textureCube.Height, textureCube.ChannelCount);
            TextureLayer left = ReadTextureLayerFile(cubeFile.LeftFilepath, format, textureCube.Width, textureCube.Height, textureCube.ChannelCount);
            TextureLayer top = ReadTextureLayerFile(cubeFile.TopFilepath, format, textureCube.Width, textureCube.Height, textureCube.ChannelCount);
            TextureLayer bottom = ReadTextureLayerFile(cubeFile.BottomFilepath, format, textureCube.Width, textureCube.Height, textureCube.ChannelCount);

            textureCube.Layers = {
                front,
                back,
                right,
                left,
                top,
                bottom,
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
            void* pixels = texture.Layers.front().Pixels;

            switch (fileFormat) {

                case Texture::eFileFormat::PNG:
                    stbi_write_png(filePath, texture.Width, texture.Height, texture.ChannelCount, pixels, 0);
                    break;

                case Texture::eFileFormat::JPG:
                    stbi_write_jpg(filePath, texture.Width, texture.Height, texture.ChannelCount, pixels, 3);
                    break;

                case Texture::eFileFormat::TGA:
                    stbi_write_tga(filePath, texture.Width, texture.Height, texture.ChannelCount, pixels);
                    break;

                case Texture::eFileFormat::HDR:
                    stbi_write_hdr(filePath, texture.Width, texture.Height, texture.ChannelCount, (float*) pixels);
                    break;

                case Texture::eFileFormat::BMP:
                    stbi_write_bmp(filePath, texture.Width, texture.Height, texture.ChannelCount, pixels);
                    break;

            }
        }

        void TextureManager::InitTexture(Texture &texture) {
            s_Context->CreateTexture(texture);
        }

        void TextureManager::InitTextureCube(Texture &texture) {
            s_Context->CreateTextureCube(texture);
        }

        void TextureManager::BindTexture(Texture &texture) {
            s_Context->BindTexture(&texture);
        }

        void TextureManager::FreeTexture(Texture &texture)
        {
            for (auto& layer : texture.Layers) {
                if (layer.Pixels != nullptr) {
                    stbi_image_free(layer.Pixels);
                }
            }
            texture.Layers.clear();
            s_Context->FreeTexture(&texture);
        }

        void TextureManager::WriteTexture(Texture &texture) {
            usize layerSize = texture.Layers.size();
            for (u32 layerIndex = 0 ; layerIndex < layerSize ; layerIndex++) {
                WriteTexture(texture, layerIndex);
            }
        }

        void TextureManager::WriteTexture(Texture &texture, u32 layerIndex) {
            s_Context->WriteTexture(
                    texture,
                    texture.Layers[layerIndex].Pixels,
                    texture.Width * texture.Height * texture.ChannelCount,
                    layerIndex
            );
        }

        Texture TextureManager::ResizeTexture(const Texture& input, usize outputWidth, usize outputHeight)
        {
            Texture output = {};
            output.Width = outputWidth;
            output.Height = outputHeight;
            output.Depth = 1;
            output.ChannelCount = input.ChannelCount;
            output.OnMemoryPool = K_TRUE;
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

        void TextureManager::ResizeTextureU8(const Texture &input, Texture &output) {
            usize layerSize = input.Layers.size();
            for (usize i = 0 ; i < layerSize ; i++) {
                stbir_resize_uint8(
                        (const unsigned char*)input.Layers[i].Pixels,
                        input.Width, input.Height, 0,
                        (unsigned char*)output.Layers[i].Pixels,
                        output.Width, output.Height, 0,
                        input.ChannelCount
                );
            }
        }

        void TextureManager::ResizeTextureFloat(const Texture &input, Texture &output) {
            usize layerSize = input.Layers.size();
            for (usize i = 0 ; i < layerSize ; i++) {
                stbir_resize_float(
                        (const float*)input.Layers[i].Pixels,
                        input.Width, input.Height, 0,
                        (float*)output.Layers[i].Pixels,
                        output.Width, output.Height, 0,
                        input.ChannelCount
                );
            }
        }

        void TextureManager::FlipTexture(Texture &texture) {
            int bpp = BPPTable.at(texture.Format);
            for (auto& layer : texture.Layers) {
                stbi__vertical_flip(layer.Pixels, texture.Width, texture.Height, bpp);
            }
        }

    }

}