#include <texture_loader.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize.h>

namespace xpe {

    namespace res {

        static vector<cTexture*>* s_Textures = nullptr;
        static unordered_map<string, sTextureLayer>* s_TextureLayers = nullptr;

        void cTextureLoader::Init()
        {
            s_Textures = new vector<cTexture*>();
            s_TextureLayers = new unordered_map<string, sTextureLayer>();
        }

        void cTextureLoader::Free()
        {
            for (auto& texture : *s_Textures) {
                delete texture;
            }
            delete s_Textures;

            for (auto& textureLayer : *s_TextureLayers) {
                textureLayer.second.Free();
            }
            delete s_TextureLayers;
        }

        cTexture* cTextureLoader::Load(const char* filepath, const eTextureFormat &format)
        {
            s32 width, height, channelCount;
            width = height = channelCount = 0;

            sTextureLayer textureLayer = LoadLayer(filepath, format, width, height, channelCount);
            cTexture* texture = new cTexture(
                cTexture::eType::TEXTURE_2D,
                cResource::eViewType::SRV,
                cTexture::eUsage::DYNAMIC,
                glm::vec3(width, height, 1),
                channelCount,
                format,
                1,
                false,
                0,
                0,
                true,
                { textureLayer }
            );
            s_Textures->emplace_back(texture);

            return texture;
        }

        sTextureLayer cTextureLoader::LoadLayer(
                const char* filepath,
                const eTextureFormat &format,
                int &width, int &height, int &channels
        ) {
            int desiredChannels = cTexture::k_ChannelTable.at(format);
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

            layer.Width = w;
            layer.Height = h;
            layer.Format = format;

            width = w;
            height = h;
            channels = c;

            return layer;
        }

        cTexture* cTextureLoader::LoadCube(const sTextureCubeFilepath &cubeFilepath, const eTextureFormat &format)
        {
            s32 width, height, channelCount;
            width = height = channelCount = 0;

            sTextureLayer front = LoadLayer(cubeFilepath.FrontFilepath.c_str(), format, width, height, channelCount);
            sTextureLayer back = LoadLayer(cubeFilepath.BackFilepath.c_str(), format, width, height, channelCount);
            sTextureLayer right = LoadLayer(cubeFilepath.RightFilepath.c_str(), format, width, height, channelCount);
            sTextureLayer left = LoadLayer(cubeFilepath.LeftFilepath.c_str(), format, width, height, channelCount);
            sTextureLayer top = LoadLayer(cubeFilepath.TopFilepath.c_str(), format, width, height, channelCount);
            sTextureLayer bottom = LoadLayer(cubeFilepath.BottomFilepath.c_str(), format, width, height, channelCount);

            cTexture* textureCube = new cTexture(
                cTexture::eType::TEXTURE_CUBE,
                cResource::eViewType::SRV,
                cTexture::eUsage::DEFAULT,
                glm::vec3(width, height, 1),
                channelCount,
                format,
                1,
                false,
                0,
                0,
                true,
                { right, left, top, bottom, front, back }
            );

            s_Textures->emplace_back(textureCube);
            return textureCube;
        }

        bool cTextureLoader::SaveLayer(
                const char* filepath,
                sTextureLayer* textureLayer,
                const eFileFormat &fileFormat
        ) {
            if (textureLayer == nullptr || textureLayer->Pixels == nullptr) {
                LogWarning("Unable to save NULL texture layer into file {}", filepath);
                return false;
            }

            int channels = cTexture::k_ChannelTable.at(textureLayer->Format);

            switch (fileFormat) {

                case eFileFormat::PNG:
                    return stbi_write_png(filepath, textureLayer->Width, textureLayer->Height, channels, textureLayer->Pixels, 0);

                case eFileFormat::JPG:
                    return stbi_write_jpg(filepath, textureLayer->Width, textureLayer->Height, channels, textureLayer->Pixels, 3);

                case eFileFormat::TGA:
                    return stbi_write_tga(filepath, textureLayer->Width, textureLayer->Height, channels, textureLayer->Pixels);

                case eFileFormat::HDR:
                    return stbi_write_hdr(filepath, textureLayer->Width, textureLayer->Height, channels, (float*) textureLayer->Pixels);

                case eFileFormat::BMP:
                    return stbi_write_bmp(filepath, textureLayer->Width, textureLayer->Height, channels, textureLayer->Pixels);

            }

            return false;
        }

    }

}