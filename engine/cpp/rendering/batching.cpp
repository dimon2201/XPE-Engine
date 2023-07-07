#include <rendering/context.hpp>
#include <rendering/batching.h>

namespace xpe {

    namespace render {

        BatchManager::BatchManager(Context* context) : _context(context)
        {
            _vertex.Type = eBufferType::VERTEX;
            _vertex.ByteSize = k_vertexBufferByteSize;
            _context->CreateBuffer(_vertex, K_TRUE);

            _index.Type = eBufferType::INDEX;
            _index.ByteSize = k_indexBufferByteSize;
            _context->CreateBuffer(_index, K_TRUE);

            _instance.Type = eBufferType::STRUCTURED;
            _instance.ByteSize = k_instanceBufferByteSize;
            _instance.StructureSize = sizeof(RenderInstance);
            _instance.FirstElement = 0;
            _instance.NumElements = k_instanceBufferInstanceCount;
            _context->CreateBuffer(_instance, K_TRUE);

            _constant.Type = eBufferType::CONSTANT;
            _constant.ByteSize = k_constantBufferByteSize;
            _context->CreateBuffer(_constant, K_TRUE);

            _batch.GeometryInfo = nullptr;
        }

        BatchManager::~BatchManager()
        {
            _context->FreeBuffer(_instance);
            _context->FreeBuffer(_index);
            _context->FreeBuffer(_vertex);
        }

        void BatchManager::StoreGlobalGeometryData(const std::string& usid, const void* vertices, usize verticesByteSize, usize vertexCount, const void* indices, usize indicesByteSize, usize indexCount)
        {
            usize vertexOffset = _vertex.AppendOffset;
            usize indexOffset = _index.AppendOffset;

            _context->WriteBufferAppend(_vertex, vertices, verticesByteSize);
            _context->WriteBufferAppend(_index, indices, indicesByteSize);

            _geometries.insert({ usid, { vertexOffset, indexOffset, vertexCount, indexCount } });
        }

        void BatchManager::BeginBatch(const std::string& geometryUSID)
        {
            _batch.GeometryUSID = geometryUSID;

            auto it = _geometries.find(_batch.GeometryUSID);
            if (it == _geometries.end())
            {
                return;
            }

            _batch.GeometryInfo = &it->second;
            _batch.InstanceCount = 0;
        }

        void BatchManager::RecordConstantBuffer(const ConstantBuffer* buffer)
        {
            if (_batch.GeometryInfo == nullptr)
            {
                return;
            }

            memcpy(_constant.CPUMemory, buffer, sizeof(ConstantBuffer));
        }

        void BatchManager::RecordInstance(const RenderInstance& instance)
        {
            if (_batch.GeometryInfo == nullptr)
            {
                return;
            }

            ((RenderInstance*)_instance.CPUMemory)[_batch.InstanceCount].Position = instance.Position;
            _batch.InstanceCount += 1;
        }

        void BatchManager::EndBatch()
        {
            _context->WriteBuffer(_instance, _instance.CPUMemory, sizeof(RenderInstance) * _batch.InstanceCount);
            _context->WriteBuffer(_constant, _constant.CPUMemory, k_constantBufferByteSize);
        }

        void BatchManager::DrawBatch()
        {
            _context->DrawBatch(
                    _batch.GeometryInfo->VertexBufferOffset,
                    _batch.GeometryInfo->IndexBufferOffset,
                    _batch.GeometryInfo->IndexCount,
                    _batch.InstanceCount
            );
        }

    }

}