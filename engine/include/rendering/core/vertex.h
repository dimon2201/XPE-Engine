#pragma once

namespace xpe {

    namespace render {

        using namespace core;

        // defined statically in Vertex classes for describing "format" of single vertex of geometry
        struct ENGINE_API VertexFormat final
        {

            struct ENGINE_API Attribute final
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

            std::vector<Attribute> Attributes; // used std::vector, because VertexFormat is always defined as static var in Vertex class
            usize Stride = 0;

            VertexFormat() = default;
            VertexFormat(const std::vector<Attribute>& attributes);

        };

        // define here any custom attributes that you want to use in your vertex shader
        struct ENGINE_API Attributes final
        {
            static constexpr VertexFormat::Attribute POS = { 0, "XPE_POSITION", VertexFormat::Attribute::eType::VEC3_FLOAT, 12 };
            static constexpr VertexFormat::Attribute UV = { 1, "XPE_UV", VertexFormat::Attribute::eType::VEC2_FLOAT, 8 };
            static constexpr VertexFormat::Attribute NORMAL = { 2, "XPE_NORMAL", VertexFormat::Attribute::eType::VEC3_FLOAT, 12 };
            static constexpr VertexFormat::Attribute TANGENT = { 3, "XPE_TANGENT", VertexFormat::Attribute::eType::VEC3_FLOAT, 12 };
            static constexpr VertexFormat::Attribute BONE_IDS = { 4, "XPE_BONE_IDS", VertexFormat::Attribute::eType::VEC4_INT, 16 };
            static constexpr VertexFormat::Attribute BONE_WEIGHTS = { 5, "XPE_BONE_WEIGHTS", VertexFormat::Attribute::eType::VEC4_FLOAT, 16 };
        };

        struct ENGINE_API Vertex final
        {
            static VertexFormat Format;

            glm::vec3 Position = { 0, 0, 0 };
            glm::vec2 UV = { 0, 0 };
            glm::vec3 Normal = { 0, 0, 0 };
            glm::vec3 Tangent = { 0, 0, 0 };
            glm::ivec4 BoneIDs = { -1, -1, -1, -1 };
            glm::vec4 BoneWeights = { 0, 0, 0, 0 };
        };

        Json(
            Vertex,
            Position,
            UV,
            Normal,
            Tangent,
            BoneIDs,
            BoneWeights
        )

    }

}