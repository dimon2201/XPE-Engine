#pragma once

#include <rendering/core/core.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace math;

        class Context;

        class IndexBuffer : public Buffer
        {

        public:
            vector<u32> List;

            IndexBuffer() = default;
            IndexBuffer(const usize indexCount);
            IndexBuffer(const vector<u32>& indexArray);
            ~IndexBuffer();

        public:
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