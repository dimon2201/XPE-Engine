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
            INSTANCE = 2,
            CONSTANT = 3
        };

        struct ENGINE_API Buffer {
            eBufferType Type;
            GPUResource Resource;
            void* CPUMemory;
            usize AppendOffset;
        };

        struct ENGINE_API Blob final {
            void* Instance = nullptr;
            void* ByteCode = nullptr;
            usize ByteCodeSize = 0;
        };

    }

}