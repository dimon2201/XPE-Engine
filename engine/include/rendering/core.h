#pragma once

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API GPUResource {
            void* Instance = nullptr;
            void* ViewInstance = nullptr;
        };

        enum class ePrimitiveTopology {
            TRIANGLE_LIST,
            TRIANGLE_STRIP,
            POINT_LIST,
            LINE_LIST,
            LINE_STRIP
        };

        enum class eBufferType {
            VERTEX,
            INDEX,
            CONSTANT,
            STRUCTURED
        };

        struct ENGINE_API Buffer : public GPUResource {
            eBufferType Type;
            void* CPUMemory;
            usize AppendOffset;
            usize ByteSize = 0;
            usize StructureSize = 0;
            u32 FirstElement = 0;
            u32 NumElements = 0;
            u32 Slot = 0;
            Boolean Duplicate = K_FALSE;
        };

        struct ENGINE_API Blob final {
            void* Instance = nullptr;
            void* ByteCode = nullptr;
            usize ByteCodeSize = 0;
        };

    }

}