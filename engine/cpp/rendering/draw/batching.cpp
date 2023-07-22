#include <rendering/core/context.hpp>
#include <rendering/draw/batching.h>

namespace xpe {

    namespace render {

        // --------------------- Batching 3D ---------------------- //

        BatchManager::BatchManager(Context* context) : m_Context(context)
        {
        }

        BatchManager::~BatchManager()
        {
            for (auto& batchVertexed : m_BatchesVertexed)
            {
                FreeBatchVertexed(batchVertexed);
            }
            m_BatchesVertexed.clear();

            for (auto& batchIndexed : m_BatchesIndexed)
            {
                FreeBatchIndexed(batchIndexed);
            }
            m_BatchesIndexed.clear();
        }

        void BatchManager::InitBatchIndexed(BatchIndexed &batchIndexed, const GeometryIndexedFormat& format)
        {
            batchIndexed.Format = format;
            batchIndexed.Vertices = VertexBuffer<Vertex3D>(m_Context, format.VertexCount);
            batchIndexed.Indices = IndexBuffer(m_Context, format.IndexCount);
            batchIndexed.Instances = InstanceBuffer(m_Context, 1);
        }

        void BatchManager::InitBatchVertexed(BatchVertexed &batchVertexed, const GeometryVertexedFormat& format)
        {
            batchVertexed.Format = format;
            batchVertexed.Vertices = VertexBuffer<Vertex3D>(m_Context, format.VertexCount);
            batchVertexed.Instances = InstanceBuffer(m_Context, 1);
        }

        void BatchManager::FreeBatchIndexed(BatchIndexed &batchIndexed)
        {
            m_BatchIndexedLookup[batchIndexed.Format.USID] = nullptr;
            batchIndexed.Vertices.Free();
            batchIndexed.Indices.Free();
            batchIndexed.Instances.Free();
        }

        void BatchManager::FreeBatchVertexed(BatchVertexed &batchVertexed)
        {
            m_BatchVertexedLookup[batchVertexed.Format.USID] = nullptr;
            batchVertexed.Vertices.Free();
            batchVertexed.Instances.Free();
        }

        void BatchManager::NewBatchIndexed(const GeometryIndexedFormat& format)
        {
            BatchIndexed batchIndexed;
            InitBatchIndexed(batchIndexed, format);
            batchIndexed.Format = format;
            m_BatchesIndexed.emplace_back(batchIndexed);
            m_BatchIndexedLookup[format.USID] = &m_BatchesIndexed.back();
        }

        void BatchManager::NewBatchVertexed(const GeometryVertexedFormat& format)
        {
            BatchVertexed batchVertexed;
            InitBatchVertexed(batchVertexed, format);
            batchVertexed.Format = format;
            m_BatchesVertexed.emplace_back(batchVertexed);
            m_BatchVertexedLookup[format.USID] = &m_BatchesVertexed.back();
        }

        void BatchManager::StoreGeometryIndexed(const string &usid, const GeometryIndexed<Vertex3D>& geometry)
        {
            auto batchIndexed = m_BatchIndexedLookup.find(usid);

            if (batchIndexed == m_BatchIndexedLookup.end() || batchIndexed->second == nullptr) {
                GeometryIndexedFormat format;
                format.PrimitiveTopology = geometry.PrimitiveTopology;
                format.USID = usid;
                format.VertexOffset = 0;
                format.IndexOffset = 0;
                format.VertexCount = geometry.Vertices.Count();
                format.IndexCount = geometry.Indices.Count();
                NewBatchIndexed(format);
                batchIndexed = m_BatchIndexedLookup.find(usid);
            }

            batchIndexed->second->Vertices.FlushVertices(geometry.Vertices);
            batchIndexed->second->Indices.FlushIndices(geometry.Indices);
        }

        void BatchManager::StoreGeometryVertexed(const string &usid, const GeometryVertexed<Vertex3D>& geometry)
        {
            auto batchVertexed = m_BatchVertexedLookup.find(usid);

            if (batchVertexed == m_BatchVertexedLookup.end() || batchVertexed->second == nullptr) {
                GeometryVertexedFormat format;
                format.PrimitiveTopology = geometry.PrimitiveTopology;
                format.USID = usid;
                format.VertexOffset = 0;
                format.VertexCount = geometry.Vertices.Count();
                NewBatchVertexed(format);
                batchVertexed = m_BatchVertexedLookup.find(usid);
            }

            batchVertexed->second->Vertices.FlushVertices(geometry.Vertices);
        }

        void BatchManager::BeginBatch(const string& usid)
        {
            auto batchIndexed = m_BatchIndexedLookup.find(usid);

            if (batchIndexed == m_BatchIndexedLookup.end() || batchIndexed->second == nullptr) {

                auto batchVertexed = m_BatchVertexedLookup.find(usid);

                if (batchVertexed == m_BatchVertexedLookup.end() || batchVertexed->second == nullptr) {
                    LogWarning("Failed to begin batch for usid {}. It's absent", usid);
                    return;
                }

                else {
                    BeginBatch(*batchVertexed->second);
                }

            }

            else {
                BeginBatch(*batchIndexed->second);
            }
        }

        void BatchManager::BeginBatch(BatchVertexed &batchVertexed) {
            batchVertexed.Vertices.Bind();
            batchVertexed.Instances.Bind();
            m_Context->BindPrimitiveTopology(batchVertexed.Format.PrimitiveTopology);
        }

        void BatchManager::BeginBatch(BatchIndexed &batchIndexed) {
            batchIndexed.Vertices.Bind();
            batchIndexed.Indices.Bind();
            batchIndexed.Instances.Bind();
            m_Context->BindPrimitiveTopology(batchIndexed.Format.PrimitiveTopology);
        }

        bool BatchManager::AddInstance(const string& usid, const RenderInstance &instance)
        {
            auto batchIndexed = m_BatchIndexedLookup.find(usid);

            if (batchIndexed == m_BatchIndexedLookup.end() || batchIndexed->second == nullptr) {

                auto batchVertexed = m_BatchVertexedLookup.find(usid);

                if (batchVertexed == m_BatchVertexedLookup.end() || batchVertexed->second == nullptr) {
//                    LogWarning("Failed to add instance to batch {}. Batch is absent", usid);
                    return false;
                }

                else {
                    batchVertexed->second->Instances.GetList().emplace_back(instance);
                }

            }

            else {
                batchIndexed->second->Instances.GetList().emplace_back(instance);
            }

            return true;
        }

        void BatchManager::RemoveInstance(const string& usid, const RenderInstance &instance)
        {
            auto batchIndexed = m_BatchIndexedLookup.find(usid);

            if (batchIndexed == m_BatchIndexedLookup.end() || batchIndexed->second == nullptr) {

                auto batchVertexed = m_BatchVertexedLookup.find(usid);

                if (batchVertexed == m_BatchVertexedLookup.end() || batchVertexed->second == nullptr) {
                    LogWarning("Failed to remove instance from batch {}. Batch is absent", usid);
                    return;
                }

                else {
                    auto& instanceList = batchVertexed->second->Instances.GetList();
                    auto it = std::find(instanceList.begin(), instanceList.end(), instance);
                    if (it != instanceList.end()) {
                        instanceList.erase(it);
                    }
                }

            }

            else {
                batchIndexed->second->Instances.GetList().emplace_back(instance);
            }
        }

        void BatchManager::ClearInstances(const string& usid)
        {
            auto batchIndexed = m_BatchIndexedLookup.find(usid);

            if (batchIndexed == m_BatchIndexedLookup.end() || batchIndexed->second == nullptr) {

                auto batchVertexed = m_BatchVertexedLookup.find(usid);

                if (batchVertexed == m_BatchVertexedLookup.end() || batchVertexed->second == nullptr) {
                    LogWarning("Failed to clear instances of batch {}. Batch is absent", usid);
                    return;
                }

                else {
                    batchVertexed->second->Instances.GetList().clear();
                }

            }

            else {
                batchIndexed->second->Instances.GetList().clear();
            }
        }

        void BatchManager::FlushInstances(const string &usid)
        {
            auto batchIndexed = m_BatchIndexedLookup.find(usid);

            if (batchIndexed == m_BatchIndexedLookup.end() || batchIndexed->second == nullptr) {

                auto batchVertexed = m_BatchVertexedLookup.find(usid);

                if (batchVertexed == m_BatchVertexedLookup.end() || batchVertexed->second == nullptr) {
                    LogWarning("Failed to flush instances of batch {}. Batch is absent", usid);
                    return;
                }

                else {
                    batchVertexed->second->Instances.Flush();
                }

            }

            else {
                batchIndexed->second->Instances.Flush();
            }
        }

        void BatchManager::ReserveInstances(const string &usid, const usize count) {
            auto batchIndexed = m_BatchIndexedLookup.find(usid);

            if (batchIndexed == m_BatchIndexedLookup.end() || batchIndexed->second == nullptr) {

                auto batchVertexed = m_BatchVertexedLookup.find(usid);

                if (batchVertexed == m_BatchVertexedLookup.end() || batchVertexed->second == nullptr) {
                    LogWarning("Failed to reserve instances for batch {}. Batch is absent", usid);
                    return;
                }

                else {
                    batchVertexed->second->Instances.Reserve(count);
                }

            }

            else {
                batchIndexed->second->Instances.Reserve(count);
            }
        }

        void BatchManager::DrawBatch(const string &usid)
        {
            auto batchIndexed = m_BatchIndexedLookup.find(usid);

            if (batchIndexed == m_BatchIndexedLookup.end() || batchIndexed->second == nullptr) {

                auto batchVertexed = m_BatchVertexedLookup.find(usid);

                if (batchVertexed == m_BatchVertexedLookup.end() || batchVertexed->second == nullptr) {
                    LogWarning("Failed to flush instances of batch {}. Batch is absent", usid);
                    return;
                }

                else {
                    DrawBatch(*batchVertexed->second);
                }

            }

            else {
                DrawBatch(*batchIndexed->second);
            }
        }

        void BatchManager::DrawBatch(BatchVertexed &batchVertexed)
        {
            auto& geometryInfo = batchVertexed.Format;
            auto& instances = batchVertexed.Instances;
            m_Context->DrawBatch(
                    geometryInfo.VertexOffset,
                    geometryInfo.VertexCount,
                    instances.Count()
            );
        }

        void BatchManager::DrawBatch(BatchIndexed &batchIndexed)
        {
            auto& geometryInfo = batchIndexed.Format;
            auto& instances = batchIndexed.Instances;
            m_Context->DrawBatch(
                    geometryInfo.VertexOffset,
                    geometryInfo.IndexOffset,
                    geometryInfo.IndexCount,
                    instances.Count()
            );
        }

        void BatchManager::DrawAll()
        {
            for (auto& batch : m_BatchesVertexed) {
                BeginBatch(batch);
                DrawBatch(batch);
            }

            for (auto& batch : m_BatchesIndexed) {
                BeginBatch(batch);
                DrawBatch(batch);
            }
        }

        // --------------------- Batching 2D ------------------------ //

        BatchManager2d::BatchManager2d(Context* context) : m_Context(context)
        {
        }

        BatchManager2d::~BatchManager2d()
        {
            for (auto& batchVertexed : m_BatchesVertexed)
            {
                FreeBatchVertexed(batchVertexed);
            }
            m_BatchesVertexed.clear();

            for (auto& batchIndexed : m_BatchesIndexed)
            {
                FreeBatchIndexed(batchIndexed);
            }
            m_BatchesIndexed.clear();
        }

        void BatchManager2d::InitBatchIndexed(BatchIndexed2d &batchIndexed, const GeometryIndexedFormat& format)
        {
            batchIndexed.Format = format;
            batchIndexed.Vertices = VertexBuffer<Vertex2D>(m_Context, format.VertexCount);
            batchIndexed.Indices = IndexBuffer(m_Context, format.IndexCount);
            batchIndexed.Instances = InstanceBuffer2d(m_Context, 1);
        }

        void BatchManager2d::InitBatchVertexed(BatchVertexed2d &batchVertexed, const GeometryVertexedFormat& format)
        {
            batchVertexed.Format = format;
            batchVertexed.Vertices = VertexBuffer<Vertex2D>(m_Context, format.VertexCount);
            batchVertexed.Instances = InstanceBuffer2d(m_Context, 1);
        }

        void BatchManager2d::FreeBatchIndexed(BatchIndexed2d &batchIndexed)
        {
            m_BatchIndexedLookup[batchIndexed.Format.USID] = nullptr;
            batchIndexed.Vertices.Free();
            batchIndexed.Indices.Free();
            batchIndexed.Instances.Free();
        }

        void BatchManager2d::FreeBatchVertexed(BatchVertexed2d &batchVertexed)
        {
            m_BatchVertexedLookup[batchVertexed.Format.USID] = nullptr;
            batchVertexed.Vertices.Free();
            batchVertexed.Instances.Free();
        }

        void BatchManager2d::NewBatchIndexed(const GeometryIndexedFormat& format)
        {
            BatchIndexed2d batchIndexed;
            InitBatchIndexed(batchIndexed, format);
            m_BatchesIndexed.emplace_back(batchIndexed);
            m_BatchIndexedLookup[format.USID] = &m_BatchesIndexed[m_BatchesIndexed.size() - 1];
        }

        void BatchManager2d::NewBatchVertexed(const GeometryVertexedFormat& format)
        {
            BatchVertexed2d batchVertexed;
            InitBatchVertexed(batchVertexed, format);
            m_BatchesVertexed.emplace_back(batchVertexed);
            m_BatchVertexedLookup[format.USID] = &m_BatchesVertexed[m_BatchesVertexed.size() - 1];
        }

        void BatchManager2d::StoreGeometryIndexed(const string &usid, const GeometryIndexed<Vertex2D>& geometry)
        {
            auto batchIndexed = m_BatchIndexedLookup.find(usid);

            if (batchIndexed == m_BatchIndexedLookup.end() || batchIndexed->second == nullptr) {
                GeometryIndexedFormat format;
                format.PrimitiveTopology = geometry.PrimitiveTopology;
                format.USID = usid;
                format.VertexOffset = 0;
                format.IndexOffset = 0;
                format.VertexCount = geometry.Vertices.Count();
                format.IndexCount = geometry.Indices.Count();
                NewBatchIndexed(format);
                batchIndexed = m_BatchIndexedLookup.find(usid);
            }

            batchIndexed->second->Vertices.FlushVertices(geometry.Vertices);
            batchIndexed->second->Indices.FlushIndices(geometry.Indices);
        }

        void BatchManager2d::StoreGeometryVertexed(const string &usid, const GeometryVertexed<Vertex2D>& geometry)
        {
            auto batchVertexed = m_BatchVertexedLookup.find(usid);

            if (batchVertexed == m_BatchVertexedLookup.end() || batchVertexed->second == nullptr) {
                GeometryVertexedFormat format;
                format.PrimitiveTopology = geometry.PrimitiveTopology;
                format.USID = usid;
                format.VertexOffset = 0;
                format.VertexCount = geometry.Vertices.Count();
                NewBatchVertexed(format);
                batchVertexed = m_BatchVertexedLookup.find(usid);
            }

            batchVertexed->second->Vertices.FlushVertices(geometry.Vertices);
        }

        void BatchManager2d::BeginBatch(const string& usid)
        {
            auto batchIndexed = m_BatchIndexedLookup.find(usid);

            if (batchIndexed == m_BatchIndexedLookup.end() || batchIndexed->second == nullptr) {

                auto batchVertexed = m_BatchVertexedLookup.find(usid);

                if (batchVertexed == m_BatchVertexedLookup.end() || batchVertexed->second == nullptr) {
                    LogWarning("Failed to begin batch for usid {}. It's absent", usid);
                    return;
                }

                else {
                    BeginBatch(*batchVertexed->second);
                }

            }

            else {
                BeginBatch(*batchIndexed->second);
            }
        }

        void BatchManager2d::BeginBatch(BatchVertexed2d &batchVertexed) {
            batchVertexed.Vertices.Bind();
            batchVertexed.Instances.Bind();
            m_Context->BindPrimitiveTopology(batchVertexed.Format.PrimitiveTopology);
        }

        void BatchManager2d::BeginBatch(BatchIndexed2d &batchIndexed) {
            batchIndexed.Vertices.Bind();
            batchIndexed.Indices.Bind();
            batchIndexed.Instances.Bind();
            m_Context->BindPrimitiveTopology(batchIndexed.Format.PrimitiveTopology);
        }

        bool BatchManager2d::AddInstance(const string& usid, const RenderInstance2d &instance)
        {
            auto batchIndexed = m_BatchIndexedLookup.find(usid);

            if (batchIndexed == m_BatchIndexedLookup.end() || batchIndexed->second == nullptr) {

                auto batchVertexed = m_BatchVertexedLookup.find(usid);

                if (batchVertexed == m_BatchVertexedLookup.end() || batchVertexed->second == nullptr) {
                    LogWarning("Failed to add instance to batch {}. Batch is absent", usid);
                    return false;
                }

                else {
                    batchVertexed->second->Instances.GetList().emplace_back(instance);
                }

            }

            else {
                batchIndexed->second->Instances.GetList().emplace_back(instance);
            }

            return true;
        }

        void BatchManager2d::RemoveInstance(const string& usid, const RenderInstance2d &instance)
        {
            auto batchIndexed = m_BatchIndexedLookup.find(usid);

            if (batchIndexed == m_BatchIndexedLookup.end() || batchIndexed->second == nullptr) {

                auto batchVertexed = m_BatchVertexedLookup.find(usid);

                if (batchVertexed == m_BatchVertexedLookup.end() || batchVertexed->second == nullptr) {
                    LogWarning("Failed to remove instance from batch {}. Batch is absent", usid);
                    return;
                }

                else {
                    auto& instanceList = batchVertexed->second->Instances.GetList();
                    auto it = std::find(instanceList.begin(), instanceList.end(), instance);
                    if (it != instanceList.end()) {
                        instanceList.erase(it);
                    }
                }

            }

            else {
                batchIndexed->second->Instances.GetList().emplace_back(instance);
            }
        }

        void BatchManager2d::ClearInstances(const string& usid)
        {
            auto batchIndexed = m_BatchIndexedLookup.find(usid);

            if (batchIndexed == m_BatchIndexedLookup.end() || batchIndexed->second == nullptr) {

                auto batchVertexed = m_BatchVertexedLookup.find(usid);

                if (batchVertexed == m_BatchVertexedLookup.end() || batchVertexed->second == nullptr) {
                    LogWarning("Failed to clear instances of batch {}. Batch is absent", usid);
                    return;
                }

                else {
                    batchVertexed->second->Instances.GetList().clear();
                }

            }

            else {
                batchIndexed->second->Instances.GetList().clear();
            }
        }

        void BatchManager2d::FlushInstances(const string &usid)
        {
            auto batchIndexed = m_BatchIndexedLookup.find(usid);

            if (batchIndexed == m_BatchIndexedLookup.end() || batchIndexed->second == nullptr) {

                auto batchVertexed = m_BatchVertexedLookup.find(usid);

                if (batchVertexed == m_BatchVertexedLookup.end() || batchVertexed->second == nullptr) {
                    LogWarning("Failed to flush instances of batch {}. Batch is absent", usid);
                    return;
                }

                else {
                    batchVertexed->second->Instances.Flush();
                }

            }

            else {
                batchIndexed->second->Instances.Flush();
            }
        }

        void BatchManager2d::ReserveInstances(const string &usid, const usize count) {
            auto batchIndexed = m_BatchIndexedLookup.find(usid);

            if (batchIndexed == m_BatchIndexedLookup.end() || batchIndexed->second == nullptr) {

                auto batchVertexed = m_BatchVertexedLookup.find(usid);

                if (batchVertexed == m_BatchVertexedLookup.end() || batchVertexed->second == nullptr) {
                    LogWarning("Failed to reserve instances for batch {}. Batch is absent", usid);
                    return;
                }

                else {
                    batchVertexed->second->Instances.Reserve(count);
                }

            }

            else {
                batchIndexed->second->Instances.Reserve(count);
            }
        }

        void BatchManager2d::DrawBatch(const string &usid)
        {
            auto batchIndexed = m_BatchIndexedLookup.find(usid);

            if (batchIndexed == m_BatchIndexedLookup.end() || batchIndexed->second == nullptr) {

                auto batchVertexed = m_BatchVertexedLookup.find(usid);

                if (batchVertexed == m_BatchVertexedLookup.end() || batchVertexed->second == nullptr) {
                    LogWarning("Failed to flush instances of batch {}. Batch is absent", usid);
                    return;
                }

                else {
                    DrawBatch(*batchVertexed->second);
                }

            }

            else {
                DrawBatch(*batchIndexed->second);
            }
        }

        void BatchManager2d::DrawBatch(BatchVertexed2d &batchVertexed)
        {
            auto& geometryInfo = batchVertexed.Format;
            auto& instances = batchVertexed.Instances;
            m_Context->DrawBatch(
                    geometryInfo.VertexOffset,
                    geometryInfo.VertexCount,
                    instances.Count()
            );
        }

        void BatchManager2d::DrawBatch(BatchIndexed2d &batchIndexed)
        {
            auto& geometryInfo = batchIndexed.Format;
            auto& instances = batchIndexed.Instances;
            m_Context->DrawBatch(
                    geometryInfo.VertexOffset,
                    geometryInfo.IndexOffset,
                    geometryInfo.IndexCount,
                    instances.Count()
            );
        }

        void BatchManager2d::DrawAll()
        {
            for (auto& batch : m_BatchesVertexed) {
                BeginBatch(batch);
                DrawBatch(batch);
            }

            for (auto& batch : m_BatchesIndexed) {
                BeginBatch(batch);
                DrawBatch(batch);
            }
        }

    }

}