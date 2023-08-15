#include <rendering/core/texture.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_resize.h>

#include <rendering/core/context.hpp>

namespace xpe {

    namespace render {

        const std::unordered_map<eTextureFormat, int> Texture::ChannelTable =
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
                { eTextureFormat::RGBA32, 4 }

        };

        const std::unordered_map<eTextureFormat, int> Texture::BPPTable =
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
                { eTextureFormat::RGBA32, 16 }

        };

        Texture::~Texture()
        {
            for (auto& layer : Layers)
            {
                layer.Free();
            }
            Layers.clear();
            context::FreeTexture(*this);
        }

        void Texture::Init()
        {
            context::CreateTexture(*this);
        }

        TextureLayer Texture::CreateLayer() const
        {
            TextureLayer layer;
            layer.RowByteSize = Width * BPPTable.at(Format);
            layer.Pixels = alloc(layer.RowByteSize);
            return layer;
        }

        void Texture::RemoveLayerAt(u32 index)
        {
            auto& layer = Layers[index];
            layer.Free();
            Layers.erase(Layers.begin() + index);
        }

        u32 Texture::GetMipLevels() const
        {
            if (Layers.empty()) {
                return 1;
            }

            auto& mips = Layers.front().Mips;
            return mips.empty() ? 1 : mips.size();
        }

        u32 Texture::GetMipsLevels(s32 width)
        {
            return (u32)(log(width) / log(2)) + 1;
        }

        void Texture::Resize(s32 width, s32 height)
        {
            Texture& input = *this;
            Texture output = {};
            output.Width = width;
            output.Height = height;

            switch (input.Format) {

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

                case eTextureFormat::RGBA32:
                    ResizeTextureFloat(width, height);
                    break;

            }

            *this = output;
        }

        void Texture::ResizeTextureU8(s32 width, s32 height) {
            for (auto& layer : Layers) {
                layer.ResizeU8(
                        Width, Height, Channels,
                        width, height
                );
            }
        }

        void Texture::ResizeTextureFloat(s32 width, s32 height) {
            for (auto& layer : Layers) {
                layer.ResizeFloat(
                        Width, Height, Channels,
                        width, height
                );
            }
        }

        void Texture::Flip()
        {
            int bpp = BPPTable.at(Format);
            for (auto& layer : Layers) {
                stbi__vertical_flip(layer.Pixels, Width, Height, bpp);
            }
        }

        void Texture::GenerateMips()
        {
            for (auto& layer : Layers) {
                layer.GenerateMips(Width, Height, Format);
            }
        }

        void Texture::FlushLayer(u32 index)
        {
            context::WriteTexture(
                    *this,
                    Layers[index].Pixels,
                    Width * Height * BPPTable.at(Format),
                    index
            );
        }

        void Texture::Flush()
        {
            // todo(cheerwizard): crashes sometimes in WriteTexture, so need to be fixed, before using it.
//            u32 layerCount = Layers.size();
//            for (u32 i = 0 ; i < layerCount ; i++)
//            {
//                FlushLayer(i);
//            }
            context::FreeTexture(*this);
            context::CreateTexture(*this);
        }

        void TextureLayer::Free()
        {
            if (Pixels != nullptr) {
                if (FromFile == K_TRUE) {
                    stbi_image_free(Pixels);
                }
                dealloc(Pixels);
                FreeMips();
            }
        }

        void TextureLayer::GenerateMips(int width, int height, const eTextureFormat &format)
        {
            if (Pixels == nullptr) return;

            int bpp = Texture::BPPTable.at(format);
            int channels = Texture::ChannelTable.at(format);

            switch (format) {

                case eTextureFormat::R8:
                    GenerateMipsU8(width, height, bpp, channels);
                    break;

                case eTextureFormat::R32:
                    GenerateMipsFloat(width, height, bpp, channels);
                    break;

                case eTextureFormat::R32_TYPELESS:
                    GenerateMipsFloat(width, height, bpp, channels);
                    break;

                case eTextureFormat::RG8:
                    GenerateMipsU8(width, height, bpp, channels);
                    break;

                case eTextureFormat::RG32:
                    GenerateMipsFloat(width, height, bpp, channels);
                    break;

                case eTextureFormat::RGB8:
                    GenerateMipsU8(width, height, bpp, channels);
                    break;

                case eTextureFormat::RGB32:
                    GenerateMipsFloat(width, height, bpp, channels);
                    break;

                case eTextureFormat::RGBA8:
                    GenerateMipsU8(width, height, bpp, channels);
                    break;

                case eTextureFormat::RGBA32:
                    GenerateMipsFloat(width, height, bpp, channels);
                    break;

            }
        }

        void TextureLayer::GenerateMipsU8(int width, int height, int bpp, int channels)
        {
            void* previousMip = Pixels;
            while (width != 1 && height != 1)
            {
                width /= 2;
                height /= 2;

                previousMip = ResizePixelsU8(previousMip, width * 2, height * 2, channels, width, height);

                Mips.emplace_back(previousMip, width * bpp);
            }
        }

        void TextureLayer::GenerateMipsFloat(int width, int height, int bpp, int channels) {
            void* previousMip = Pixels;
            while (width != 1 && height != 1)
            {
                width /= 2;
                height /= 2;

                previousMip = ResizePixelsFloat(previousMip, width * 2, height * 2, channels, width, height);

                Mips.emplace_back(previousMip, width * bpp);
            }
        }

        void TextureLayer::FreeMips() {
            for (auto& mip : Mips) {
                if (mip.Pixels != nullptr) {
                    dealloc(mip.Pixels);
                }
            }
            Mips.clear();
        }

        void TextureLayer::ResizeU8(
                int inputWidth, int inputHeight, int channels,
                int outputWidth, int outputHeight
        ) {
            TextureLayer output = {};

            output.Pixels = ResizePixelsU8(
                    Pixels,
                    inputWidth, inputHeight, channels,
                    outputWidth, outputHeight
            );

            if (!Mips.empty()) {
                output.FreeMips();
                int bpp = sizeof(u8) * channels;
                output.GenerateMipsU8(outputWidth, outputHeight, bpp, channels);
            }

            *this = output;
        }

        void TextureLayer::ResizeFloat(
                int inputWidth, int inputHeight, int channels,
                int outputWidth, int outputHeight
        ) {
            TextureLayer output = {};

            output.Pixels = ResizePixelsFloat(
                    Pixels,
                    inputWidth, inputHeight, channels,
                    outputWidth, outputHeight
            );

            if (!Mips.empty()) {
                output.FreeMips();
                int bpp = sizeof(float) * channels;
                output.GenerateMipsFloat(outputWidth, outputHeight, bpp, channels);
            }

            *this = output;
        }

        void* TextureLayer::ResizePixelsU8(
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

        void* TextureLayer::ResizePixelsFloat(
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

    }

}