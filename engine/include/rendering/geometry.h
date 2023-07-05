#pragma once

namespace xpe {

    namespace render {

        struct ENGINE_API GeometryInfo {

            usize VertexBufferOffset;
            usize IndexBufferOffset;
            usize VertexCount;
            usize IndexCount;

            GeometryInfo(usize vertexBufferOffset, usize indexBufferOffset, usize vertexCount, usize indexCount)
            : VertexBufferOffset(vertexBufferOffset), IndexBufferOffset(indexBufferOffset), VertexCount(vertexCount), IndexCount(indexCount) {}

        };

    }

}