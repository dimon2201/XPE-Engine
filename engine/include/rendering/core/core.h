#pragma once

namespace xpe {

    namespace render {

        using namespace core;

        enum class eMapType
        {
            READ,
            WRITE,
            READ_WRITE,
            WRITE_NO_OVERWRITE,
            WRITE_DISCARD
        };

        struct ENGINE_API GPUResource
        {
            void* Instance = nullptr;
            void* ViewInstance = nullptr;
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

        enum class eBufferType
        {
            VERTEX,
            INDEX,
            CONSTANT,
            STRUCTURED
        };

        enum class eBufferUsage
        {
            STATIC,
            DYNAMIC,

            DEFAULT = STATIC
        };

        struct ENGINE_API Buffer : public GPUResource
        {
            eBufferType Type;
            eBufferUsage Usage;
            usize StructureSize = 0;
            u32 NumElements = 0;
            u32 Slot = 0;

            [[nodiscard]] inline usize ByteSize() const
            {
                return NumElements * StructureSize;
            }
        };

        struct ENGINE_API Blob final
        {
            void* Instance = nullptr;
            void* ByteCode = nullptr;
            usize ByteCodeSize = 0;
        };

    }

}