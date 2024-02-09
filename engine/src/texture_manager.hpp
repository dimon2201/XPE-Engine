#pragma once

#include <render_core.hpp>

namespace xpe
{
    namespace render
    {
        enum class eTextureFormat
        {
            R8, R16, R32, R32_TYPELESS,
            RG8, RG16, RG32,
            RGB8, RGB16, RGB32,
            RGBA8, RGBA16, RGBA32,
            SRGBA8,

            DEFAULT = RGBA8,
            HDR = RGBA32
        };

        class ENGINE_API cTextureLayer final
        {

        public:

            eTextureFormat Format;
            s32 Width, Height, Depth = 0;
            void* Pixels = nullptr;

            cTextureLayer() = default;

            cTextureLayer(eTextureFormat format, s32 width, s32 height, void* pixels)
            : Format(format), Width(width), Height(height), Pixels(pixels) {}

            void Free();

            void CopyFrom(const cTextureLayer& other);

            [[nodiscard]] cTextureLayer Clone() const;

            void Resize(const eTextureFormat& format, s32 width, s32 height);

            void ResizeU8(s32 width, s32 height);

            void ResizeFloat(s32 width, s32 height);

            void* ResizePixelsU8(
                    const void* inputPixels, int inputWidth, int inputHeight,
                    int outputWidth, int outputHeight
            );

            void* ResizePixelsFloat(
                    const void* inputPixels, int inputWidth, int inputHeight,
                    int outputWidth, int outputHeight
            );
        };

        enum class eFileFormat
        {
            PNG,
            JPG,
            TGA,
            HDR,
            BMP
        };

        class ENGINE_API cTexture : public sResource
        {

        public:

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

            // channels count table for each texture format
            static const std::unordered_map<eTextureFormat, int> k_ChannelTable;
            // bytes per pixel table for each texture format
            static const std::unordered_map<eTextureFormat, int> k_BppTable;

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
            bool InitializeData = true;
            vector<cTextureLayer> Layers;
            bool EnableMipmapping = false;
            u32 MostDetailedMip = 0;

            cTexture();
            ~cTexture();

            virtual void Init();
            void Free();

            cTextureLayer CreateLayer() const;

            void RemoveLayerAt(u32 index);

            void WindowFrameResized(s32 width, s32 height);

            void Resize(s32 width, s32 height);

            void ResizePixels(s32 width, s32 height);

            void Flip();

            void FlushLayer(u32 index);

            void Flush();

            void SetResizable(bool resizable);
            [[nodiscard]] inline bool IsResizable() const { return m_Resizable; }

            void SetMipmapping(bool enable);

        private:
            void ResizeTextureU8(s32 width, s32 height);
            void ResizeTextureFloat(s32 width, s32 height);

            bool m_Resizable = false;
        };

        class ENGINE_API cAtlas : public cTexture
        {

        public:

            struct ENGINE_API sCell final
            {
                u32 LayerIndex = 0;
                glm::vec2 Position = { 0, 0 };
                glm::vec2 Size = { 0, 0 };
                cTextureLayer* Texture = nullptr;

                sCell() = default;

                sCell(u32 atlasIndex, const glm::vec2& position, const glm::vec2& size, cTextureLayer* texture)
                : LayerIndex(atlasIndex), Position(position), Size(size), Texture(texture) {}
            };

            struct ENGINE_API sLocation final
            {
                u32 LayerIndex = 0;
                glm::vec2 UV[4] = {
                        glm::vec2(0),
                        glm::vec2(0),
                        glm::vec2(0),
                        glm::vec2(0)
                };
            };

            void AddLayer();

            template<typename... Args>
            sLocation AddCell(Args&&... args);

            template<typename... Args>
            void RemoveCell(Args&&... args);
        };

        class ENGINE_API cCircleFilter3D : public cTexture
        {
        public:
            int FilterSize = 0;

            void Init() override final;
        };

        template<typename... Args>
        cAtlas::sLocation cAtlas::AddCell(Args &&... args)
        {
            sCell cell = sCell(std::forward<Args>(args)...);

            if (cell.LayerIndex >= Layers.size())
            {
                LogWarning("Atlas with index={} is not initialized. Initialize atlasLayer before adding this cell!", cell.LayerIndex);
                return {};
            }

            if (cell.Texture == nullptr || cell.Texture->Pixels == nullptr)
            {
                LogWarning("Cell texture or texture pixels are not initialized. Initialize cell texture before adding this cell!");
                return {};
            }

            cTextureLayer& atlasLayer = Layers[cell.LayerIndex];
            cTextureLayer cellTexture = cell.Texture->Clone();
            cellTexture.Resize(Format, cell.Size.x, cell.Size.y);

            s32 cellX = cell.Position.x;
            s32 cellY = cell.Position.y;
            s32 cellWidth = cell.Size.x;
            s32 cellHeight = cell.Size.y;
            u8* atlasPixels = static_cast<u8*>(atlasLayer.Pixels);
            u8* cellPixels = static_cast<u8*>(cellTexture.Pixels);
            Channels = k_ChannelTable.at(Format);
            int bpc = k_BppTable.at(Format) / Channels; // bytes per channel

            for (int y = 0; y < cellHeight; ++y)
            {
                for (int x = 0; x < cellWidth; ++x)
                {
                    int cellId = (y * cellWidth + x) * Channels;
                    int atlasX = x + cellX;
                    int atlasY = y + cellY;
                    int atlasId = (atlasY * atlasLayer.Width + atlasX) * Channels;
                    for (int c = 0; c < Channels; ++c)
                    {
                        atlasPixels[atlasId + c * bpc] = cellPixels[cellId + c * bpc];
                    }
                }
            }

            cellTexture.Free();

            sLocation location;
            location.LayerIndex = cell.LayerIndex;
            location.UV[0] = glm::fvec2(cellX, cellY) / glm::fvec2(atlasLayer.Width, atlasLayer.Height);
            location.UV[1] = glm::fvec2(cellX, cellY + cellHeight) / glm::fvec2(atlasLayer.Width, atlasLayer.Height);
            location.UV[2] = glm::fvec2(cellX + cellWidth, cellY + cellHeight) / glm::fvec2(atlasLayer.Width, atlasLayer.Height);
            location.UV[3] = glm::fvec2(cellX + cellWidth, cellY) / glm::fvec2(atlasLayer.Width, atlasLayer.Height);
            return location;
        }

        template<typename... Args>
        void cAtlas::RemoveCell(Args &&... args)
        {
            sCell cell = sCell(std::forward<Args>(args)...);

            if (cell.LayerIndex >= Layers.size())
            {
                LogWarning("Atlas with index={} is not initialized. Initialize atlasLayer before adding this cell!", cell.LayerIndex);
                return;
            }

            cTextureLayer& atlasLayer = Layers[cell.LayerIndex];

            s32 cellX = cell.Position.x;
            s32 cellY = cell.Position.y;
            s32 cellWidth = cell.Size.x;
            s32 cellHeight = cell.Size.y;
            u8* atlasPixels = static_cast<u8*>(atlasLayer.Pixels);
            Channels = k_ChannelTable.at(Format);
            int bpc = k_BppTable.at(Format) / Channels; // bytes per channel

            for (int y = 0; y < cellHeight; ++y)
            {
                for (int x = 0; x < cellWidth; ++x)
                {
                    int atlasX = x + cellX;
                    int atlasY = y + cellY;
                    int atlasId = (atlasY * atlasLayer.Width + atlasX) * Channels;
                    for (int c = 0; c < Channels; ++c)
                    {
                        atlasPixels[atlasId + c * bpc] = 0;
                    }
                }
            }
        }

        class ENGINE_API cSampler : public cObject
        {

        public:

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

            void* Instance = nullptr;

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

            ~cSampler();

            void Init();
        };

        struct ENGINE_API sTextureCubeFilepath final
        {
            string FrontFilepath;
            string BackFilepath;
            string RightFilepath;
            string LeftFilepath;
            string TopFilepath;
            string BottomFilepath;
        };

        class ENGINE_API MTexture final
        {

        public:
            static void Init();
            static void Free();
            static cTexture* Load(const string& name, const char* filepath, const eTextureFormat& format);
            static cTexture* LoadCubemap(const string& name, const sTextureCubeFilepath& cubeFilepath, const eTextureFormat& format);
            static cTextureLayer* LoadLayer(
                    const char* filepath,
                    const eTextureFormat& format,
                    int& width, int& height, int& channels
            );
            static bool SaveLayer(const char* filepath, cTextureLayer* textureLayer, const eFileFormat& fileFormat);
        };
    }
}