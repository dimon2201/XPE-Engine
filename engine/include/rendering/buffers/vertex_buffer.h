#pragma once

#include <rendering/core/context.hpp>
#include <rendering/core/vertex.h>

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API sVertexBuffer : public sBuffer
        {
            vector<sVertex> List;

            sVertexBuffer(const usize vertexCount = 0, sBuffer::eSubType subtype = sBuffer::eSubType::NONE, sBuffer::eViewType viewtype = sBuffer::eViewType::NONE);
            sVertexBuffer(const vector<sVertex>& vertices, sBuffer::eSubType subtype = sBuffer::eSubType::NONE, sBuffer::eViewType viewtype = sBuffer::eViewType::NONE);
            ~sVertexBuffer();

            void Flush();
            void FlushVertex(u32 index, const sVertex& item);
            void FlushVertices(const vector<sVertex>& vertices);

            void Resize(const usize count);
            void Reserve(const usize count);
            void Recreate(const usize count);

            void Clear();

            // returns vertex offset of added vertices
            usize AddVertices(const vector<sVertex>& vertices);

            inline sVertex* GetVertex(const u32 index)
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