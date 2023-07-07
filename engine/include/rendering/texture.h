#pragma once

#include <rendering/core.h>

namespace xpe {

    namespace render {

        using namespace core;

        class Context;

        struct ENGINE_API Texture : public GPUResource
        {
            enum class eType
            {
                TEXTURE_1D = 1,
                TEXTURE_2D = 2,
                TEXTURE_3D = 3,
                TEXTURE_CUBE = 4,

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

            enum class eFormat
            {
                R8 = 0,
                R16 = 1,
                R32 = 2,
                RG8 = 3,
                RG16 = 4,
                RG32 = 5,
                RGB8 = 6,
                RGB16 = 7,
                RGB32 = 8,
                RGBA8 = 9,
                RGBA16 = 10,
                RGBA32 = 11
            };

            eType Type = eType::TEXTURE_DEFAULT;
            eUsage Usage = eUsage::DEFAULT;

            usize Width;
            usize Height;
            usize Depth;
            usize ChannelCount;
            eFormat Format;
            boolean OnMemoryPool;
            void* Pixels = nullptr;

            u32 ArraySize = 1;
            u32 MipLevels = 1;
            u32 Slot = 0;
        };

        struct ENGINE_API TextureSampler : public GPUResource
        {
            enum class eComparison
            {
                NEVER = 1,
                LESS = 2,
                EQUAL = 3,
                LESS_EQUAL = 4,
                GREATER	= 5,
                NOT_EQUAL = 6,
                GREATER_EQUAL = 7,
                ALWAYS= 8
            };

            enum class eAddressMode
            {
                WRAP = 1,
                MIRROR = 2,
                CLAMP = 3,
                BORDER = 4,
                MIRROR_ONCE	= 5
            };

            enum class eFilter
            {
                MIN_MAG_MIP_POINT = 1,
            };

            eFilter Filter = eFilter::MIN_MAG_MIP_POINT;
            u32 MaxAnisotropy = 0;
            float MinLOD = 0;
            float MaxLOD = 0;
            float MipLODBias = 0;
            glm::vec4 BorderColor = { 0, 0, 0, 0 };
            u32 Slot = 0;
            eComparison Comparison = eComparison::ALWAYS;
            eAddressMode AddressU = eAddressMode::WRAP;
            eAddressMode AddressV = eAddressMode::WRAP;
            eAddressMode AddressW = eAddressMode::WRAP;
        };

        class ENGINE_API TextureManager final
        {

        public:

            static void Init(Context* context);
            static void Free();

            static Texture* LoadTexture(const char* filePath, const Texture::eFormat& format);
            static void WriteTexture(const char* filePath, const Texture& image, const Texture::eFileFormat& fileFormat);
            static void FreeTexture(Texture& texture);
            static Texture ResizeTexture(Texture& input, usize outputWidth, usize outputHeight);

        private:
            static Context* s_Context;
            static unordered_map<string, Texture> s_TextureTable;

        };

    }

}