#pragma once

#include <rendering/core.h>
#include <rendering/geometry.h>

namespace xpe {

    namespace render {

        using namespace xpe::core;

        class Context;

        struct ENGINE_API RenderInstance final {

            u32 TransformIndex = 0;
            u32 CameraIndex = 0;
            u32 MaterialIndex = 0;

            friend inline bool operator ==(const RenderInstance& instance1, const RenderInstance& instance2) {
                return instance1.TransformIndex == instance2.TransformIndex;
            }

        };

        struct ENGINE_API GeometryInstances final {
            GeometryInfo Info;
            vector<RenderInstance> Instances;

            GeometryInstances(const GeometryInfo& info, const vector<RenderInstance>& instances = {})
            : Info(info), Instances(instances) {}
        };

        struct ENGINE_API Batch final {
            string GeometryUSID;
            GeometryInfo* GeometryInfo;
            vector<RenderInstance>* Instances;
        };

        struct ENGINE_API ConstantBuffer final {};

        class ENGINE_API BatchManager final {

        public:
            static const usize k_vertexBufferByteSize = K_MEMORY_MIB;
            static const usize k_indexBufferByteSize = K_MEMORY_MIB;
            static const usize k_instanceBufferInstanceCount = 1000000;
            static const usize k_instanceBufferByteSize = sizeof(RenderInstance) * k_instanceBufferInstanceCount;
            static const usize k_constantBufferByteSize = K_MEMORY_KIB;

            BatchManager(Context* context);
            ~BatchManager();

            void StoreGlobalGeometryData(const string& usid, const void* vertices, usize verticesByteSize, usize vertexCount, const void* indices, usize indicesByteSize, usize indexCount);
            void BeginBatch(const string& geometryUSID);
            void RecordConstantBuffer(const ConstantBuffer* buffer);
            void RecordInstance(const RenderInstance& instance);
            void AddInstance(const string& usid, const RenderInstance& instance);
            void RemoveInstance(const string& usid, const RenderInstance& instance);
            void ClearInstances(const string& usid);
            void EndBatch();
            void DrawBatch();
            void DrawAll();

            inline Buffer* GetVertexBuffer() { return &_vertex; }
            inline Buffer* GetIndexBuffer() { return &_index; }
            inline Buffer* GetInstanceBuffer() { return &_instance; }
            inline Buffer* GetConstantBuffer() { return &_constant; }

        private:
            Context* _context;
            Buffer _vertex;
            Buffer _index;
            Buffer _instance;
            Buffer _constant;
            Batch _batch;
            unordered_map<string, GeometryInstances> _geometryInstanceMap;
        };

    }

}