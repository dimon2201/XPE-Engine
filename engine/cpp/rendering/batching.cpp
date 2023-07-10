#include <rendering/context.hpp>
#include <rendering/batching.h>

namespace xpe {

    namespace render {

        BatchManager::BatchManager(Context* context) : _context(context)
        {
            _vertex.Type = eBufferType::VERTEX;
            _vertex.ByteSize = k_vertexBufferByteSize;
            _vertex.Duplicate = K_TRUE;
            _context->CreateBuffer(_vertex);

            _index.Type = eBufferType::INDEX;
            _index.ByteSize = k_indexBufferByteSize;
            _index.Duplicate = K_TRUE;
            _context->CreateBuffer(_index);

            // todo move into StructureBuffer<RenderInstance> wrapper class?
            _instance.Type = eBufferType::STRUCTURED;
            _instance.ByteSize = k_instanceBufferByteSize;
            _instance.StructureSize = sizeof(RenderInstance);
            _instance.FirstElement = 0;
            _instance.NumElements = k_instanceBufferInstanceCount;
            _instance.Slot = K_SLOT_INSTANCES;
            _instance.Duplicate = K_TRUE;
            _context->CreateBuffer(_instance);

            _constant.Type = eBufferType::CONSTANT;
            _constant.ByteSize = k_constantBufferByteSize;
            _constant.Duplicate = K_TRUE;
            _context->CreateBuffer(_constant);

            _batch.GeometryInfo = nullptr;
        }

        BatchManager::~BatchManager()
        {
            _context->FreeBuffer(_instance);
            _context->FreeBuffer(_index);
            _context->FreeBuffer(_vertex);
        }

        void BatchManager::StoreGlobalGeometryData(const string& usid, const void* vertices, usize verticesByteSize, usize vertexCount, const void* indices, usize indicesByteSize, usize indexCount)
        {
            usize vertexOffset = _vertex.AppendOffset;
            usize indexOffset = _index.AppendOffset;

            _context->WriteBufferAppend(_vertex, vertices, verticesByteSize);
            _context->WriteBufferAppend(_index, indices, indicesByteSize);

            _geometryInstanceMap.insert({usid, {{vertexOffset, indexOffset, vertexCount, indexCount } } });
        }

        void BatchManager::BeginBatch(const string& geometryUSID)
        {
            auto it = _geometryInstanceMap.find(geometryUSID);
            if (it == _geometryInstanceMap.end())
            {
                return;
            }

            _batch.GeometryUSID = geometryUSID;
            _batch.GeometryInfo = &it->second.Info;
            _batch.Instances = &it->second.Instances;
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

            ((RenderInstance*)_instance.CPUMemory)[_batch.Instances->size()].Position = instance.Position;
            _batch.Instances->emplace_back(instance);
        }

        void BatchManager::AddInstance(const string& usid, const RenderInstance &instance) {
            if (_geometryInstanceMap.find(usid) == _geometryInstanceMap.end()) {
                GeometryInstances geometryInstances = { { 0, 0, 0, 0 } };
                _geometryInstanceMap.insert({ usid, geometryInstances });
            }
            _geometryInstanceMap.at(usid).Instances.emplace_back(instance);
        }

        void BatchManager::RemoveInstance(const string& usid, const RenderInstance &instance) {
            if (_geometryInstanceMap.find(usid) != _geometryInstanceMap.end()) {
                auto& instanceList = _geometryInstanceMap.at(usid).Instances;
                auto it = std::find(instanceList.begin(), instanceList.end(), instance);
                if (it != instanceList.end()) {
                    instanceList.erase(it);
                }
            }
        }

        void BatchManager::ClearInstances(const string& usid) {
            if (_geometryInstanceMap.find(usid) != _geometryInstanceMap.end()) {
                _geometryInstanceMap.at(usid).Instances.clear();
            }
        }

        void BatchManager::EndBatch()
        {
            _context->WriteBuffer(_instance, _instance.CPUMemory, sizeof(RenderInstance) * _batch.Instances->size());
            _context->WriteBuffer(_constant, _constant.CPUMemory, k_constantBufferByteSize);
        }

        void BatchManager::DrawAll() {
            for (auto& instanceEntry : _geometryInstanceMap) {
                _batch.GeometryUSID = instanceEntry.first;
                _batch.GeometryInfo = &instanceEntry.second.Info;

                auto& instanceList = instanceEntry.second.Instances;
                _batch.Instances = &instanceList;
                memcpy(_instance.CPUMemory, instanceList.data(), instanceList.size() * sizeof(RenderInstance));

                EndBatch();
                DrawBatch();
            }
        }

        void BatchManager::DrawBatch()
        {
            _context->DrawBatch(
                    _batch.GeometryInfo->VertexBufferOffset,
                    _batch.GeometryInfo->IndexBufferOffset,
                    _batch.GeometryInfo->IndexCount,
                    _batch.Instances->size()
            );
        }

    }

}