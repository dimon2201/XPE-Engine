#pragma once

#include <rendering/core/core.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace math;

        struct ENGINE_API sIndexBuffer : public sBuffer
        {
            vector<u32> List;

            sIndexBuffer(const usize indexCount = 0, sBuffer::eSubType subtype = sBuffer::eSubType::NONE, sBuffer::eViewType viewtype = sBuffer::eViewType::NONE);
            sIndexBuffer(const vector<u32>& indexArray, sBuffer::eSubType subtype = sBuffer::eSubType::NONE, sBuffer::eViewType viewtype = sBuffer::eViewType::NONE);
            ~sIndexBuffer();

            void Flush();
            void FlushIndices(const vector<u32>& indices);
            void FlushIndex(u32 i, u32 index);

            void Resize(const usize count);
            void Reserve(const usize count);
            void Recreate(const usize count);

            // returns index offset of added indices
            usize AddIndices(const vector<u32>& indices);

            inline u32* GetIndex(const u32 i)
            {
                // check if index is in the size bounds
                // if not, then resize to index + 1
                if (i >= Count()) {
                    Resize(i + 1);
                }
                return &List[i];
            }

            inline usize Count() {
                return List.size();
            }

        };

    }

}