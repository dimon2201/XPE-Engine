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

        std::unordered_map<Texture::eFormat, int> TextureManager::ChannelTable = {

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
        std::unordered_map<Texture::eFormat, int> TextureManager::BPPTable = {
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

        Context* TextureManager::s_Context = nullptr;
        TextureStorage* TextureManager::s_Storage = nullptr;

        TextureStorage::~TextureStorage() {
            for (auto& texture : Table) {
                TextureManager::FreeTexture(texture.second);
            }
            Table.clear();
        }

        void TextureManager::Init(Context* context) {
            LogInfo("TextureManager::Init()");
            s_Context = context;
            s_Storage = new TextureStorage();
            LogInfo("TextureManager initialized");
        }

        void TextureManager::Free() {
            LogInfo("TextureManager::Free()");
            delete s_Storage;
        }

        Texture* TextureManager::ReadTextureFile(const char *filepath, const Texture::eFormat &format) {
            auto& table = s_Storage->Table;

            if (table.find(filepath) != table.end()) {
                return &table[filepath];
            }

            Texture texture = {};
            texture.Format = format;
            texture.Depth = 1;
            TextureLayer layer = ReadTextureLayerFile(filepath, format, texture.Width, texture.Height, texture.Channels);
            texture.Layers.emplace_back(layer);

            table[filepath] = texture;

            return &table[filepath];
        }

        Texture* TextureManager::LoadTextureFile(const char* filePath, const Texture::eFormat& format)
        {
            Texture* texture = ReadTextureFile(filePath, format);
            InitTexture(*texture);
            return texture;
        }

        TextureLayer TextureManager::ReadTextureLayerFile(
                const char *filepath,
                const Texture::eFormat &format, int& width, int& height, int& channels
        ) {
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

            layer.RowByteSize = width * BPPTable.at(format);

            return layer;
        }

        Texture* TextureManager::ReadTextureCubeFile(const TextureCubeFile &cubeFile, const Texture::eFormat &format) {
            auto& table = s_Storage->Table;

            if (table.find(cubeFile.Name) != table.end()) {
                return &table[cubeFile.Name];
            }

            Texture textureCube;
            textureCube.Type = Texture::eType::TEXTURE_CUBE;
            textureCube.Format = format;

            TextureLayer front = ReadTextureLayerFile(cubeFile.FrontFilepath, format, textureCube.Width, textureCube.Height, textureCube.Channels);
            TextureLayer back = ReadTextureLayerFile(cubeFile.BackFilepath, format, textureCube.Width, textureCube.Height, textureCube.Channels);
            TextureLayer right = ReadTextureLayerFile(cubeFile.RightFilepath, format, textureCube.Width, textureCube.Height, textureCube.Channels);
            TextureLayer left = ReadTextureLayerFile(cubeFile.LeftFilepath, format, textureCube.Width, textureCube.Height, textureCube.Channels);
            TextureLayer top = ReadTextureLayerFile(cubeFile.TopFilepath, format, textureCube.Width, textureCube.Height, textureCube.Channels);
            TextureLayer bottom = ReadTextureLayerFile(cubeFile.BottomFilepath, format, textureCube.Width, textureCube.Height, textureCube.Channels);

            textureCube.Layers = {
                front,
                back,
                right,
                left,
                top,
                bottom,
            };

            table[cubeFile.Name] = textureCube;

            return &table[cubeFile.Name];
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
                    stbi_write_png(filePath, texture.Width, texture.Height, texture.Channels, pixels, 0);
                    break;

                case Texture::eFileFormat::JPG:
                    stbi_write_jpg(filePath, texture.Width, texture.Height, texture.Channels, pixels, 3);
                    break;

                case Texture::eFileFormat::TGA:
                    stbi_write_tga(filePath, texture.Width, texture.Height, texture.Channels, pixels);
                    break;

                case Texture::eFileFormat::HDR:
                    stbi_write_hdr(filePath, texture.Width, texture.Height, texture.Channels, (float*) pixels);
                    break;

                case Texture::eFileFormat::BMP:
                    stbi_write_bmp(filePath, texture.Width, texture.Height, texture.Channels, pixels);
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
                    dealloc(layer.Pixels);
                }
                FreeMips(layer);
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
                    texture.Width * texture.Height * BPPTable.at(texture.Format),
                    layerIndex
            );
        }

        Texture TextureManager::ResizeTexture(const Texture& input, int outputWidth, int outputHeight)
        {
            Texture output = input;
            output.Width = outputWidth;
            output.Height = outputHeight;

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
                ResizeTextureLayerU8(
                        input.Layers[i], input.Width, input.Height, input.Channels,
                        output.Layers[i], output.Width, output.Height
                );
            }
        }

        void TextureManager::ResizeTextureFloat(const Texture &input, Texture &output) {
            usize layerSize = input.Layers.size();
            for (usize i = 0 ; i < layerSize ; i++) {
                ResizeTextureLayerFloat(
                    input.Layers[i], input.Width, input.Height, input.Channels,
                    output.Layers[i], output.Width, output.Height
                );
            }
        }

        void TextureManager::ResizeTextureLayerU8(
                const TextureLayer &input, int inputWidth, int inputHeight, int channels,
                TextureLayer &output, int outputWidth, int outputHeight
        ) {
            output.Pixels = ResizePixelsU8(
                    input.Pixels,
                    inputWidth, inputHeight, channels,
                    outputWidth, outputHeight
            );

            if (!input.Mips.empty()) {
                FreeMips(output);
                int bpp = sizeof(u8) * channels;
                GenerateMipsU8(output, outputWidth, outputHeight, bpp, channels);
            }
        }

        void TextureManager::ResizeTextureLayerFloat(
                const TextureLayer &input, int inputWidth, int inputHeight, int channels,
                TextureLayer &output, int outputWidth, int outputHeight
        ) {
            output.Pixels = ResizePixelsFloat(
                    input.Pixels,
                    inputWidth, inputHeight, channels,
                    outputWidth, outputHeight
            );

            if (!input.Mips.empty()) {
                FreeMips(output);
                int bpp = sizeof(float) * channels;
                GenerateMipsFloat(output, outputWidth, outputHeight, bpp, channels);
            }
        }

        void* TextureManager::ResizePixelsU8(
                const void* inputPixels, int inputWidth, int inputHeight, int channels,
                int outputWidth, int outputHeight
        ) {
            auto* output = allocT(u8, outputWidth * outputHeight * channels);
            stbir_resize_uint8(
                    (const u8*) inputPixels,
                    inputWidth, inputHeight, 0,
                    output,
                    outputWidth, outputHeight, 0,
                    channels
            );
            return output;
        }

        void* TextureManager::ResizePixelsFloat(
                const void* inputPixels, int inputWidth, int inputHeight, int channels,
                int outputWidth, int outputHeight
        ) {
            auto* output = allocT(float, outputWidth * outputHeight * channels);
            stbir_resize_float(
                    (const float*) inputPixels,
                    inputWidth, inputHeight, 0,
                    output,
                    outputWidth, outputHeight, 0,
                    channels
            );
            return output;
        }

        void TextureManager::FlipTexture(Texture &texture) {
            int bpp = BPPTable.at(texture.Format);
            for (auto& layer : texture.Layers) {
                stbi__vertical_flip(layer.Pixels, texture.Width, texture.Height, bpp);
            }
        }

        u32 TextureManager::GetMipLevels(usize width) {
            return (u32)(log(width) / log(2)) + 1;
        }

        void TextureManager::GenerateMips(Texture& texture) {
            for (auto& layer : texture.Layers) {
                GenerateMips(layer, texture.Width, texture.Height, texture.Format);
            }
        }

        void TextureManager::GenerateMips(
                TextureLayer &textureLayer,
                int width, int height,
                const Texture::eFormat &format
        ) {
            int bpp = BPPTable.at(format);
            int channels = ChannelTable.at(format);

            switch (format) {

                case Texture::eFormat::R8:
                    GenerateMipsU8(textureLayer, width, height, bpp, channels);
                    break;

                case Texture::eFormat::R32:
                    GenerateMipsFloat(textureLayer, width, height, bpp, channels);
                    break;

                case Texture::eFormat::R32_TYPELESS:
                    GenerateMipsFloat(textureLayer, width, height, bpp, channels);
                    break;

                case Texture::eFormat::RG8:
                    GenerateMipsU8(textureLayer, width, height, bpp, channels);
                    break;

                case Texture::eFormat::RG32:
                    GenerateMipsFloat(textureLayer, width, height, bpp, channels);
                    break;

                case Texture::eFormat::RGB8:
                    GenerateMipsU8(textureLayer, width, height, bpp, channels);
                    break;

                case Texture::eFormat::RGB32:
                    GenerateMipsFloat(textureLayer, width, height, bpp, channels);
                    break;

                case Texture::eFormat::RGBA8:
                    GenerateMipsU8(textureLayer, width, height, bpp, channels);
                    break;

                case Texture::eFormat::RGBA32:
                    GenerateMipsFloat(textureLayer, width, height, bpp, channels);
                    break;

            }
        }

        void TextureManager::GenerateMipsU8(
                TextureLayer &textureLayer, int width, int height,
                int bpp, int channels
        ) {
            void* previousMip = textureLayer.Pixels;
            while (width != 1) {
                width /= 2;
                height /= 2;

                previousMip = ResizePixelsU8(previousMip, width * 2, height * 2, channels, width, height);

                textureLayer.Mips.emplace_back(previousMip, width * bpp);
            }
        }

        void TextureManager::GenerateMipsFloat(
                TextureLayer &textureLayer, int width, int height,
                int bpp, int channels
        ) {
            void* previousMip = textureLayer.Pixels;
            while (width != 1) {
                width /= 2;
                height /= 2;

                previousMip = ResizePixelsFloat(previousMip, width * 2, height * 2, channels, width, height);

                textureLayer.Mips.emplace_back(previousMip, width * bpp);
            }
        }

        void TextureManager::FreeMips(TextureLayer& textureLayer) {
            for (auto& mip : textureLayer.Mips) {
                if (mip.Pixels != nullptr) {
                    dealloc(mip.Pixels);
                }
            }
            textureLayer.Mips.clear();
        }

    }

}