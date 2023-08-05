#include <rendering/core/context.hpp>
#include <rendering/text/text_batching.hpp>

namespace xpe {

    namespace render {

        // --------------------- Text Batching ---------------------- //

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
            batchIndexed.Vertices = VertexBuffer<Vertex3D>(format.VertexCount);
            batchIndexed.Indices = IndexBuffer(format.IndexCount);
            batchIndexed.Instances = TextGlyphInstanceBuffer(instanceCount);
        }

        void TextBatchManager::FreeBatchIndexed(BatchTextGlyphIndexed& batchIndexed)
        {
            batchIndexed.Vertices.Free();
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

        bool TextBatchManager::StoreGeometryIndexed(const string& str, const GeometryIndexed<Vertex3D>& geometry, usize instanceCount)
        {
            u64 usid = core::Hash(str);

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Vertices.FlushVertices(geometry.Vertices);
                    batchIndexed.Indices.FlushIndices(geometry.Indices);
                    return true;
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

            return true;
        }

        bool TextBatchManager::HasGeometryIndexed(const string &str) {
            u64 usid = core::Hash(str);

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    return true;
                }
            }

            return false;
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
            batchIndexed.Vertices.Bind();
            batchIndexed.Indices.Bind();
            batchIndexed.Instances.Bind();
            context::BindPrimitiveTopology(batchIndexed.Format.PrimitiveTopology);
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

        void TextBatchManager::RemoveInstanceAt(const string& str, u32 index)
        {
            u64 usid = core::Hash(str);

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Instances.RemoveAt(index);
                    return;
                }
            }
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

        void TextBatchManager::ResizeInstances(const string& str, const usize count) {
            u64 usid = core::Hash(str);

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    batchIndexed.Instances.Resize(count);
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

        usize TextBatchManager::GetInstanceCapacity(const string &str) {
            u64 usid = core::Hash(str);

            for (auto& batchIndexed : m_BatchesIndexed) {
                if (batchIndexed.Format.USID == usid) {
                    return batchIndexed.Instances.Capacity();
                }
            }

            return 0;
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
            context::DrawBatch(
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