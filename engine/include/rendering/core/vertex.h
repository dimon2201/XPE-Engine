#pragma once

namespace xpe {

    namespace render {

        using namespace core;

        // defined statically in Vertex classes for describing "format" of single vertex of geometry
        struct ENGINE_API VertexFormat final
        {

            struct ENGINE_API Attribute final
            {

                enum class eFormat : u32
                {

                    FLOAT = 4,      // 4 bytes
                    BOOL = 4,       // 4 bytes
                    INT = 4,        // 4 bytes

                    VEC2 = 8,       // 8 bytes
                    VEC3 = 12,      // 12 bytes
                    VEC4 = 16,      // 16 bytes

                    MAT2X2 = 16,    // 16 bytes
                    MAT3X3 = 36,    // 36 bytes
                    MAT4X4 = 64,    // 64 bytes

                };

                u32 Location = 0;
                const char* Name = nullptr;
                eFormat Format;

            };

            std::vector<Attribute> Attributes; // used std::vector, because VertexFormat is always defined as static var in Vertex class
            usize Stride = 0;

            VertexFormat() = default;
            VertexFormat(const std::vector<Attribute>& attributes);

        };

        // define here any custom attributes that you want to use in your vertex shader
        struct ENGINE_API Attributes final
        {
            static constexpr VertexFormat::Attribute POS2 = { 0, "XPE_POSITION_2D", VertexFormat::Attribute::eFormat::VEC2 };
            static constexpr VertexFormat::Attribute POS3 = { 0, "XPE_POSITION", VertexFormat::Attribute::eFormat::VEC3 };
            static constexpr VertexFormat::Attribute UV = { 1, "XPE_UV", VertexFormat::Attribute::eFormat::VEC2 };
            static constexpr VertexFormat::Attribute NORMAL = { 2, "XPE_NORMAL", VertexFormat::Attribute::eFormat::VEC3 };
            static constexpr VertexFormat::Attribute TANGENT = { 3, "XPE_TANGENT", VertexFormat::Attribute::eFormat::VEC3 };
            static constexpr VertexFormat::Attribute BITANGENT = { 4, "XPE_BITANGENT", VertexFormat::Attribute::eFormat::VEC3 };
            static constexpr VertexFormat::Attribute BONE_ID = { 5, "XPE_BONE_ID", VertexFormat::Attribute::eFormat::INT };
            static constexpr VertexFormat::Attribute BONE_WEIGHT = {6, "XPE_BONE_WEIGHT", VertexFormat::Attribute::eFormat::FLOAT };
        };

    }

}