#pragma once

#include <rendering/core/context.hpp>
#include <rendering/core/vertex.h>

namespace xpe {

    namespace render {

        using namespace core;

        class VertexBuffer : public Buffer
        {

        public:
            vector<Vertex> List;

            VertexBuffer() = default;
            VertexBuffer(const usize vertexCount);
            VertexBuffer(const vector<Vertex>& vertices);
            ~VertexBuffer();

        public:
            void Flush();

            void FlushVertex(u32 index, const Vertex& item);

            void FlushVertices(const vector<Vertex>& vertices);

            void Resize(const usize count);

            void Reserve(const usize count);

            void Recreate(const usize count);

            void Clear();

            // returns vertex offset of added vertices
            usize AddVertices(const vector<Vertex>& vertices);

            inline Vertex* GetVertex(const u32 index)
            {
                // check if index is in the size bounds
                // if not, then resize to index + 1
                if (index >= Count()) {
                    Resize(index + 1);
                }
                return &List[index];
            }

            inline usize Count()
            {
                return List.size();
            }

        };

    }

}