#pragma once

#include <geometry/mesh_geometry.h>

#include <rendering/buffers/vertex_buffer.h>
#include <rendering/buffers/index_buffer.h>
#include <rendering/buffers/instance_buffer.h>

namespace xpe {

    namespace render {

        using namespace xpe::core;
        using namespace xpe::math;

        class Context;

        struct ENGINE_API BatchTextGlyphIndexed final
        {
            GeometryIndexedFormat Format;
            VertexBuffer<Vertex3D> Vertices;
            IndexBuffer Indices;
            TextGlyphInstanceBuffer Instances;
        };

        class ENGINE_API TextBatchManager final
        {

        public:
            ~TextBatchManager();

            bool StoreGeometryIndexed(const string& str, const GeometryIndexed<Vertex3D>& geometry, usize instanceCount = 0);

            bool HasGeometryIndexed(const string& str);

            void BeginBatch(const string& str);
            void BeginBatch(BatchTextGlyphIndexed& batchIndexed);

            u32 AddInstance(const string& str, const TextGlyphInstance& instance);

            void RemoveInstanceAt(const string& str, u32 index);

            void ClearInstances(const string& str);

            void FlushInstances(const string& str);

            void ResizeInstances(const string& str, const usize count);

            void ReserveInstances(const string& str, const usize count);

            usize GetInstanceCapacity(const string& str);

            void DrawBatch(const string& str);
            void DrawBatch(BatchTextGlyphIndexed& batchIndexed);

            void DrawAll();

        private:

            void InitBatchIndexed(BatchTextGlyphIndexed& batchIndexed, const GeometryIndexedFormat& format, usize instanceCount);

            void FreeBatchIndexed(BatchTextGlyphIndexed& batchIndexed);

            BatchTextGlyphIndexed NewBatchIndexed(const GeometryIndexedFormat& format, usize instanceCount);

        private:
            vector<BatchTextGlyphIndexed> m_BatchesIndexed;
        };

    }

}