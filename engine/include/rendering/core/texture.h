#pragma once

#include <rendering/core/core.h>

namespace xpe {

    namespace render {

        using namespace core;

        class Context;

        struct Texture;

        struct ENGINE_API TextureComponent : public Component {

            TextureComponent(const string& usid) : Component(usid) {}

            Texture* Texture = nullptr;

        };

        struct ENGINE_API Mip final {
            void* Pixels = nullptr;
            u32 RowByteSize = 0;

            Mip() = default;
            Mip(void* pixels, u32 rowByteSize) : Pixels(pixels), RowByteSize(rowByteSize) {}
        };

        struct ENGINE_API TextureLayer final {
            void* Pixels = nullptr;
            u32 RowByteSize = 0;
            core::Boolean FromFile = K_FALSE;
            vector<Mip> Mips;
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
                R8, R16, R32, R32_TYPELESS,
                RG8, RG16, RG32,
                RGB8, RGB16, RGB32,
                RGBA8, RGBA16, RGBA32
            };

            eType Type = eType::TEXTURE_DEFAULT;
            eUsage Usage = eUsage::DEFAULT;

            int Width;
            int Height;
            int Depth = 1;
            int Channels;
            eFormat Format;

            bool EnableRenderTarget = false;

            u32 Slot = 0;

            u32 MostDetailedMip = 0;

            bool InitializeData = true;
            bool GenerateMips = false;
            vector<TextureLayer> Layers;

            inline u32 GetMipLevels() const {
                if (Layers.empty()) {
                    return 1;
                }

                auto& mips = Layers.front().Mips;
                return mips.empty() ? 1 : mips.size();
            }
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

            float MinLOD = 0;
            float MaxLOD = FLT_MAX;
            float MipLODBias = 0;

            u32 AnisotropyLevel = 1;

            glm::vec4 BorderColor = { 0, 0, 0, 0 };

            eComparison Comparison = eComparison::NEVER;

            eAddress AddressU = eAddress::CLAMP;
            eAddress AddressV = eAddress::CLAMP;
            eAddress AddressW = eAddress::CLAMP;
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

        struct ENGINE_API TextureStorage : public Object {
            unordered_map<string, Texture> Table;

            TextureStorage() = default;
            ~TextureStorage();
        };

        class ENGINE_API TextureManager final
        {

        public:
            // channels count table for each texture format
            static std::unordered_map<Texture::eFormat, int> ChannelTable;
            // bytes per pixel table for each texture format
            static std::unordered_map<Texture::eFormat, int> BPPTable;

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
            static TextureLayer ReadTextureLayerFile(
                    const char* filepath,
                    const Texture::eFormat& format,
                    int& width, int& height, int& channels
            );
            static Texture* ReadTextureCubeFile(const TextureCubeFile& cubeFile, const Texture::eFormat& format);
            static Texture* LoadTextureCubeFile(const TextureCubeFile& cubeFile, const Texture::eFormat& format);

            static void WriteTextureFile(const char* filePath, const Texture& image, const Texture::eFileFormat& fileFormat);
            static Texture ResizeTexture(const Texture& input, int outputWidth, int outputHeight);
            static void FlipTexture(Texture& texture);

            static u32 GetMipLevels(usize width);
            static void GenerateMips(Texture& texture);
            static void GenerateMips(TextureLayer& textureLayer, int width, int height, const Texture::eFormat& format);
            static void GenerateMipsU8(TextureLayer& textureLayer, int width, int height, int bpp, int channels);
            static void GenerateMipsFloat(TextureLayer& textureLayer, int width, int height, int bpp, int channels);

            static void FreeMips(TextureLayer& textureLayer);

        private:
            static void ResizeTextureU8(const Texture& input, Texture& output);
            static void ResizeTextureFloat(const Texture& input, Texture& output);

            static void ResizeTextureLayerU8(
                    const TextureLayer& input, int inputWidth, int inputHeight, int channels,
                    TextureLayer& output, int outputWidth, int outputHeight
            );
            static void ResizeTextureLayerFloat(
                    const TextureLayer& input, int inputWidth, int inputHeight, int channels,
                    TextureLayer& output, int outputWidth, int outputHeight
            );

            static void* ResizePixelsU8(
                    const void* inputPixels, int inputWidth, int inputHeight, int channels,
                    int outputWidth, int outputHeight
            );
            static void* ResizePixelsFloat(
                    const void* inputPixels, int inputWidth, int inputHeight, int channels,
                    int outputWidth, int outputHeight
            );

        private:
            static Context* s_Context;
            static TextureStorage* s_Storage;
        };

    }

}