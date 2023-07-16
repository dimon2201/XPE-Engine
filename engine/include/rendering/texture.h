#pragma once

#include <rendering/core.h>

namespace xpe {

    namespace render {

        using namespace core;

        class Context;

        struct Texture;

        struct ENGINE_API TextureComponent : public Component {

            TextureComponent(const string& usid) : Component(usid) {}

            Texture* Texture = nullptr;

        };

        struct ENGINE_API TextureLayer final {
            void* Pixels = nullptr;
        };

        struct ENGINE_API Texture : public GPUResource
        {
            enum class eType
            {
                TEXTURE_1D,
                TEXTURE_2D,
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

            enum class eFormat
            {
                R8, R16, R32,
                RG8, RG16, RG32,
                RGB8, RGB16, RGB32,
                RGBA8, RGBA16, RGBA32
            };

            eType Type = eType::TEXTURE_DEFAULT;
            eUsage Usage = eUsage::DEFAULT;

            usize Width;
            usize Height;
            usize Depth;
            usize ChannelCount;
            eFormat Format;
            Boolean OnMemoryPool;

            u32 MipLevels = 1;
            u32 MostDetailedMip = 0;
            u32 Slot = 0;

            vector<TextureLayer> Layers;
            bool InitializeData = true;
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

            enum class eAddressMode
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
            };

            eFilter Filter = eFilter::MIN_MAG_MIP_LINEAR;
            float MinLOD = FLT_MIN;
            float MaxLOD = FLT_MAX;
            float MipLODBias = 0;
            u32 MaxAnisotropy = 1;
            glm::vec4 BorderColor = { 0, 0, 0, 0 };
            u32 Slot = 0;
            eComparison Comparison = eComparison::NEVER;
            eAddressMode AddressU = eAddressMode::CLAMP;
            eAddressMode AddressV = eAddressMode::CLAMP;
            eAddressMode AddressW = eAddressMode::CLAMP;
        };

        struct ENGINE_API TextureCubeFile final {
            string Name = "";
            const char* FrontFilepath = nullptr;
            const char* BackFilepath = nullptr;
            const char* RightFilepath = nullptr;
            const char* LeftFilepath = nullptr;
            const char* TopFilepath = nullptr;
            const char* BottomFilepath = nullptr;
        };

        class ENGINE_API TextureManager final
        {

        public:
            // hardware specific values
            static const usize K_TEXTURE_ARRAY_SIZE = 1;
            // channels count table for each texture format
            static unordered_map<Texture::eFormat, int> ChannelTable;
            // bytes per pixel table for each texture format
            static unordered_map<Texture::eFormat, int> BPPTable;

        public:
            static void Init(Context* context);
            static void Free();

            static void InitTexture(Texture& texture);
            static void InitTextureCube(Texture& texture);

            static void BindTexture(Texture& texture);

            static void FreeTexture(Texture& texture);

            static void WriteTexture(Texture& texture);
            static void WriteTexture(Texture& texture, u32 layerIndex);

            static Texture* ReadTextureFile(const char* filepath, const Texture::eFormat& format);
            static Texture* LoadTextureFile(const char* filePath, const Texture::eFormat& format);
            static TextureLayer ReadTextureLayerFile(const char* filepath, const Texture::eFormat& format, usize& width, usize& height, usize& channels);
            static Texture* ReadTextureCubeFile(const TextureCubeFile& cubeFile, const Texture::eFormat& format);
            static Texture* LoadTextureCubeFile(const TextureCubeFile& cubeFile, const Texture::eFormat& format);

            static void WriteTextureFile(const char* filePath, const Texture& image, const Texture::eFileFormat& fileFormat);
            static Texture ResizeTexture(const Texture& input, usize outputWidth, usize outputHeight);
            static void FlipTexture(Texture& texture);

        private:
            static void ResizeTextureU8(const Texture& input, Texture& output);
            static void ResizeTextureFloat(const Texture& input, Texture& output);

        private:
            static Context* s_Context;
            static unordered_map<string, Texture> s_TextureTable;

        };

    }

}