#pragma once

namespace xpe {

    namespace render {

        using namespace core;

        class ENGINE_API cResource : public cObject
        {

        public:
            enum class eType
            {
                BUFFER,
                TEXTURE,
                SAMPLER
            };

            enum class eViewType
            {
                SRV = 0,
                UAV = 1,
                SRV_UAV = 2,
                DEPTH_STENCIL = 3,
                RENDER_TARGET = 4,
                NONE = 5
            };

            enum class eMapType
            {
                READ,
                WRITE,
                READ_WRITE,
                WRITE_NO_OVERWRITE,
                WRITE_DISCARD
            };

            inline void* GetInstance() const { return m_Instance; }
            inline void* GetSRVInstance() const { return m_ViewInstance[0]; }
            inline void* GetUAVInstance() const { return m_ViewInstance[1]; }
            inline void** GetInstancePtr() { return &m_Instance; }
            inline void** GetSRVInstancePtr() { return &m_ViewInstance[0]; }
            inline void** GetUAVInstancePtr() { return &m_ViewInstance[1]; }
            inline eViewType GetViewType() const { return m_ViewType; }

        protected:
            void* m_Instance = nullptr;
            void* m_ViewInstance[2] = { nullptr };
            eViewType m_ViewType = eViewType::NONE;

        };

        enum class ePrimitiveTopology
        {
            TRIANGLE_LIST,
            TRIANGLE_STRIP,
            POINT_LIST,
            LINE_LIST,
            LINE_STRIP,

            DEFAULT = TRIANGLE_LIST
        };

        class ENGINE_API cBuffer : public cResource
        {

        public:
            enum class eType
            {
                NONE,
                VERTEX,
                INDEX,
                CONSTANT,
                STRUCTURED,
                RW
            };

            enum class eUsage
            {
                DEFAULT,
                STATIC,
                DYNAMIC,
                STAGING
            };

            inline eType GetType() const { return m_Type; }
            inline eViewType GetViewType() const { return m_ViewType; }
            inline eUsage GetUsage() const { return m_Usage; }
            inline u32 GetSlot() const { return m_Slot; }
            inline usize GetStructureSize() const { return m_StructureSize; }
            inline u32 GetNumElements() const { return m_NumElements; }
            inline void* GetInitialData() const { return m_InitialData; }
            inline usize GetByteSize() const { return m_NumElements * m_StructureSize; }
            inline void SetSlot(u32 slot) { m_Slot = slot; }

        protected:
            eType m_Type = eType::NONE;
            eUsage m_Usage = eUsage::DYNAMIC;
            usize m_StructureSize = 0;
            u32 m_NumElements = 0;
            u32 m_Slot = 0;
            void* m_InitialData = nullptr;

        };

        struct ENGINE_API sBlob : public cObject
        {
            void* Instance = nullptr;
            void* ByteCode = nullptr;
            usize ByteCodeSize = 0;
        };

        struct ENGINE_API sViewport : public cObject
        {
            sViewport(const glm::vec4& rect, const glm::vec2& depth)
                : Left(rect.x), Top(rect.y), Width(rect.z), Height(rect.w), MinDepth(depth.x), MaxDepth(depth.y)
            {}

            float Left = 0;
            float Top = 0;
            float Width = 0;
            float Height = 0;
            float MinDepth = 0;
            float MaxDepth = 1;
        };

        // defined statically in sVertex classes for describing "format" of single vertex of geometry
        struct ENGINE_API sVertexFormat final
        {

            struct ENGINE_API sAttribute final
            {

                enum class eType
                {

                    FLOAT,          // 4 bytes
                    BOOL,           // 4 bytes
                    INT,            // 4 bytes
                    UINT,           // 4 bytes

                    VEC2_FLOAT,     // 8 bytes
                    VEC2_UINT,      // 8 bytes
                    VEC2_INT,       // 8 bytes
                    VEC2_TYPELESS,  // 8 bytes

                    VEC3_FLOAT,     // 12 bytes
                    VEC3_UINT,      // 12 bytes
                    VEC3_INT,       // 12 bytes
                    VEC3_TYPELESS,  // 12 bytes

                    VEC4_FLOAT,     // 16 bytes
                    VEC4_UINT,      // 16 bytes
                    VEC4_INT,       // 16 bytes
                    VEC4_TYPELESS,  // 16 bytes

                    MAT2X2,         // 16 bytes
                    MAT3X3,         // 36 bytes
                    MAT4X4,         // 64 bytes

                };

                u32 Location = 0;
                const char* Name = nullptr;
                eType Format;
                u8 ByteSize = 0;

            };

            std::vector<sAttribute> Attributes; // used std::vector, because sVertexFormat is always defined as static var in sVertex class
            usize Stride = 0;

            sVertexFormat() = default;
            sVertexFormat(const std::vector<sAttribute>& attributes);

        };

        // define here any custom attributes that you want to use in your vertex shader
        struct ENGINE_API sAttributes final
        {
            static constexpr sVertexFormat::sAttribute k_Pos = { 0, "XPE_POSITION", sVertexFormat::sAttribute::eType::VEC3_FLOAT, 12 };
            static constexpr sVertexFormat::sAttribute k_UV = { 1, "XPE_UV", sVertexFormat::sAttribute::eType::VEC2_FLOAT, 8 };
            static constexpr sVertexFormat::sAttribute k_Normal = { 2, "XPE_NORMAL", sVertexFormat::sAttribute::eType::VEC3_FLOAT, 12 };
            static constexpr sVertexFormat::sAttribute k_Tangent = { 3, "XPE_TANGENT", sVertexFormat::sAttribute::eType::VEC3_FLOAT, 12 };
            static constexpr sVertexFormat::sAttribute k_BoneIds = { 4, "XPE_BONE_IDS", sVertexFormat::sAttribute::eType::VEC4_INT, 16 };
            static constexpr sVertexFormat::sAttribute k_BoneWeights = { 5, "XPE_BONE_WEIGHTS", sVertexFormat::sAttribute::eType::VEC4_FLOAT, 16 };
        };

        struct ENGINE_API sVertex final
        {
            static const sVertexFormat k_Format;

            glm::vec3 Position = { 0, 0, 0 };
            glm::vec2 UV = { 0, 0 };
            glm::vec3 Normal = { 0, 0, 0 };
            glm::vec3 Tangent = { 0, 0, 0 };
            glm::ivec4 BoneIDs = { -1, -1, -1, -1 };
            glm::vec4 BoneWeights = { 0, 0, 0, 0 };
        };

        JSON(
                sVertex,
                Position,
                UV,
                Normal,
                Tangent,
                BoneIDs,
                BoneWeights
        )

        struct ENGINE_API sInputLayout final
        {
            void* Instance;
            sBlob* VertexBlob = nullptr;
            sVertexFormat Format;

            sInputLayout() = default;
            sInputLayout(const sVertexFormat& format) : Format(format) {}
        };

        enum class eDepthWriteMask {
            ZERO = 0,
            ALL = 1
        };

        enum class eDepthStencilFunc {
            NEVER = 1,
            LESS = 2,
            EQUAL = 3,
            LESS_EQUAL = 4,
            GREATER = 5,
            NOT_EQUAL = 6,
            GREATER_EQUAL = 7,
            ALWAYS = 8,
        };

        enum class eDepthStencilOp {
            KEEP = 1,
            ZERO = 2,
            REPLACE = 3,
            INCR_SAT = 4,
            DECR_SAT = 5,
            INVERT = 6,
            INCR = 7,
            DECR = 8
        };

        struct ENGINE_API sStencilTest : public cObject
        {
            eDepthStencilOp StencilFailOp = eDepthStencilOp::KEEP;
            eDepthStencilOp StencilDepthFailOp = eDepthStencilOp::KEEP;
            eDepthStencilOp StencilPassOp = eDepthStencilOp::KEEP;
            eDepthStencilFunc StencilFunc = eDepthStencilFunc::ALWAYS;
        };

        struct ENGINE_API sDepthStencilMode : public cObject
        {
            void* State = nullptr;
            bool EnableDepth = true;
            eDepthWriteMask DepthWriteMask = eDepthWriteMask::ALL;
            eDepthStencilFunc DepthFunc = eDepthStencilFunc::LESS;
            bool EnableStencil = false;
            u8 StencilReadMask = 0xff;
            u8 StencilWriteMask = 0xff;
            sStencilTest FrontFace;
            sStencilTest BackFace;
        };

        enum class eBlend {
            ZERO = 1,
            ONE	= 2,
            SRC_COLOR = 3,
            INV_SRC_COLOR = 4,
            SRC_ALPHA = 5,
            INV_SRC_ALPHA = 6,
            DEST_ALPHA = 7,
            INV_DEST_ALPHA = 8,
            DEST_COLOR = 9,
            INV_DEST_COLOR = 10,
            SRC_ALPHA_SAT = 11,
            BLEND_FACTOR = 14,
            INV_BLEND_FACTOR = 15,
            SRC1_COLOR = 16,
            INV_SRC1_COLOR = 17,
            SRC1_ALPHA = 18,
            INV_SRC1_ALPHA = 19
        };

        enum class eBlendOp {
            ADD	= 1,
            SUBTRACT = 2,
            REV_SUBTRACT = 3,
            MIN	= 4,
            MAX	= 5
        };

        enum eColorWriteEnableMask : u8 {
            RED	= 1,
            GREEN = 2,
            BLUE = 4,
            ALPHA = 8,
            ALL = (((RED | GREEN) | BLUE) | ALPHA)
        };

        struct ENGINE_API sBlendTarget : public cObject
        {
            bool Enable = false;
            eBlend Src = eBlend::ONE;
            eBlend Dest = eBlend::ZERO;
            eBlendOp BlendOp = eBlendOp::ADD;
            eBlend SrcAlpha = eBlend::ONE;
            eBlend DestAlpha = eBlend::ZERO;
            eBlendOp BlendOpAlpha = eBlendOp::ADD;
            u8 RenderTargetWriteMask = eColorWriteEnableMask::ALL;
        };

        struct ENGINE_API sBlendMode : public cObject
        {
            void* State = nullptr;
            bool AlphaToCoverageEnable = false;
            bool IndependentBlendEnable = false;
            // blend mode can only have maximum 8 targets per passes
            vector<sBlendTarget> Targets;

            sBlendMode() = default;
            sBlendMode(const vector<sBlendTarget>& targets);
            ~sBlendMode();

            void Bind();

        private:
            void Init();
        };

        enum class eFillMode {
            WIREFRAME,
            SOLID,

            DEFAULT = SOLID
        };

        enum class eCullMode {
            NONE,
            FRONT,
            BACK,

            DEFAULT = NONE,
        };

        struct ENGINE_API sRasterizer : public cObject
        {
            void*     State = nullptr;
            eFillMode FillMode = eFillMode::DEFAULT;
            eCullMode CullMode = eCullMode::DEFAULT;
            bool      FrontCounterClockwise = false;
            int       DepthBias = 0;
            float     DepthBiasClamp = 0;
            float     SlopeScaledDepthBias = 0;
            bool      DepthClipEnable = true;
            bool      ScissorEnable = false;
            bool      MultisampleEnable = false;
            bool      AntialiasedLineEnable = false;
        };

        enum class eTextureFormat
        {
            R8, R16, R32, R32_TYPELESS, R16_TYPELESS,
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

        class ENGINE_API cTexture : public cResource
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

            cTexture(
                eType type,
                eViewType viewType,
                eUsage usage,
                const glm::vec3& size,
                usize channelCount,
                eTextureFormat format,
                usize sampleCount,
                dual enableRenderTarget,
                u32 slot,
                u32 mostDetailedMip,
                dual initializeData,
                const vector<sTextureLayer>& layers
            );
            ~cTexture();

            //void Init();
            //void Free();

            sTextureLayer CreateLayer() const;
            void RemoveLayerAt(u32 index);
            u32 GetMipLevels() const;
            static u32 GetMipsLevels(s32 width);
            void WindowFrameResized(s32 width, s32 height);
            void Resize(s32 width, s32 height);
            void ResizePixels(s32 width, s32 height);
            void Flip();
            void GenerateMips();
            void FlushLayer(u32 index);
            void Flush();
            void SetResizable(bool resizable);

            inline bool IsResizable() const { return m_Resizable; }
            inline eType GetType() const { return m_Type; }
            inline eUsage GetUsage() const { return m_Usage; }
            inline s32 GetWidth() const { return m_Width; }
            inline s32 GetHeight() const { return m_Height; }
            inline s32 GetDepth() const { return m_Depth; }
            inline s32 GetChannelCount() const { return m_Channels; }
            inline s32& GetWidthRef() { return m_Width; }
            inline s32& GetHeightRef() { return m_Height; }
            inline s32& GetDepthRef() { return m_Depth; }
            inline s32& GetChannelCountRef() { return m_Channels; }
            inline eTextureFormat GetFormat() const { return m_Format; }
            inline u32 GetSampleCount() const { return m_SampleCount; }
            inline dual IsRenderTargetEnabled() const { return m_EnableRenderTarget; }
            inline u32 GetSlot() const { return m_Slot; }
            inline u32 GetMostDetailedMip() const { return m_MostDetailedMip; }
            inline dual IsDataInitialized() const { return m_InitializeData; }
            inline vector<sTextureLayer>& GetLayers() { return m_Layers; }

        protected:
            void ResizeTextureU8(s32 width, s32 height);
            void ResizeTextureFloat(s32 width, s32 height);

            bool m_Resizable = false;
            eType m_Type = eType::TEXTURE_DEFAULT;
            eUsage m_Usage = eUsage::DEFAULT;
            s32 m_Width;
            s32 m_Height;
            s32 m_Depth = 1;
            s32 m_Channels;
            eTextureFormat m_Format;
            u32 m_SampleCount = 1;
            dual m_EnableRenderTarget = false;
            u32 m_Slot = 0;
            u32 m_MostDetailedMip = 0;
            dual m_InitializeData = true;
            vector<sTextureLayer> m_Layers;

        public:
            // channels count table for each texture format
            static const std::unordered_map<eTextureFormat, int> k_ChannelTable;
            // bytes per pixel table for each texture format
            static const std::unordered_map<eTextureFormat, int> k_BppTable;
        };

        struct ENGINE_API sAtlas2DTexture
        {
            sAtlas2DTexture() = default;
            sAtlas2DTexture(const glm::vec4& offsets) : Offsets(offsets) {}

            glm::vec4 Offsets;
        };

        struct ENGINE_API cAtlas2D : public cTexture
        {

        public:
            cAtlas2D(
                cTexture::eType type,
                cTexture::eUsage usage,
                const glm::vec2& size,
                usize channelCount,
                eTextureFormat format,
                usize sampleCount,
                dual enableRenderTarget,
                u32 slot,
                u32 mostDetailedMip,
                dual initializeData
            ) : cTexture(
                type,
                cResource::eViewType::SRV,
                usage,
                glm::vec3(size.x, size.y, 1.0f),
                channelCount,
                format,
                sampleCount,
                enableRenderTarget,
                slot,
                mostDetailedMip,
                initializeData,
                {}
            ) {}
            ~cAtlas2D() {}

            sAtlas2DTexture AddTexture(const glm::vec2& size);
            void RemoveTexture(const sAtlas2DTexture& texture);

        private:
            vector<sAtlas2DTexture> m_Textures;

        };

        struct ENGINE_API sAtlas : public cTexture
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

            if (cell.LayerIndex >= m_Layers.size()) {
                LogWarning("Atlas with index={} is not initialized. Initialize atlasLayer before adding this cell!", cell.LayerIndex);
                return {};
            }

            if (cell.Texture == nullptr || cell.Texture->Pixels == nullptr) {
                LogWarning("Cell texture or texture pixels are not initialized. Initialize cell texture before adding this cell!");
                return {};
            }

            sTextureLayer& atlasLayer = m_Layers[cell.LayerIndex];
            sTextureLayer cellTexture = cell.Texture->Clone();
            cellTexture.Resize(m_Format, cell.Size.x, cell.Size.y);

            s32 cellX = cell.Position.x;
            s32 cellY = cell.Position.y;
            s32 cellWidth = cell.Size.x;
            s32 cellHeight = cell.Size.y;
            u8* atlasPixels = static_cast<u8*>(atlasLayer.Pixels);
            u8* cellPixels = static_cast<u8*>(cellTexture.Pixels);
            m_Channels = k_ChannelTable.at(m_Format);
            int bpc = k_BppTable.at(m_Format) / m_Channels; // bytes per channel

            for (int y = 0; y < cellHeight; ++y) {
                for (int x = 0; x < cellWidth; ++x) {
                    int cellId = (y * cellWidth + x) * m_Channels;
                    int atlasX = x + cellX;
                    int atlasY = y + cellY;
                    int atlasId = (atlasY * atlasLayer.Width + atlasX) * m_Channels;
                    for (int c = 0; c < m_Channels; ++c) {
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

        struct ENGINE_API sSampler : public cResource
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

            sSampler();
            ~sSampler();

            void Init();
        };

    }

}