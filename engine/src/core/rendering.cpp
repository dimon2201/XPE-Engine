#include "rendering.hpp"
#include "memory_pool.hpp"

xpe::core::BatchManager::BatchManager(RenderingContext_Interface* context)
{
    _context = context;
    _vertex = _context->CreateBuffer(xBuffer::xType::VERTEX, k_vertexBufferByteSize, K_TRUE);
    _index = _context->CreateBuffer(xBuffer::xType::INDEX, k_indexBufferByteSize, K_TRUE);
    _instance = _context->CreateBuffer(xBuffer::xType::INSTANCE, k_instanceBufferByteSize, K_TRUE);
    _constant = _context->CreateBuffer(xBuffer::xType::CONSTANT, k_constantBufferByteSize, K_TRUE);
    _batch.GeometryInfo = nullptr;
}

xpe::core::BatchManager::~BatchManager()
{
    _context->FreeBuffer(_instance);
    _context->FreeBuffer(_index);
    _context->FreeBuffer(_vertex);
}

void xpe::core::BatchManager::StoreGlobalGeometryData(const std::string& usid, const void* vertices, usize verticesByteSize, usize vertexCount, const void* indices, usize indicesByteSize, usize indexCount)
{
    usize vertexOffset = _vertex.AppendOffset;
    usize indexOffset = _index.AppendOffset;

    _context->WriteBufferAppend(_vertex, vertices, verticesByteSize);
    _context->WriteBufferAppend(_index, indices, indicesByteSize);

    _geometries.insert({ usid, { vertexOffset, indexOffset, vertexCount, indexCount } });
}

void xpe::core::BatchManager::BeginBatch(const std::string& geometryUSID)
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

void xpe::core::BatchManager::RecordConstantBuffer(const xConstantBuffer* buffer)
{
    if (_batch.GeometryInfo == nullptr)
    {
        return;
    }

    memcpy(_constant.CPUMemory, buffer, sizeof(xConstantBuffer));
}

void xpe::core::BatchManager::RecordInstance(const xRenderInstance& instance)
{
    if (_batch.GeometryInfo == nullptr)
    {
        return;
    }

    ((xRenderInstance*)_instance.CPUMemory)[_batch.InstanceCount].Position = instance.Position;
    _batch.InstanceCount += 1;
}

void xpe::core::BatchManager::EndBatch()
{
    _context->WriteBuffer(_instance, _instance.CPUMemory, sizeof(xRenderInstance) * _batch.InstanceCount);
    _context->WriteBuffer(_constant, _constant.CPUMemory, k_constantBufferByteSize);
}

void xpe::core::BatchManager::DrawBatch()
{
    _context->DrawBatch(
        _batch.GeometryInfo->VertexBufferOffset,
        _batch.GeometryInfo->IndexBufferOffset,
        _batch.GeometryInfo->IndexCount,
        _batch.InstanceCount
    );
}