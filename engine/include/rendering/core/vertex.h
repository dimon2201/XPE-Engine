#pragma once

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API VertexFormat final {

            struct ENGINE_API Attribute final {

                enum class eFormat : u32 {

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

            std::vector<Attribute> Attributes;
            usize Stride = 0;

            VertexFormat() = default;
            VertexFormat(const std::vector<Attribute>& attributes);

        };

        class ENGINE_API Attributes final {

        public:
            static constexpr VertexFormat::Attribute POS2 = { 0, "XPE_POSITION_2D", VertexFormat::Attribute::eFormat::VEC2 };
            static constexpr VertexFormat::Attribute POS3 = { 0, "XPE_POSITION", VertexFormat::Attribute::eFormat::VEC3 };
            static constexpr VertexFormat::Attribute UV = { 1, "XPE_UV", VertexFormat::Attribute::eFormat::VEC2 };
            static constexpr VertexFormat::Attribute NORMAL = { 2, "XPE_NORMAL", VertexFormat::Attribute::eFormat::VEC3 };
            static constexpr VertexFormat::Attribute TANGENT = { 3, "XPE_TANGENT", VertexFormat::Attribute::eFormat::VEC3 };
            static constexpr VertexFormat::Attribute BITANGENT = { 4, "XPE_BITANGENT", VertexFormat::Attribute::eFormat::VEC3 };
            static constexpr VertexFormat::Attribute BONE_ID = { 5, "XPE_BONE_ID", VertexFormat::Attribute::eFormat::INT };
            static constexpr VertexFormat::Attribute WEIGHT = { 6, "XPE_WEIGHT", VertexFormat::Attribute::eFormat::FLOAT };
        };

        struct ENGINE_API Vertex2D final {
            static VertexFormat Format;

            glm::vec2 Position = { 0, 0 };
            glm::vec2 UV = { 0, 0 };
        };

        struct ENGINE_API Vertex3D final {
            static VertexFormat Format;

            glm::vec3 Position = { 0, 0, 0 };
            glm::vec2 UV = { 0, 0 };
            glm::vec3 Normal = { 0, 0, 0 };
            glm::vec3 Tangent = { 0, 0, 0 };
        };

    }

}