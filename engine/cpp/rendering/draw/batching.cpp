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

        void BatchManager::InitBatchIndexed(BatchIndexed &batchIndexed, const GeometryIndexedFormat& format, usize instanceCount)
        {
            batchIndexed.Format = format;
            batchIndexed.Vertices = VertexBuffer<Vertex3D>(m_Context, format.VertexCount);
            batchIndexed.Indices = IndexBuffer(m_Context, format.IndexCount);
            batchIndexed.Instances = InstanceBuffer(m_Context, instanceCount);
        }

        void BatchManager::InitBatchVertexed(BatchVertexed &batchVertexed, const GeometryVertexedFormat& format, usize instanceCount)
        {
            batchVertexed.Format = format;
            batchVertexed.Vertices = VertexBuffer<Vertex3D>(m_Context, format.VertexCount);
            batchVertexed.Instances = InstanceBuffer(m_Context, instanceCount);
        }

        void BatchManager::FreeBatchIndexed(BatchIndexed &batchIndexed)
        {
            batchIndexed.Vertices.Free();
            batchIndexed.Indices.Free();
            batchIndexed.Instances.Free();
        }

        void BatchManager::FreeBatchVertexed(BatchVertexed &batchVertexed)
        {
            batchVertexed.Vertices.Free();
            batchVertexed.Instances.Free();
        }

        BatchIndexed BatchManager::NewBatchIndexed(const GeometryIndexedFormat& format, usize instanceCount)
        {
            BatchIndexed batchIndexed;
            InitBatchIndexed(batchIndexed, format, instanceCount);
            batchIndexed.Format = format;
            m_BatchesIndexed.emplace_back(batchIndexed);
            return batchIndexed;
        }

        BatchVertexed BatchManager::NewBatchVertexed(const GeometryVertexedFormat& format, usize instanceCount)
        {
            BatchVertexed batchVertexed;
            InitBatchVertexed(batchVertexed, format, instanceCount);
            batchVertexed.Format = format;
            m_BatchesVertexed.emplace_back(batchVertexed);
            return batchVertexed;
        }

        void BatchManager::StoreGeometryIndexed(const string &str, const GeometryIndexed<Vertex3D>& geometry, usize instanceCount)
        {
            u64 usid = core::Hash(str);

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Vertices.FlushVertices(geometry.Vertices);
                    batchIndexed.Indices.FlushIndices(geometry.Indices);
                    return;
                }
            }

            GeometryIndexedFormat format;
            format.PrimitiveTopology = geometry.PrimitiveTopology;
            format.USID = usid;
            format.VertexOffset = 0;
            format.IndexOffset = 0;
            format.VertexCount = geometry.Vertices.Count();
            format.IndexCount = geometry.Indices.Count();

            BatchIndexed batchIndexed = NewBatchIndexed(format, instanceCount);

            batchIndexed.Vertices.FlushVertices(geometry.Vertices);
            batchIndexed.Indices.FlushIndices(geometry.Indices);
        }

        void BatchManager::StoreGeometryVertexed(const string& str, const GeometryVertexed<Vertex3D>& geometry, usize instanceCount)
        {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    batchVertexed.Vertices.FlushVertices(geometry.Vertices);
                    return;
                }
            }

            GeometryVertexedFormat format;
            format.PrimitiveTopology = geometry.PrimitiveTopology;
            format.USID = usid;
            format.VertexOffset = 0;
            format.VertexCount = geometry.Vertices.Count();

            BatchVertexed batchVertexed = NewBatchVertexed(format, instanceCount);

            batchVertexed.Vertices.FlushVertices(geometry.Vertices);
        }

        void BatchManager::BeginBatch(const string& str)
        {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    BeginBatch(batchVertexed);
                    return;
                }
            }

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    BeginBatch(batchIndexed);
                    return;
                }
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

        u32 BatchManager::AddInstance(const string& str, const RenderInstance &instance)
        {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    return batchVertexed.Instances.Add(instance);
                }
            }

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    return batchIndexed.Instances.Add(instance);
                }
            }

            return -1;
        }

        void BatchManager::RemoveInstanceAt(const string& str, u32 index)
        {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    batchVertexed.Instances.RemoveAt(index);
                    return;
                }
            }

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Instances.RemoveAt(index);
                    return;
                }
            }
        }

        void BatchManager::ClearInstances(const string& str)
        {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    batchVertexed.Instances.Clear();
                    return;
                }
            }

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Instances.Clear();
                    return;
                }
            }
        }

        void BatchManager::FlushInstances(const string &str)
        {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    batchVertexed.Instances.Flush();
                    return;
                }
            }

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Instances.Flush();
                    return;
                }
            }
        }

        void BatchManager::ResizeInstances(const string &str, const usize count) {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    batchVertexed.Instances.Resize(count);
                    return;
                }
            }

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Instances.Resize(count);
                    return;
                }
            }
        }

        void BatchManager::ReserveInstances(const string &str, const usize count) {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    batchVertexed.Instances.Reserve(count);
                    return;
                }
            }

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Instances.Reserve(count);
                    return;
                }
            }
        }

        void BatchManager::DrawBatch(const string &str)
        {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    DrawBatch(batchVertexed);
                    return;
                }
            }

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    DrawBatch(batchIndexed);
                    return;
                }
            }
        }

        void BatchManager::DrawBatch(BatchVertexed &batchVertexed)
        {
            auto& geometryInfo = batchVertexed.Format;
            auto& instances = batchVertexed.Instances;
            m_Context->DrawBatch(
                    geometryInfo.VertexOffset,
                    geometryInfo.VertexCount,
                    instances.Size()
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
                    instances.Size()
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

        void BatchManager2d::InitBatchIndexed(BatchIndexed2d &batchIndexed, const GeometryIndexedFormat& format, usize instanceCount)
        {
            batchIndexed.Format = format;
            batchIndexed.Vertices = VertexBuffer<Vertex2D>(m_Context, format.VertexCount);
            batchIndexed.Indices = IndexBuffer(m_Context, format.IndexCount);
            batchIndexed.Instances = InstanceBuffer2d(m_Context, instanceCount);
        }

        void BatchManager2d::InitBatchVertexed(BatchVertexed2d &batchVertexed, const GeometryVertexedFormat& format, usize instanceCount)
        {
            batchVertexed.Format = format;
            batchVertexed.Vertices = VertexBuffer<Vertex2D>(m_Context, format.VertexCount);
            batchVertexed.Instances = InstanceBuffer2d(m_Context, instanceCount);
        }

        void BatchManager2d::FreeBatchIndexed(BatchIndexed2d &batchIndexed)
        {
            batchIndexed.Vertices.Free();
            batchIndexed.Indices.Free();
            batchIndexed.Instances.Free();
        }

        void BatchManager2d::FreeBatchVertexed(BatchVertexed2d &batchVertexed)
        {
            batchVertexed.Vertices.Free();
            batchVertexed.Instances.Free();
        }

        BatchIndexed2d BatchManager2d::NewBatchIndexed(const GeometryIndexedFormat& format, usize instanceCount)
        {
            BatchIndexed2d batchIndexed;
            InitBatchIndexed(batchIndexed, format, instanceCount);
            batchIndexed.Format = format;
            m_BatchesIndexed.emplace_back(batchIndexed);
            return batchIndexed;
        }

        BatchVertexed2d BatchManager2d::NewBatchVertexed(const GeometryVertexedFormat& format, usize instanceCount)
        {
            BatchVertexed2d batchVertexed;
            InitBatchVertexed(batchVertexed, format, instanceCount);
            batchVertexed.Format = format;
            m_BatchesVertexed.emplace_back(batchVertexed);
            return batchVertexed;
        }

        void BatchManager2d::StoreGeometryIndexed(const string &str, const GeometryIndexed<Vertex2D>& geometry, usize instanceCount)
        {
            u64 usid = core::Hash(str);

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Vertices.FlushVertices(geometry.Vertices);
                    batchIndexed.Indices.FlushIndices(geometry.Indices);
                    return;
                }
            }

            GeometryIndexedFormat format;
            format.PrimitiveTopology = geometry.PrimitiveTopology;
            format.USID = usid;
            format.VertexOffset = 0;
            format.IndexOffset = 0;
            format.VertexCount = geometry.Vertices.Count();
            format.IndexCount = geometry.Indices.Count();

            BatchIndexed2d batchIndexed = NewBatchIndexed(format, instanceCount);

            batchIndexed.Vertices.FlushVertices(geometry.Vertices);
            batchIndexed.Indices.FlushIndices(geometry.Indices);
        }

        void BatchManager2d::StoreGeometryVertexed(const string& str, const GeometryVertexed<Vertex2D>& geometry, usize instanceCount)
        {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    batchVertexed.Vertices.FlushVertices(geometry.Vertices);
                    return;
                }
            }

            GeometryVertexedFormat format;
            format.PrimitiveTopology = geometry.PrimitiveTopology;
            format.USID = usid;
            format.VertexOffset = 0;
            format.VertexCount = geometry.Vertices.Count();

            BatchVertexed2d batchVertexed = NewBatchVertexed(format, instanceCount);

            batchVertexed.Vertices.FlushVertices(geometry.Vertices);
        }

        void BatchManager2d::BeginBatch(const string& str)
        {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    BeginBatch(batchVertexed);
                    return;
                }
            }

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    BeginBatch(batchIndexed);
                    return;
                }
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

        u32 BatchManager2d::AddInstance(const string& str, const RenderInstance2d &instance)
        {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    return batchVertexed.Instances.Add(instance);
                }
            }

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    return batchIndexed.Instances.Add(instance);
                }
            }

            return -1;
        }

        void BatchManager2d::RemoveInstanceAt(const string& str, u32 index)
        {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    batchVertexed.Instances.RemoveAt(index);
                    return;
                }
            }

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Instances.RemoveAt(index);
                    return;
                }
            }
        }

        void BatchManager2d::ClearInstances(const string& str)
        {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    batchVertexed.Instances.Clear();
                    return;
                }
            }

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Instances.Clear();
                    return;
                }
            }
        }

        void BatchManager2d::FlushInstances(const string &str)
        {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    batchVertexed.Instances.Flush();
                    return;
                }
            }

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Instances.Flush();
                    return;
                }
            }
        }

        void BatchManager2d::ResizeInstances(const string &str, const usize count) {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    batchVertexed.Instances.Resize(count);
                    return;
                }
            }

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Instances.Resize(count);
                    return;
                }
            }
        }

        void BatchManager2d::ReserveInstances(const string &str, const usize count) {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    batchVertexed.Instances.Reserve(count);
                    return;
                }
            }

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Instances.Reserve(count);
                    return;
                }
            }
        }

        void BatchManager2d::DrawBatch(const string &str)
        {
            u64 usid = core::Hash(str);

            for (auto& batchVertexed : m_BatchesVertexed) {
                if (batchVertexed.Format.USID == usid) {
                    DrawBatch(batchVertexed);
                    return;
                }
            }

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    DrawBatch(batchIndexed);
                    return;
                }
            }
        }

        void BatchManager2d::DrawBatch(BatchVertexed2d &batchVertexed)
        {
            auto& geometryInfo = batchVertexed.Format;
            auto& instances = batchVertexed.Instances;
            m_Context->DrawBatch(
                    geometryInfo.VertexOffset,
                    geometryInfo.VertexCount,
                    instances.Size()
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
                    instances.Size()
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

        // --------------------- Text Batching ---------------------- //

        TextBatchManager::TextBatchManager(Context* context) : m_Context(context)
        {
        }

        TextBatchManager::~TextBatchManager()
        {
            for (auto& batchIndexed : m_BatchesIndexed)
            {
                FreeBatchIndexed(batchIndexed);
            }
            m_BatchesIndexed.clear();
        }

        void TextBatchManager::InitBatchIndexed(BatchTextGlyphIndexed& batchIndexed, const GeometryIndexedFormat& format, usize instanceCount)
        {
            batchIndexed.Format = format;
            batchIndexed.Vertices = VertexBuffer<Vertex3D>(m_Context, format.VertexCount);
            batchIndexed.Indices = IndexBuffer(m_Context, format.IndexCount);
            batchIndexed.Instances = TextGlyphInstanceBuffer(m_Context, instanceCount);
        }

        void TextBatchManager::FreeBatchIndexed(BatchTextGlyphIndexed& batchIndexed)
        {
            batchIndexed.Indices.Free();
            batchIndexed.Instances.Free();
        }

        BatchTextGlyphIndexed TextBatchManager::NewBatchIndexed(const GeometryIndexedFormat& format, usize instanceCount)
        {
            BatchTextGlyphIndexed batchIndexed;
            InitBatchIndexed(batchIndexed, format, instanceCount);
            batchIndexed.Format = format;
            m_BatchesIndexed.emplace_back(batchIndexed);
            return batchIndexed;
        }

        void TextBatchManager::StoreGeometryIndexed(const string& str, const GeometryIndexed<Vertex3D>& geometry, usize instanceCount)
        {
            u64 usid = core::Hash(str);

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Vertices.FlushVertices(geometry.Vertices);
                    batchIndexed.Indices.FlushIndices(geometry.Indices);
                    return;
                }
            }

            GeometryIndexedFormat format;
            format.PrimitiveTopology = geometry.PrimitiveTopology;
            format.USID = usid;
            format.VertexOffset = 0;
            format.IndexOffset = 0;
            format.VertexCount = geometry.Vertices.Count();
            format.IndexCount = geometry.Indices.Count();

            BatchTextGlyphIndexed batchIndexed = NewBatchIndexed(format, instanceCount);

            batchIndexed.Vertices.FlushVertices(geometry.Vertices);
            batchIndexed.Indices.FlushIndices(geometry.Indices);
        }

        void TextBatchManager::BeginBatch(const string& str)
        {
            u64 usid = core::Hash(str);

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    BeginBatch(batchIndexed);
                    return;
                }
            }
        }

        void TextBatchManager::BeginBatch(BatchTextGlyphIndexed& batchIndexed) {
            batchIndexed.Indices.Bind();
            batchIndexed.Instances.Bind();
            m_Context->BindPrimitiveTopology(batchIndexed.Format.PrimitiveTopology);
        }

        u32 TextBatchManager::AddInstance(const string& str, const TextGlyphInstance& instance)
        {
            u64 usid = core::Hash(str);

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    return batchIndexed.Instances.Add(instance);
                }
            }

            return -1;
        }

        void TextBatchManager::ClearInstances(const string& str)
        {
            u64 usid = core::Hash(str);

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Instances.Clear();
                    return;
                }
            }
        }

        void TextBatchManager::FlushInstances(const string& str)
        {
            u64 usid = core::Hash(str);

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Instances.Flush();
                    return;
                }
            }
        }

        void TextBatchManager::ReserveInstances(const string& str, const usize count) {
            u64 usid = core::Hash(str);

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Instances.Reserve(count);
                    return;
                }
            }
        }

        void TextBatchManager::DrawBatch(const string& str)
        {
            u64 usid = core::Hash(str);

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    DrawBatch(batchIndexed);
                    return;
                }
            }
        }

        void TextBatchManager::DrawBatch(BatchTextGlyphIndexed& batchIndexed)
        {
            auto& geometryInfo = batchIndexed.Format;
            auto& instances = batchIndexed.Instances;
            m_Context->DrawBatch(
                geometryInfo.VertexOffset,
                geometryInfo.IndexOffset,
                geometryInfo.IndexCount,
                instances.Size()
            );
        }

        void TextBatchManager::DrawAll()
        {
            for (auto& batch : m_BatchesIndexed) {
                BeginBatch(batch);
                DrawBatch(batch);
            }
        }
    }

}