#include <texture_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize.h>

namespace xpe {

    namespace res {

        Ref<Texture> TextureLoader::Load(const char* filepath, const eTextureFormat &format)
        {
            Ref<Texture> texture = m_Storage->Add(filepath, {});
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

            }

            width = w;
            height = h;
            channels = c;

            layer.RowByteSize = width * Texture::BPPTable.at(format);
            layer.FromFile = K_TRUE;

            return layer;
        }

        Ref<Texture> TextureLoader::LoadCube(const TextureCubeFilepath &cubeFilepath, const eTextureFormat &format)
        {
            Ref<Texture> textureCube = m_Storage->Add(cubeFilepath.Name, {});
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

        void TextureLoader::Save(const char* filepath, const Texture &texture, const Texture::eFileFormat &fileFormat)
        {
            void* pixels = texture.Layers.front().Pixels;

            switch (fileFormat) {

                case Texture::eFileFormat::PNG:
                    stbi_write_png(filepath, texture.Width, texture.Height, texture.Channels, pixels, 0);
                    break;

                case Texture::eFileFormat::JPG:
                    stbi_write_jpg(filepath, texture.Width, texture.Height, texture.Channels, pixels, 3);
                    break;

                case Texture::eFileFormat::TGA:
                    stbi_write_tga(filepath, texture.Width, texture.Height, texture.Channels, pixels);
                    break;

                case Texture::eFileFormat::HDR:
                    stbi_write_hdr(filepath, texture.Width, texture.Height, texture.Channels, (float*) pixels);
                    break;

                case Texture::eFileFormat::BMP:
                    stbi_write_bmp(filepath, texture.Width, texture.Height, texture.Channels, pixels);
                    break;

            }
        }

    }

}