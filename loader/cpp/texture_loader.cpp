#include <texture_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize.h>

namespace xpe {

    namespace res {

        unordered_map<string, TextureLayer>* TextureLoader::s_Layers = nullptr;

        Ref<Texture> TextureLoader::Load(const char* filepath, const eTextureFormat &format)
        {
            Ref<Texture> texture;
            texture.Create();
            texture->Format = format;
            texture->Depth = 1;
            TextureLayer layer = LoadLayer(filepath, format, texture->Width, texture->Height, texture->Channels);
            texture->Layers.emplace_back(layer);
            texture->Init();
            return texture;
        }

        TextureLayer TextureLoader::LoadLayer(
                const char* filepath,
                const eTextureFormat &format,
                int &width, int &height, int &channels
        ) {
            if (s_Layers == nullptr) {
                s_Layers = new unordered_map<string, TextureLayer>();
            }

            if (s_Layers->find(filepath) != s_Layers->end()) {
                return s_Layers->at(filepath).Clone();
            }

            int desiredChannels = Texture::ChannelTable.at(format);
            TextureLayer layer;
            int w;
            int h;
            int c;

            switch (format) {

                case eTextureFormat::R8:
                    layer.Pixels = stbi_load(filepath, &w, &h, &c, desiredChannels);
                    break;

                case eTextureFormat::R16:
                    layer.Pixels = stbi_load_16(filepath, &w, &h, &c, desiredChannels);
                    break;

                case eTextureFormat::R32:
                    layer.Pixels = stbi_loadf(filepath, &w, &h, &c, desiredChannels);
                    break;

                case eTextureFormat::RG8:
                    layer.Pixels = stbi_load(filepath, &w, &h, &c, desiredChannels);
                    break;

                case eTextureFormat::RG16:
                    layer.Pixels = stbi_load_16(filepath, &w, &h, &c, desiredChannels);
                    break;

                case eTextureFormat::RG32:
                    layer.Pixels = stbi_loadf(filepath, &w, &h, &c, desiredChannels);
                    break;

                case eTextureFormat::RGB8:
                    layer.Pixels = stbi_load(filepath, &w, &h, &c, desiredChannels);
                    break;

                case eTextureFormat::RGB16:
                    layer.Pixels = stbi_load_16(filepath, &w, &h, &c, desiredChannels);
                    break;

                case eTextureFormat::RGB32:
                    layer.Pixels = stbi_loadf(filepath, &w, &h, &c, desiredChannels);
                    break;

                case eTextureFormat::RGBA8:
                    layer.Pixels = stbi_load(filepath, &w, &h, &c, desiredChannels);
                    break;

                case eTextureFormat::RGBA16:
                    layer.Pixels = stbi_load_16(filepath, &w, &h, &c, desiredChannels);
                    break;

                case eTextureFormat::RGBA32:
                    layer.Pixels = stbi_loadf(filepath, &w, &h, &c, desiredChannels);
                    break;

                case eTextureFormat::SRGBA8:
                    layer.Pixels = stbi_load(filepath, &w, &h, &c, desiredChannels);
                    break;

            }

            layer.RowByteSize = w * Texture::BPPTable.at(format);
            layer.Width = w;
            layer.Height = h;
            layer.Channels = c;

            width = w;
            height = h;
            channels = c;

            return layer;
        }

        Ref<Texture> TextureLoader::LoadCube(const TextureCubeFilepath &cubeFilepath, const eTextureFormat &format)
        {
            Ref<Texture> textureCube;
            textureCube.Create();
            textureCube->Type = Texture::eType::TEXTURE_CUBE;
            textureCube->Format = format;

            TextureLayer front = LoadLayer(
                    cubeFilepath.FrontFilepath.c_str(),
                    format,
                    textureCube->Width, textureCube->Height, textureCube->Channels
            );

            TextureLayer back = LoadLayer(
                    cubeFilepath.BackFilepath.c_str(),
                    format,
                    textureCube->Width, textureCube->Height, textureCube->Channels
            );

            TextureLayer right = LoadLayer(
                    cubeFilepath.RightFilepath.c_str(),
                    format,
                    textureCube->Width, textureCube->Height, textureCube->Channels
            );

            TextureLayer left = LoadLayer(
                    cubeFilepath.LeftFilepath.c_str(),
                    format,
                    textureCube->Width, textureCube->Height, textureCube->Channels
            );

            TextureLayer top = LoadLayer(
                    cubeFilepath.TopFilepath.c_str(),
                    format,
                    textureCube->Width, textureCube->Height, textureCube->Channels
            );

            TextureLayer bottom = LoadLayer(
                    cubeFilepath.BottomFilepath.c_str(),
                    format,
                    textureCube->Width, textureCube->Height, textureCube->Channels
            );

            textureCube->Layers = {
                front,
                back,
                right,
                left,
                top,
                bottom,
            };

            textureCube->Init();

            return textureCube;
        }

        bool TextureLoader::Save(
                const char* filepath,
                const Texture &texture,
                const Texture::eFileFormat &fileFormat
        ) {
            return SaveLayer(filepath, texture.Layers.front(), fileFormat);
        }

        bool TextureLoader::SaveLayer(
                const char *filepath,
                const TextureLayer &textureLayer,
                const Texture::eFileFormat &fileFormat
        ) {

            switch (fileFormat) {

                case Texture::eFileFormat::PNG:
                    return stbi_write_png(filepath, textureLayer.Width, textureLayer.Height, textureLayer.Channels, textureLayer.Pixels, 0);

                case Texture::eFileFormat::JPG:
                    return stbi_write_jpg(filepath, textureLayer.Width, textureLayer.Height, textureLayer.Channels, textureLayer.Pixels, 3);

                case Texture::eFileFormat::TGA:
                    return stbi_write_tga(filepath, textureLayer.Width, textureLayer.Height, textureLayer.Channels, textureLayer.Pixels);

                case Texture::eFileFormat::HDR:
                    return stbi_write_hdr(filepath, textureLayer.Width, textureLayer.Height, textureLayer.Channels, (float*) textureLayer.Pixels);

                case Texture::eFileFormat::BMP:
                    return stbi_write_bmp(filepath, textureLayer.Width, textureLayer.Height, textureLayer.Channels, textureLayer.Pixels);

            }

            return false;
        }

    }

}