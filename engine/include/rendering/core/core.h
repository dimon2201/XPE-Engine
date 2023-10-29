#pragma once

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API GPUResource : public Object
        {
            void* Instance = nullptr;
            void* ViewInstance = nullptr;
        };

        enum class eMapType
        {
            READ,
            WRITE,
            READ_WRITE,
            WRITE_NO_OVERWRITE,
            WRITE_DISCARD
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
            DEFAULT,
            STATIC,
            DYNAMIC,
            STAGING
        };

        struct ENGINE_API Buffer : public GPUResource
        {
            eBufferType Type;
            eBufferUsage Usage = eBufferUsage::DYNAMIC;
            usize StructureSize = 0;
            u32 NumElements = 0;
            u32 Slot = 0;
            void* InitialData = nullptr;

            [[nodiscard]] inline usize GetByteSize() const
            {
                return NumElements * StructureSize;
            }
        };

        struct ENGINE_API Blob : public Object
        {
            void* Instance = nullptr;
            void* ByteCode = nullptr;
            usize ByteCodeSize = 0;
        };

    }

}