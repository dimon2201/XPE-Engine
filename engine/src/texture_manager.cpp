#include <texture_manager.hpp>
#include <render_context.hpp>
#include <math_manager.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize.h>

namespace xpe
{
    namespace render
    {
        const std::unordered_map<eTextureFormat, int> cTexture::k_ChannelTable =
        {
                { eTextureFormat::R8, 1 },
                { eTextureFormat::R16, 1 },
                { eTextureFormat::R32, 1 },

                { eTextureFormat::RG8, 2 },
                { eTextureFormat::RG16, 2 },
                { eTextureFormat::RG32, 2 },

                { eTextureFormat::RGB8, 3 },
                { eTextureFormat::RGB16, 3 },
                { eTextureFormat::RGB32, 3 },

                { eTextureFormat::RGBA8, 4 },
                { eTextureFormat::RGBA16, 4 },
                { eTextureFormat::RGBA32, 4 },

                { eTextureFormat::SRGBA8, 4 }
        };

        const std::unordered_map<eTextureFormat, int> cTexture::k_BppTable =
        {
                { eTextureFormat::R8, 1 },
                { eTextureFormat::R16, 2 },
                { eTextureFormat::R32, 4 },

                { eTextureFormat::RG8, 2 },
                { eTextureFormat::RG16, 4 },
                { eTextureFormat::RG32, 8 },

                { eTextureFormat::RGB8, 3 },
                { eTextureFormat::RGB16, 6 },
                { eTextureFormat::RGB32, 12 },

                { eTextureFormat::RGBA8, 4 },
                { eTextureFormat::RGBA16, 8 },
                { eTextureFormat::RGBA32, 16 },

                { eTextureFormat::SRGBA8, 4 },
        };

        cTexture::cTexture()
        {
            ViewType = eViewType::SRV;
        }

        cTexture::~cTexture()
        {
            for (auto& layer : Layers)
            {
                layer.Free();
            }
            Layers.clear();
            Free();
        }

        void cTexture::Init()
        {
            context::CreateTexture(*this);
        }

        void cTexture::Free()
        {
            context::FreeTexture(*this);
            MEvent::WindowFrameResized.Remove((u64) this);
        }

        cTextureLayer cTexture::CreateLayer() const
        {
            cTextureLayer layer;
            usize size = Width * Height * k_BppTable.at(Format);
            layer.Width = Width;
            layer.Height = Height;
            layer.Format = Format;
            layer.Pixels = main_alloc(size);
            memset(layer.Pixels, 0, size);
            return layer;
        }

        void cTexture::RemoveLayerAt(u32 index)
        {
            auto& layer = Layers[index];
            layer.Free();
            Layers.erase(Layers.begin() + index);
        }

        void cTexture::WindowFrameResized(s32 width, s32 height)
        {
            Resize(width, height);
        }

        void cTexture::Resize(s32 width, s32 height)
        {
            context::FreeTexture(*this);
            Width = width;
            Height = height;
            context::CreateTexture(*this);
        }

        void cTexture::ResizePixels(s32 width, s32 height)
        {
            switch (Format)
            {
                case eTextureFormat::R8:
                    ResizeTextureU8(width, height);
                    break;

                case eTextureFormat::R32:
                    ResizeTextureFloat(width, height);
                    break;

                case eTextureFormat::RG8:
                    ResizeTextureU8(width, height);
                    break;

                case eTextureFormat::RG32:
                    ResizeTextureFloat(width, height);
                    break;

                case eTextureFormat::RGB8:
                    ResizeTextureU8(width, height);
                    break;

                case eTextureFormat::RGB32:
                    ResizeTextureFloat(width, height);
                    break;

                case eTextureFormat::RGBA8:
                    ResizeTextureU8(width, height);
                    break;

                case eTextureFormat::SRGBA8:
                    ResizeTextureU8(width, height);
                    break;

                case eTextureFormat::RGBA32:
                    ResizeTextureFloat(width, height);
                    break;
            }

            Width = width;
            Height = height;
        }

        void cTexture::ResizeTextureU8(s32 width, s32 height)
        {
            for (auto& layer : Layers)
            {
                layer.ResizeU8(width, height);
            }
        }

        void cTexture::ResizeTextureFloat(s32 width, s32 height)
        {
            for (auto& layer : Layers)
            {
                layer.ResizeFloat(width, height);
            }
        }

        void cTexture::Flip()
        {
            int bpp = k_BppTable.at(Format);
            for (auto& layer : Layers)
            {
                stbi__vertical_flip(layer.Pixels, Width, Height, bpp);
            }
        }

        void cTexture::FlushLayer(u32 index)
        {
            context::CopyTexture(*this, Layers[index].Pixels, Width * Height * k_BppTable.at(Format), index);
        }

        void cTexture::Flush()
        {
            context::FreeTexture(*this);
            context::CreateTexture(*this);
        }

        void cTexture::SetResizable(bool resizable)
        {
            m_Resizable = resizable;
            if (resizable)
            {
                MEvent::WindowFrameResized.Add((u64) this, eWindowFrameResizedPriority::TEXTURE, [this](int w, int h)
                {
                    WindowFrameResized(w, h);
                });
            }
            else
            {
                MEvent::WindowFrameResized.Remove((u64) this);
            }
        }

        void cTexture::SetMipmapping(bool enable)
        {
            if (enable == EnableMipmapping) return;
            EnableMipmapping = enable;
            context::FreeTexture(*this);
            context::CreateTexture(*this);
        }

        void cTextureLayer::Free()
        {
            if (Pixels != nullptr)
            {
                main_free(Pixels);
                Pixels = nullptr;
            }
        }

        void cTextureLayer::CopyFrom(const cTextureLayer &other)
        {
            Width = other.Width;
            Height = other.Height;
            Format = other.Format;
            usize size = Width * Height * cTexture::k_BppTable.at(Format);
            main_free(Pixels);
            Pixels = main_alloc(size);
            memcpy(Pixels, other.Pixels, size);
        }

        cTextureLayer cTextureLayer::Clone() const
        {
            cTextureLayer clone;
            clone.CopyFrom(*this);
            return clone;
        }

        void cTextureLayer::Resize(const eTextureFormat &format, s32 width, s32 height)
        {
            switch (format)
            {
                case eTextureFormat::R8:
                    ResizeU8(width, height);
                    break;

                case eTextureFormat::R32:
                    ResizeFloat(width, height);
                    break;

                case eTextureFormat::RG8:
                    ResizeU8(width, height);
                    break;

                case eTextureFormat::RG32:
                    ResizeFloat(width, height);
                    break;

                case eTextureFormat::RGB8:
                    ResizeU8(width, height);
                    break;

                case eTextureFormat::RGB32:
                    ResizeFloat(width, height);
                    break;

                case eTextureFormat::RGBA8:
                    ResizeU8(width, height);
                    break;

                case eTextureFormat::SRGBA8:
                    ResizeU8(width, height);
                    break;

                case eTextureFormat::RGBA32:
                    ResizeFloat(width, height);
                    break;
            }
        }

        void cTextureLayer::ResizeU8(s32 width, s32 height)
        {
            void* pixels = ResizePixelsU8(Pixels, Width, Height, width, height);
            Pixels = pixels;
            Width = width;
            Height = height;
        }

        void cTextureLayer::ResizeFloat(s32 width, s32 height)
        {
            void* pixels = ResizePixelsFloat(Pixels, Width, Height, width, height);
            Pixels = pixels;
            Width = width;
            Height = height;
        }

        void* cTextureLayer::ResizePixelsU8(
                const void* inputPixels, int inputWidth, int inputHeight,
                int outputWidth, int outputHeight
        )
        {
            int channels = cTexture::k_ChannelTable.at(Format);
            auto* output = main_allocT(u8, outputWidth * outputHeight * channels);
            stbir_resize_uint8(
                    (const u8*) inputPixels,
                    inputWidth, inputHeight, 0,
                    output,
                    outputWidth, outputHeight, 0,
                    channels
            );
            return output;
        }

        void* cTextureLayer::ResizePixelsFloat(
                const void* inputPixels, int inputWidth, int inputHeight,
                int outputWidth, int outputHeight
        )
        {
            int channels = cTexture::k_ChannelTable.at(Format);
            auto* output = main_allocT(float, outputWidth * outputHeight * channels);
            stbir_resize_float(
                    (const float*) inputPixels,
                    inputWidth, inputHeight, 0,
                    output,
                    outputWidth, outputHeight, 0,
                    channels
            );
            return output;
        }

        void cAtlas::AddLayer()
        {
            Layers.emplace_back(CreateLayer());
        }

        cSampler::~cSampler()
        {
            context::FreeSampler(*this);
        }

        void cSampler::Init()
        {
            context::CreateSampler(*this);
        }

        static unordered_map<string, cTexture*>* s_Textures = nullptr;
        static unordered_map<string, cTextureLayer>* s_TextureLayers = nullptr;

        void MTexture::Init()
        {
            s_Textures = new unordered_map<string, cTexture*>();
            s_TextureLayers = new unordered_map<string, cTextureLayer>();
        }

        void MTexture::Free()
        {
            for (auto& texture : *s_Textures)
            {
                delete texture.second;
            }
            delete s_Textures;

            for (auto& textureLayer : *s_TextureLayers)
            {
                textureLayer.second.Free();
            }
            delete s_TextureLayers;
        }

        cTexture* MTexture::Load(const string& name, const char* filepath, const eTextureFormat &format)
        {
            if (s_Textures->find(name) != s_Textures->end())
            {
                return s_Textures->at(name);
            }

            int width, height, channels;

            cTextureLayer* textureLayer = LoadLayer(filepath, format, width, height, channels);

            if (textureLayer == nullptr)
            {
                return nullptr;
            }

            cTexture* texture = new cTexture();
            texture->Width = width;
            texture->Height = height;
            texture->Channels = channels;
            texture->Format = format;
            texture->Depth = 1;
            texture->Layers.emplace_back(*textureLayer);
            texture->Init();
            s_Textures->insert({ name, texture });
            return texture;
        }

        cTextureLayer* MTexture::LoadLayer(
                const char* filepath,
                const eTextureFormat &format,
                int &width, int &height, int &channels
        )
        {
            if (MFile::NotExists(filepath))
            {
                LogError("Texture not found in {}", filepath);
                return nullptr;
            }

            if (s_TextureLayers->find(filepath) != s_TextureLayers->end())
            {
                return &s_TextureLayers->at(filepath);
            }

            int desiredChannels = cTexture::k_ChannelTable.at(format);
            cTextureLayer layer;
            int w;
            int h;
            int c;

            switch (format)
            {
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

            s_TextureLayers->insert({ filepath, layer });
            return &s_TextureLayers->at(filepath);
        }

        cTexture* MTexture::LoadCubemap(const string& name, const sTextureCubeFilepath &cubeFilepath, const eTextureFormat &format)
        {
            if (s_Textures->find(name) != s_Textures->end())
            {
                return s_Textures->at(name);
            }

            int width, height, channels;

            cTextureLayer* front = LoadLayer(
                    cubeFilepath.FrontFilepath.c_str(),
                    format,
                    width, height, channels
            );

            if (front == nullptr)
            {
                return nullptr;
            }

            cTextureLayer* back = LoadLayer(
                    cubeFilepath.BackFilepath.c_str(),
                    format,
                    width, height, channels
            );

            if (back == nullptr)
            {
                return nullptr;
            }

            cTextureLayer* right = LoadLayer(
                    cubeFilepath.RightFilepath.c_str(),
                    format,
                    width, height, channels
            );

            if (right == nullptr)
            {
                return nullptr;
            }

            cTextureLayer* left = LoadLayer(
                    cubeFilepath.LeftFilepath.c_str(),
                    format,
                    width, height, channels
            );

            if (left == nullptr)
            {
                return nullptr;
            }

            cTextureLayer* top = LoadLayer(
                    cubeFilepath.TopFilepath.c_str(),
                    format,
                    width, height, channels
            );

            if (top == nullptr)
            {
                return nullptr;
            }

            cTextureLayer* bottom = LoadLayer(
                    cubeFilepath.BottomFilepath.c_str(),
                    format,
                    width, height, channels
            );

            if (bottom == nullptr)
            {
                return nullptr;
            }

            cTexture* cubemap = new cTexture();
            cubemap->Type = cTexture::eType::TEXTURE_CUBE;
            cubemap->Format = format;
            cubemap->Width = width;
            cubemap->Height = height;
            cubemap->Channels = channels;

            // !IMPORTANT!
            // DX11 has specific order of texture layers for cube mapping
#ifdef DX11
            cubemap->Layers = {
                *right,
                *left,
                *top,
                *bottom,
                *front,
                *back,
            };

#else       // other APIs probably have same order as OpenGL as following
            cubemap->Layers = {
                *front,
                *back,
                *right,
                *left,
                *top,
                *bottom,
            };
#endif

            cubemap->Init();
            s_Textures->insert({ name, cubemap });
            return cubemap;
        }

        bool MTexture::SaveLayer(
                const char* filepath,
                cTextureLayer* textureLayer,
                const eFileFormat &fileFormat
        )
        {
            if (textureLayer == nullptr || textureLayer->Pixels == nullptr)
            {
                LogWarning("Unable to save NULL texture layer into file {}", filepath);
                return false;
            }

            int channels = cTexture::k_ChannelTable.at(textureLayer->Format);

            switch (fileFormat)
            {
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

        void cCircleFilter3D::Init()
        {
            Type = eType::TEXTURE_3D;
            Format = eTextureFormat::RGBA32;
            Channels = k_ChannelTable.at(Format);
            InitializeData = true;
            EnableMipmapping = false;
            int samplesSize = Width * Height * Depth * 4;
            vector<float> samples;
            samples.resize(samplesSize);
            int index = 0;

            for (int texY = 0; texY < Height; texY++)
            {
                for (int texX = 0; texX < Width; texX++)
                {
                    for (int v = FilterSize - 1; v >= 0; v--)
                    {
                        for (int u = 0; u < FilterSize; u++)
                        {
                            float x = ((float)u + 0.5f + Random(-0.5f, 0.5f)) / (float) FilterSize;
                            float y = ((float)v + 0.5f + Random(-0.5f, 0.5f)) / (float) FilterSize;

                            assert(index + 1 < samples.size());
                            samples[index] = sqrtf(y) * cosf(2 * K_PI * x);
                            samples[index + 1] = sqrtf(y) * sinf(2 * K_PI * x);

                            index += 2;
                        }
                    }
                }
            }

            if (Layers.empty())
            {
                Layers.emplace_back();
                Layers[0].Pixels = main_alloc(samplesSize * sizeof(float));
            }

            else if (Layers[0].Width != Width || Layers[0].Height != Height || Layers[0].Depth != Depth)
            {
                Layers[0].Free();
                Layers[0].Pixels = main_alloc(samplesSize * sizeof(float));
            }

            Layers[0].Format = Format;
            Layers[0].Width  = Width;
            Layers[0].Height = Height;
            Layers[0].Depth  = Depth;
            memcpy(Layers[0].Pixels, samples.data(), samplesSize * sizeof(float));

            cTexture::Init();
        }
    }
}