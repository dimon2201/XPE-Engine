#pragma once

#include <rendering/core.h>

namespace xpe {

    namespace render {

        struct ENGINE_API InputLayout final {

            struct ENGINE_API Entry final {

                enum class eFormat {
                    VEC2 = 0,
                    VEC3 = 1,
                    VEC4 = 2
                };

                const char* Name;
                eFormat Format;
                usize ByteSize;
            };

            ePrimitiveTopology PrimitiveTopology;
            GPUResource InputLayout;
            Blob* VertexBlob = nullptr;
            usize EntryCount;
            Entry Entries[16];
            usize StrideByteSize;

        };

    }

}