#pragma once

#include <geometry/mesh.h>

#include <rendering/buffers/vertex_buffer.h>
#include <rendering/buffers/index_buffer.h>
#include <rendering/buffers/instance_buffer.h>

namespace xpe {

    namespace render {

        using namespace xpe::core;
        using namespace xpe::math;

        class Context;

        struct ENGINE_API BatchIndexed final
        {
            GeometryIndexedFormat Format;
            VertexBuffer<Vertex3D> Vertices;
            IndexBuffer Indices;
            InstanceBuffer Instances;
        };

        struct ENGINE_API BatchIndexed2d final
        {
            GeometryIndexedFormat Format;
            VertexBuffer<Vertex2D> Vertices;
            IndexBuffer Indices;
            InstanceBuffer2d Instances;
        };

        struct ENGINE_API BatchVertexed final
        {
            GeometryVertexedFormat Format;
            VertexBuffer<Vertex3D> Vertices;
            InstanceBuffer Instances;
        };

        struct ENGINE_API BatchVertexed2d final
        {
            GeometryVertexedFormat Format;
            VertexBuffer<Vertex2D> Vertices;
            InstanceBuffer2d Instances;
        };

        class ENGINE_API BatchManager final
        {

        public:
            BatchManager(Context* context);
            ~BatchManager();

            void StoreGeometryIndexed(const string& str, const GeometryIndexed<Vertex3D>& geometry, usize instanceCount = 0);
            void StoreGeometryVertexed(const string& str, const GeometryVertexed<Vertex3D>& geometry, usize instanceCount = 0);

            void BeginBatch(const string& str);
            void BeginBatch(BatchVertexed& batchVertexed);
            void BeginBatch(BatchIndexed& batchIndexed);

            u32 AddInstance(const string& str, const RenderInstance& instance);

            void RemoveInstanceAt(const string& str, u32 index);

            void ClearInstances(const string& str);

            void FlushInstances(const string& str);

            void ResizeInstances(const string& str, const usize count);

            void ReserveInstances(const string& str, const usize count);

            void DrawBatch(const string& str);
            void DrawBatch(BatchVertexed& batchVertexed);
            void DrawBatch(BatchIndexed& batchIndexed);

            void DrawAll();

        private:

            void InitBatchIndexed(BatchIndexed& batchIndexed, const GeometryIndexedFormat& format, usize instanceCount);
            void InitBatchVertexed(BatchVertexed& batchVertexed, const GeometryVertexedFormat& format, usize instanceCount);

            void FreeBatchIndexed(BatchIndexed& batchIndexed);
            void FreeBatchVertexed(BatchVertexed& batchVertexed);

            BatchIndexed NewBatchIndexed(const GeometryIndexedFormat& format, usize instanceCount);
            BatchVertexed NewBatchVertexed(const GeometryVertexedFormat& format, usize instanceCount);

        private:
            Context* m_Context;
            vector<BatchIndexed> m_BatchesIndexed;
            vector<BatchVertexed> m_BatchesVertexed;
        };

        class ENGINE_API BatchManager2d final
        {

        public:
            BatchManager2d(Context* context);
            ~BatchManager2d();

            void StoreGeometryIndexed(const string& str, const GeometryIndexed<Vertex2D>& geometry, usize instanceCount = 0);
            void StoreGeometryVertexed(const string& str, const GeometryVertexed<Vertex2D>& geometry, usize instanceCount = 0);

            void BeginBatch(const string& str);
            void BeginBatch(BatchVertexed2d& batchVertexed);
            void BeginBatch(BatchIndexed2d& batchIndexed);

            u32 AddInstance(const string& str, const RenderInstance2d& instance);

            void RemoveInstanceAt(const string& str, u32 index);

            void ClearInstances(const string& str);

            void FlushInstances(const string& str);

            void ReserveInstances(const string& str, const usize count);

            void ResizeInstances(const string& str, const usize count);

            void DrawBatch(const string& str);
            void DrawBatch(BatchVertexed2d& batchVertexed);
            void DrawBatch(BatchIndexed2d& batchIndexed);

            void DrawAll();

        private:

            void InitBatchIndexed(BatchIndexed2d& batchIndexed, const GeometryIndexedFormat& format, usize instanceCount);
            void InitBatchVertexed(BatchVertexed2d& batchVertexed, const GeometryVertexedFormat& format, usize instanceCount);

            void FreeBatchIndexed(BatchIndexed2d& batchIndexed);
            void FreeBatchVertexed(BatchVertexed2d& batchVertexed);

            BatchIndexed2d NewBatchIndexed(const GeometryIndexedFormat& format, usize instanceCount);
            BatchVertexed2d NewBatchVertexed(const GeometryVertexedFormat& format, usize instanceCount);

        private:
            Context* m_Context;
            vector<BatchIndexed2d> m_BatchesIndexed;
            vector<BatchVertexed2d> m_BatchesVertexed;
        };

    }

}