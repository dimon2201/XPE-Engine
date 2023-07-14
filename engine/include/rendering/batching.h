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
            static const usize K_VERTICES_BYTE_SIZE = K_MEMORY_MIB;
            static const usize K_INDICES_BYTE_SIZE = K_MEMORY_MIB;
            static const usize K_INSTANCE_COUNT = 1000000;

            BatchManager(Context* context);
            ~BatchManager();

            void StoreGeometryIndexed(const string& usid, const GeometryIndexed<Vertex3D>& geometry);
            void StoreGeometryVertexed(const string& usid, const GeometryVertexed<Vertex3D>& geometry);

            void BeginBatch(const string& geometryUSID);
            void BeginBatch(BatchVertexed& batchVertexed);
            void BeginBatch(BatchIndexed& batchIndexed);

            bool AddInstance(const string& usid, const RenderInstance& instance);

            void RemoveInstance(const string& usid, const RenderInstance& instance);

            void ClearInstances(const string& usid);

            void FlushInstances(const string& usid);

            void DrawBatch(const string& usid);
            void DrawBatch(BatchVertexed& batchVertexed);
            void DrawBatch(BatchIndexed& batchIndexed);

            void DrawAll();

        private:

            void InitBatchIndexed(BatchIndexed& batchIndexed, const GeometryIndexedFormat& format);
            void InitBatchVertexed(BatchVertexed& batchVertexed, const GeometryVertexedFormat& format);

            void FreeBatchIndexed(BatchIndexed& batchIndexed);
            void FreeBatchVertexed(BatchVertexed& batchVertexed);

            void NewBatchIndexed(const GeometryIndexedFormat& format);
            void NewBatchVertexed(const GeometryVertexedFormat& format);

        private:
            Context* m_Context;
            unordered_map<string, BatchIndexed*> m_BatchIndexedLookup;
            unordered_map<string, BatchVertexed*> m_BatchVertexedLookup;
            vector<BatchIndexed> m_BatchesIndexed;
            vector<BatchVertexed> m_BatchesVertexed;
        };

        class ENGINE_API BatchManager2d final
        {

        public:
            static const usize K_VERTICES_BYTE_SIZE = K_MEMORY_MIB;
            static const usize K_INDICES_BYTE_SIZE = K_MEMORY_MIB;
            static const usize K_INSTANCE_COUNT = 1000000;

            BatchManager2d(Context* context);
            ~BatchManager2d();

            void StoreGeometryIndexed(const string& usid, const GeometryIndexed<Vertex2D>& geometry);
            void StoreGeometryVertexed(const string& usid, const GeometryVertexed<Vertex2D>& geometry);

            void BeginBatch(const string& geometryUSID);
            void BeginBatch(BatchVertexed2d& batchVertexed);
            void BeginBatch(BatchIndexed2d& batchIndexed);

            bool AddInstance(const string& usid, const RenderInstance2d& instance);

            void RemoveInstance(const string& usid, const RenderInstance2d& instance);

            void ClearInstances(const string& usid);

            void FlushInstances(const string& usid);

            void DrawBatch(const string& usid);
            void DrawBatch(BatchVertexed2d& batchVertexed);
            void DrawBatch(BatchIndexed2d& batchIndexed);

            void DrawAll();

        private:

            void InitBatchIndexed(BatchIndexed2d& batchIndexed, const GeometryIndexedFormat& format);
            void InitBatchVertexed(BatchVertexed2d& batchVertexed, const GeometryVertexedFormat& format);

            void FreeBatchIndexed(BatchIndexed2d& batchIndexed);
            void FreeBatchVertexed(BatchVertexed2d& batchVertexed);

            void NewBatchIndexed(const GeometryIndexedFormat& format);
            void NewBatchVertexed(const GeometryVertexedFormat& format);

        private:
            Context* m_Context;
            unordered_map<string, BatchIndexed2d*> m_BatchIndexedLookup;
            unordered_map<string, BatchVertexed2d*> m_BatchVertexedLookup;
            vector<BatchIndexed2d> m_BatchesIndexed;
            vector<BatchVertexed2d> m_BatchesVertexed;
        };

    }

}