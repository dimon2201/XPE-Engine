#pragma once

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API GPUResource {
            void* Instance = nullptr;
            void* ViewInstance = nullptr;
        };

        enum ePrimitiveTopology {
            TRIANGLE_LIST = 0,
            TRIANGLE_STRIP = 1
        };

        enum class eBufferType {
            VERTEX = 0,
            INDEX = 1,
            CONSTANT = 2,
            STRUCTURED = 3
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