#include <texture_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize.h>

namespace xpe {

    namespace res {

        static unordered_map<string, sTexture*>* s_Textures = nullptr;

        void cTextureLoader::Init()
        {
            s_Textures = new unordered_map<string, sTexture*>();
        }

        void cTextureLoader::Free()
        {
            for (auto& texture : *s_Textures) {
                delete texture.second;
            }
            delete s_Textures;
        }

        sTexture* cTextureLoader::Load(const char* filepath, const eTextureFormat &format)
        {
            sTexture* texture = new sTexture();
            texture->Format = format;
            texture->Depth = 1;
            sTextureLayer layer = LoadLayer(filepath, format, texture->Width, texture->Height, texture->Channels);
            texture->Layers.emplace_back(layer);
            texture->Init();
            s_Textures->insert({ filepath, texture });
            return texture;
        }

        sTextureLayer cTextureLoader::LoadLayer(
                const char* filepath,
                const eTextureFormat &format,
                int &width, int &height, int &channels
        ) {
            int desiredChannels = sTexture::k_ChannelTable.at(format);
            sTextureLayer layer;
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

            layer.RowByteSize = w * sTexture::k_BppTable.at(format);
            layer.Width = w;
            layer.Height = h;
            layer.Channels = c;

            width = w;
            height = h;
            channels = c;

            return layer;
        }

        sTexture* cTextureLoader::LoadCube(const sTextureCubeFilepath &cubeFilepath, const eTextureFormat &format)
        {
            sTexture* textureCube = new sTexture();
            textureCube->Type = sTexture::eType::TEXTURE_CUBE;
            textureCube->Format = format;

            sTextureLayer front = LoadLayer(
                    cubeFilepath.FrontFilepath.c_str(),
                    format,
                    textureCube->Width, textureCube->Height, textureCube->Channels
            );

            sTextureLayer back = LoadLayer(
                    cubeFilepath.BackFilepath.c_str(),
                    format,
                    textureCube->Width, textureCube->Height, textureCube->Channels
            );

            sTextureLayer right = LoadLayer(
                    cubeFilepath.RightFilepath.c_str(),
                    format,
                    textureCube->Width, textureCube->Height, textureCube->Channels
            );

            sTextureLayer left = LoadLayer(
                    cubeFilepath.LeftFilepath.c_str(),
                    format,
                    textureCube->Width, textureCube->Height, textureCube->Channels
            );

            sTextureLayer top = LoadLayer(
                    cubeFilepath.TopFilepath.c_str(),
                    format,
                    textureCube->Width, textureCube->Height, textureCube->Channels
            );

            sTextureLayer bottom = LoadLayer(
                    cubeFilepath.BottomFilepath.c_str(),
                    format,
                    textureCube->Width, textureCube->Height, textureCube->Channels
            );

            // !IMPORTANT!
            // DX11 has specific order of texture layers for cube mapping
#ifdef DX11
            textureCube->Layers = {
                right,
                left,
                top,
                bottom,
                front,
                back,
            };

#else       // other APIs probably have same order as OpenGL as following
            textureCube->Layers = {
                front,
                back,
                right,
                left,
                top,
                bottom,
            };
#endif

            textureCube->Init();
            s_Textures->insert({ cubeFilepath.Name, textureCube });
            return textureCube;
        }

        bool cTextureLoader::Save(
                const char* filepath,
                const sTexture &texture,
                const sTexture::eFileFormat &fileFormat
        ) {
            return SaveLayer(filepath, texture.Layers.front(), fileFormat);
        }

        bool cTextureLoader::SaveLayer(
                const char* filepath,
                const sTextureLayer &textureLayer,
                const sTexture::eFileFormat &fileFormat
        ) {

            switch (fileFormat) {

                case sTexture::eFileFormat::PNG:
                    return stbi_write_png(filepath, textureLayer.Width, textureLayer.Height, textureLayer.Channels, textureLayer.Pixels, 0);

                case sTexture::eFileFormat::JPG:
                    return stbi_write_jpg(filepath, textureLayer.Width, textureLayer.Height, textureLayer.Channels, textureLayer.Pixels, 3);

                case sTexture::eFileFormat::TGA:
                    return stbi_write_tga(filepath, textureLayer.Width, textureLayer.Height, textureLayer.Channels, textureLayer.Pixels);

                case sTexture::eFileFormat::HDR:
                    return stbi_write_hdr(filepath, textureLayer.Width, textureLayer.Height, textureLayer.Channels, (float*) textureLayer.Pixels);

                case sTexture::eFileFormat::BMP:
                    return stbi_write_bmp(filepath, textureLayer.Width, textureLayer.Height, textureLayer.Channels, textureLayer.Pixels);

            }

            return false;
        }

    }

}