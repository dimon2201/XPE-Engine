#pragma once

#include <rendering/core/core.h>

namespace xpe {

    namespace render {

        using namespace core;

        enum class eTextureFormat
        {
            R8, R16, R32, R32_TYPELESS,
            RG8, RG16, RG32,
            RGB8, RGB16, RGB32,
            RGBA8, RGBA16, RGBA32,
            SRGBA8,

            DEFAULT = RGBA8
        };

        struct ENGINE_API Mip final
        {
            void* Pixels = nullptr;
            u32 RowByteSize = 0;

            Mip() = default;
            Mip(void* pixels, u32 rowByteSize) : Pixels(pixels), RowByteSize(rowByteSize) {}
        };

        struct ENGINE_API TextureLayer final
        {
            void* Pixels = nullptr;
            s32 Width, Height, Channels = 0;
            u32 RowByteSize = 0;
            vector<Mip> Mips;

            void Free();

            void CopyFrom(const TextureLayer& other);

            TextureLayer Clone() const;

            void GenerateMips(const eTextureFormat& format, int width, int height);

            void GenerateMipsU8(int width, int height, int bpp, int channels);

            void GenerateMipsFloat(int width, int height, int bpp, int channels);

            void FreeMips();

            void Resize(const eTextureFormat& format, s32 width, s32 height);

            void ResizeU8(s32 width, s32 height);

            void ResizeFloat(s32 width, s32 height);

            void* ResizePixelsU8(
                    const void* inputPixels, int inputWidth, int inputHeight, int channels,
                    int outputWidth, int outputHeight
            );

            void* ResizePixelsFloat(
                    const void* inputPixels, int inputWidth, int inputHeight, int channels,
                    int outputWidth, int outputHeight
            );
        };

        struct ENGINE_API Texture : public GPUResource
        {

            enum class eType
            {
                TEXTURE_1D,
                TEXTURE_2D,
                TEXTURE_2D_MS,
                TEXTURE_2D_DEPTH_STENCIL,
                TEXTURE_2D_ARRAY,
                TEXTURE_3D,
                TEXTURE_CUBE,

                TEXTURE_DEFAULT = TEXTURE_2D
            };

            enum class eUsage
            {
                DEFAULT,
                STATIC,
                DYNAMIC,
                STAGING,
            };

            enum class eFileFormat
            {
                PNG,
                JPG,
                TGA,
                HDR,
                BMP
            };

            // channels count table for each texture format
            static const std::unordered_map<eTextureFormat, int> ChannelTable;
            // bytes per pixel table for each texture format
            static const std::unordered_map<eTextureFormat, int> BPPTable;

            eType Type = eType::TEXTURE_DEFAULT;
            eUsage Usage = eUsage::DEFAULT;

            s32 Width;
            s32 Height;
            s32 Depth = 1;
            s32 Channels;
            eTextureFormat Format;
            u32 SampleCount = 1;

            bool EnableRenderTarget = false;

            u32 Slot = 0;

            u32 MostDetailedMip = 0;

            bool InitializeData = true;
            vector<TextureLayer> Layers;

            ~Texture();

            void Init();
            void Free();

            TextureLayer CreateLayer() const;

            void RemoveLayerAt(u32 index);

            [[nodiscard]] u32 GetMipLevels() const;
            static u32 GetMipsLevels(s32 width);

            void Resize(s32 width, s32 height);

            void Flip();

            void GenerateMips();

            void FlushLayer(u32 index);

            void Flush();

        private:
            void ResizeTextureU8(s32 width, s32 height);
            void ResizeTextureFloat(s32 width, s32 height);
        };

        struct ENGINE_API TextureSampler : public GPUResource
        {

            enum class eComparison
            {
                NEVER,
                LESS,
                EQUAL,
                LESS_EQUAL,
                GREATER,
                NOT_EQUAL,
                GREATER_EQUAL,
                ALWAYS
            };

            enum class eAddress
            {
                WRAP,
                MIRROR,
                CLAMP,
                BORDER,
                MIRROR_ONCE
            };

            enum class eFilter
            {
                MIN_MAG_MIP_POINT,
                MIN_MAG_MIP_LINEAR,
                ANISOTROPIC
            };

            u32 Slot = 0;

            eFilter Filter = eFilter::MIN_MAG_MIP_POINT;

            f32 MinLOD = 0;
            f32 MaxLOD = FLT_MAX;
            f32 MipLODBias = 0;

            u32 AnisotropyLevel = 1;

            glm::vec4 BorderColor = { 0, 0, 0, 0 };

            eComparison Comparison = eComparison::NEVER;

            eAddress AddressU = eAddress::CLAMP;
            eAddress AddressV = eAddress::CLAMP;
            eAddress AddressW = eAddress::CLAMP;
        };

    }

}