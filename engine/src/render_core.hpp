#pragma once

namespace xpe
{
    namespace render
    {
        struct ENGINE_API sResource : public cObject
        {
            enum class eViewType
            {
                NONE,
                DEPTH_STENCIL,
                RENDER_TARGET,
                SRV,
                UAV
            };

            enum class eMapType
            {
                READ,
                WRITE,
                READ_WRITE,
                WRITE_NO_OVERWRITE,
                WRITE_DISCARD
            };

            void* Instance = nullptr;
            void* ViewInstance = nullptr;
            eViewType ViewType = eViewType::NONE;
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

        struct ENGINE_API sBuffer : public sResource
        {
            enum class eType
            {
                NONE,
                VERTEX,
                INDEX,
                ITEM,
                LIST,

                CONSTANT = ITEM,
                STRUCTURED = LIST,
            };

            enum class eSubType
            {
                NONE,
                RAW,     // combined with VERTEX/INDEX/LIST buffer
                APPEND,  // combined with LIST/ITEM buffer
                CONSUME  // combined with LIST/ITEM buffer
            };

            enum class eUsage
            {
                DEFAULT,
                STATIC,
                DYNAMIC,
                STAGING
            };

            eType Type = eType::NONE;
            eSubType SubType = eSubType::NONE;
            eUsage Usage = eUsage::DYNAMIC;
            usize StructureSize = 0;
            u32 NumElements = 0;
            u32 Slot = 0;
            void* InitialData = nullptr;

            [[nodiscard]] inline usize GetByteSize() const
            {
                return NumElements * StructureSize;
            }
        };

        struct ENGINE_API sBlob : public cObject
        {
            void* Instance = nullptr;
            void* ByteCode = nullptr;
            usize ByteCodeSize = 0;
        };

        struct ENGINE_API sViewport : public cObject
        {
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

        enum class eDepthWriteMask
        {
            ZERO = 0,
            ALL = 1
        };

        enum class eDepthStencilFunc
        {
            NEVER = 1,
            LESS = 2,
            EQUAL = 3,
            LESS_EQUAL = 4,
            GREATER = 5,
            NOT_EQUAL = 6,
            GREATER_EQUAL = 7,
            ALWAYS = 8,
        };

        enum class eDepthStencilOp
        {
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

        enum class eBlend
        {
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

        enum class eBlendOp
        {
            ADD	= 1,
            SUBTRACT = 2,
            REV_SUBTRACT = 3,
            MIN	= 4,
            MAX	= 5
        };

        enum eColorWriteEnableMask : u8
        {
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

        enum class eFillMode
        {
            WIREFRAME,
            SOLID,

            DEFAULT = SOLID
        };

        enum class eCullMode
        {
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
    }
}