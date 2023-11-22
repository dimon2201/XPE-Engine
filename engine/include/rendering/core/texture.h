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

            DEFAULT = RGBA8,
            HDR = RGBA32
        };

        struct ENGINE_API sMip final
        {
            eTextureFormat Format;
            s32 Width, Height = 0;
            void* Pixels = nullptr;

            sMip() = default;

            sMip(eTextureFormat format, s32 width, s32 height, void* pixels)
            : Format(format), Width(width), Height(height), Pixels(pixels) {}
        };

        struct ENGINE_API sTextureLayer final
        {
            eTextureFormat Format;
            s32 Width, Height = 0;
            void* Pixels = nullptr;
            vector<sMip> Mips;

            sTextureLayer() = default;

            sTextureLayer(eTextureFormat format, s32 width, s32 height, void* pixels)
            : Format(format), Width(width), Height(height), Pixels(pixels) {}

            void Free();

            void CopyFrom(const sTextureLayer& other);

            [[nodiscard]] sTextureLayer Clone() const;

            void GenerateMips(const eTextureFormat& format, int width, int height);

            void GenerateMipsU8(int width, int height);

            void GenerateMipsFloat(int width, int height);

            void FreeMips();

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

        struct ENGINE_API sTexture : public sResource
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

            u32 MostDetailedMip = 0;

            bool InitializeData = true;
            vector<sTextureLayer> Layers;

            ~sTexture();

            void Init();
            void Free();

            sTextureLayer CreateLayer() const;

            void RemoveLayerAt(u32 index);

            [[nodiscard]] u32 GetMipLevels() const;
            static u32 GetMipsLevels(s32 width);

            void WindowFrameResized(s32 width, s32 height);

            void Resize(s32 width, s32 height);

            void ResizePixels(s32 width, s32 height);

            void Flip();

            void GenerateMips();

            void FlushLayer(u32 index);

            void Flush();

            void SetResizable(bool resizable);
            [[nodiscard]] inline bool IsResizable() const { return m_Resizable; }

        private:
            void ResizeTextureU8(s32 width, s32 height);
            void ResizeTextureFloat(s32 width, s32 height);

            bool m_Resizable = false;
        };

        struct ENGINE_API sAtlas : public sTexture
        {
            struct ENGINE_API sCell final
            {
                u32 LayerIndex = 0;
                glm::vec2 Position = { 0, 0 };
                glm::vec2 Size = { 0, 0 };
                sTextureLayer* Texture = nullptr;

                sCell() = default;

                sCell(u32 atlasIndex, const glm::vec2& position, const glm::vec2& size, sTextureLayer* texture)
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

        template<typename... Args>
        sAtlas::sLocation sAtlas::AddCell(Args &&... args)
        {
            sCell cell = sCell(std::forward<Args>(args)...);

            if (cell.LayerIndex >= Layers.size()) {
                LogWarning("Atlas with index={} is not initialized. Initialize atlasLayer before adding this cell!", cell.LayerIndex);
                return {};
            }

            if (cell.Texture == nullptr || cell.Texture->Pixels == nullptr) {
                LogWarning("Cell texture or texture pixels are not initialized. Initialize cell texture before adding this cell!");
                return {};
            }

            sTextureLayer& atlasLayer = Layers[cell.LayerIndex];
            sTextureLayer cellTexture = cell.Texture->Clone();
            cellTexture.Resize(Format, cell.Size.x, cell.Size.y);

            s32 cellX = cell.Position.x;
            s32 cellY = cell.Position.y;
            s32 cellWidth = cell.Size.x;
            s32 cellHeight = cell.Size.y;
            u8* atlasPixels = static_cast<u8*>(atlasLayer.Pixels);
            u8* cellPixels = static_cast<u8*>(cellTexture.Pixels);
            Channels = k_ChannelTable.at(Format);
            int bpc = k_BppTable.at(Format) / Channels; // bytes per channel

            for (int y = 0; y < cellHeight; ++y) {
                for (int x = 0; x < cellWidth; ++x) {
                    int cellId = (y * cellWidth + x) * Channels;
                    int atlasX = x + cellX;
                    int atlasY = y + cellY;
                    int atlasId = (atlasY * atlasLayer.Width + atlasX) * Channels;
                    for (int c = 0; c < Channels; ++c) {
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
        void sAtlas::RemoveCell(Args &&... args)
        {
            sCell cell = sCell(std::forward<Args>(args)...);

            if (cell.LayerIndex >= Layers.size()) {
                LogWarning("Atlas with index={} is not initialized. Initialize atlasLayer before adding this cell!", cell.LayerIndex);
                return;
            }

            sTextureLayer& atlasLayer = Layers[cell.LayerIndex];

            s32 cellX = cell.Position.x;
            s32 cellY = cell.Position.y;
            s32 cellWidth = cell.Size.x;
            s32 cellHeight = cell.Size.y;
            u8* atlasPixels = static_cast<u8*>(atlasLayer.Pixels);
            Channels = k_ChannelTable.at(Format);
            int bpc = k_BppTable.at(Format) / Channels; // bytes per channel

            for (int y = 0; y < cellHeight; ++y) {
                for (int x = 0; x < cellWidth; ++x) {
                    int atlasX = x + cellX;
                    int atlasY = y + cellY;
                    int atlasId = (atlasY * atlasLayer.Width + atlasX) * Channels;
                    for (int c = 0; c < Channels; ++c) {
                        atlasPixels[atlasId + c * bpc] = 0;
                    }
                }
            }
        }

        struct ENGINE_API sSampler : public sResource
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