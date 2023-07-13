#include <rendering/context.hpp>
#include <rendering/batching.h>

namespace xpe {

    namespace render {

        BatchManager::BatchManager(Context* context) : _context(context)
        {
            // todo move into vertex buffer structure ?
            _vertex.Type = eBufferType::VERTEX;
            _vertex.ByteSize = k_vertexBufferByteSize;
            _vertex.Duplicate = K_TRUE;
            _vertex.StructureSize = sizeof(Vertex3D);
            _context->CreateBuffer(_vertex);

            // todo move into index buffer structure ?
            _index.Type = eBufferType::INDEX;
            _index.ByteSize = k_indexBufferByteSize;
            _index.Duplicate = K_TRUE;
            _index.StructureSize = sizeof(u32);
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

            // todo move into StructureBuffer<RenderInstance> wrapper class?
            _instance2D.Type = eBufferType::STRUCTURED;
            _instance2D.ByteSize = k_instanceBufferByteSize;
            _instance2D.StructureSize = sizeof(RenderInstance2D);
            _instance2D.FirstElement = 0;
            _instance2D.NumElements = k_instanceBufferInstanceCount;
            _instance2D.Slot = K_SLOT_INSTANCES2D;
            _instance2D.Duplicate = K_TRUE;
            _context->CreateBuffer(_instance);
        }

        BatchManager::~BatchManager()
        {
            _context->FreeBuffer(_vertex);
            _context->FreeBuffer(_index);
            _context->FreeBuffer(_instance);
            _context->FreeBuffer(_instance2D);
        }

        void BatchManager::StoreGlobalGeometryData(const string& usid, const void* vertices, usize verticesByteSize, usize vertexCount, const void* indices, usize indicesByteSize, usize indexCount)
        {
            usize vertexOffset = _vertex.AppendOffset;
            usize indexOffset = _index.AppendOffset;

            _context->WriteBufferAppend(_vertex, vertices, verticesByteSize);
            _context->WriteBufferAppend(_index, indices, indicesByteSize);

            _geometryInstanceMap.insert({ usid, { { vertexOffset, indexOffset, vertexCount, indexCount } } });
        }

        void BatchManager::StoreGlobalGeometry2DData(const string& usid, const void* vertices, usize verticesByteSize, usize vertexCount, const void* indices, usize indicesByteSize, usize indexCount)
        {
            usize vertexOffset = _vertex.AppendOffset;
            usize indexOffset = _index.AppendOffset;

            _context->WriteBufferAppend(_vertex, vertices, verticesByteSize);
            _context->WriteBufferAppend(_index, indices, indicesByteSize);

            _geometry2DInstanceMap.insert({ usid, {{ vertexOffset, indexOffset, vertexCount, indexCount } } });
        }

        void BatchManager::BeginBatch(const string& geometryUSID)
        {
            auto it = _geometryInstanceMap.find(geometryUSID);
            if (it == _geometryInstanceMap.end())
            {
                return;
            }

            _batch.GeometryUSID = geometryUSID;
            _batch.Geometry = &it->second;
        }

        void BatchManager::BeginBatch2D(const string& geometryUSID)
        {
            auto it = _geometry2DInstanceMap.find(geometryUSID);
            if (it == _geometry2DInstanceMap.end())
            {
                return;
            }

            _batch.GeometryUSID = geometryUSID;
            _batch.Geometry2D = &it->second;
        }

        void BatchManager::RecordInstance(const RenderInstance& instance)
        {
            if (_batch.Geometry == nullptr)
            {
                return;
            }

            auto& geometryInstances = _batch.Geometry->Instances;

            ((RenderInstance*)_instance.CPUMemory)[geometryInstances.size()] = instance;
            geometryInstances.emplace_back(instance);
        }

        void BatchManager::RecordInstance2D(const RenderInstance2D& instance)
        {
            if (_batch.Geometry2D == nullptr)
            {
                return;
            }

            auto& geometryInstances = _batch.Geometry2D->Instances;

            ((RenderInstance2D*)_instance.CPUMemory)[geometryInstances.size()] = instance;
            geometryInstances.emplace_back(instance);
        }

        void BatchManager::AddInstance(const string& usid, const RenderInstance &instance)
        {
            if (_geometryInstanceMap.find(usid) == _geometryInstanceMap.end()) {
                GeometryInstances geometryInstances = { { 0, 0, 0, 0 } };
                _geometryInstanceMap.insert({ usid, geometryInstances });
            }
            _geometryInstanceMap.at(usid).Instances.emplace_back(instance);
        }

        void BatchManager::AddInstance2D(const string& usid, const RenderInstance2D &instance)
        {
            if (_geometry2DInstanceMap.find(usid) == _geometry2DInstanceMap.end()) {
                Geometry2DInstances geometryInstances = { { 0, 0, 0, 0 } };
                _geometry2DInstanceMap.insert({ usid, geometryInstances });
            }
            _geometry2DInstanceMap.at(usid).Instances.emplace_back(instance);
        }

        void BatchManager::RemoveInstance(const string& usid, const RenderInstance &instance)
        {
            if (_geometryInstanceMap.find(usid) != _geometryInstanceMap.end()) {
                auto& instanceList = _geometryInstanceMap.at(usid).Instances;
                auto it = std::find(instanceList.begin(), instanceList.end(), instance);
                if (it != instanceList.end()) {
                    instanceList.erase(it);
                }
            }
        }

        void BatchManager::RemoveInstance2D(const string& usid, const RenderInstance2D &instance)
        {
            if (_geometry2DInstanceMap.find(usid) != _geometry2DInstanceMap.end()) {
                auto& instanceList = _geometry2DInstanceMap.at(usid).Instances;
                auto it = std::find(instanceList.begin(), instanceList.end(), instance);
                if (it != instanceList.end()) {
                    instanceList.erase(it);
                }
            }
        }

        void BatchManager::ClearInstances(const string& usid)
        {
            if (_geometryInstanceMap.find(usid) != _geometryInstanceMap.end()) {
                _geometryInstanceMap.at(usid).Instances.clear();
            }
        }

        void BatchManager::ClearInstances2D(const string& usid)
        {
            if (_geometry2DInstanceMap.find(usid) != _geometry2DInstanceMap.end()) {
                _geometry2DInstanceMap.at(usid).Instances.clear();
            }
        }

        void BatchManager::EndBatch()
        {
            _context->WriteBuffer(
                    _instance,
                    _instance.CPUMemory,
                    sizeof(RenderInstance) * _batch.Geometry->Instances.size()
            );
        }

        void BatchManager::EndBatch2D()
        {
            _context->WriteBuffer(
                    _instance2D,
                    _instance2D.CPUMemory,
                    sizeof(RenderInstance2D) * _batch.Geometry2D->Instances.size()
            );
        }

        void BatchManager::DrawAll()
        {
            for (auto& instanceEntry : _geometryInstanceMap) {
                _batch.GeometryUSID = instanceEntry.first;
                _batch.Geometry = &instanceEntry.second;

                memcpy(
                        _instance.CPUMemory,
                        instanceEntry.second.Instances.data(),
                       instanceEntry.second.Instances.size() * sizeof(RenderInstance)
                );

                EndBatch();
                DrawBatch();
            }
        }

        void BatchManager::DrawAll2D()
        {
            for (auto& instanceEntry : _geometry2DInstanceMap) {
                _batch.GeometryUSID = instanceEntry.first;
                _batch.Geometry2D = &instanceEntry.second;

                memcpy(
                        _instance.CPUMemory,
                        instanceEntry.second.Instances.data(),
                        instanceEntry.second.Instances.size() * sizeof(RenderInstance)
                );

                EndBatch2D();
                DrawBatch2D();
            }
        }

        void BatchManager::DrawBatch()
        {
            auto& geometryInfo = _batch.Geometry->Info;
            auto& instances = _batch.Geometry->Instances;
            _context->DrawBatch(
                    geometryInfo.VertexOffset,
                    geometryInfo.IndexOffset,
                    geometryInfo.IndexCount,
                    instances.size()
            );
        }

        void BatchManager::DrawBatch2D()
        {
            auto& geometryInfo = _batch.Geometry2D->Info;
            auto& instances = _batch.Geometry2D->Instances;
            _context->DrawBatch(
                    geometryInfo.VertexOffset,
                    geometryInfo.IndexOffset,
                    geometryInfo.IndexCount,
                    instances.size()
            );
        }

    }

}