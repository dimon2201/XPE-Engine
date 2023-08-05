#pragma once

#include <geometry/geometry.h>
#include <rendering/core/core.h>

namespace xpe {

    namespace render {

        using namespace core;
        using namespace math;

        class Context;

        class IndexBuffer : public Buffer {

        public:
            IndexBuffer() = default;
            IndexBuffer(const usize indexCount);

        public:
            void Free();

            void Flush();

            void FlushIndices(const IndexArray& indices);

            void Bind();

            void FlushIndex(u32 i, u32 index);

            void Resize(const usize count);

            void Reserve(const usize count);

            void Recreate(const usize count);

            inline u32* GetIndex(const u32 i) {
                // check if index is in the size bounds
                // if not, then resize to index + 1
                if (i >= Count()) {
                    Resize(i + 1);
                }
                return &m_IndexArray[i];
            }

            inline IndexArray& GetIndexArray() {
                return m_IndexArray;
            }

            inline usize Count() {
                return m_IndexArray.Count();
            }

        protected:
            IndexArray m_IndexArray;
        };

    }

}