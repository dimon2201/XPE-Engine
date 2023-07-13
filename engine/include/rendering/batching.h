#pragma once

#include <rendering/core.h>
#include <geometry/mesh.h>

namespace xpe {

    namespace render {

        using namespace xpe::core;
        using namespace xpe::math;

        class Context;

        struct ENGINE_API RenderInstance final
        {
            u32 TransformIndex = 0;
            u32 CameraIndex = 0;
            u32 MaterialIndex = 0;

            friend inline bool operator ==(const RenderInstance& instance1, const RenderInstance& instance2) {
                return instance1.TransformIndex == instance2.TransformIndex;
            }
        };

        struct ENGINE_API RenderInstance2D final
        {
            u32 TransformIndex = 0;
            u32 CameraIndex = 0;

            friend inline bool operator ==(const RenderInstance2D& instance1, const RenderInstance2D& instance2) {
                return instance1.TransformIndex == instance2.TransformIndex;
            }
        };

        struct ENGINE_API GeometryInstances final
        {
            GeometryInfo Info;
            vector<RenderInstance> Instances;

            GeometryInstances(const GeometryInfo& info, const vector<RenderInstance>& instances = {})
            : Info(info), Instances(instances) {}
        };

        struct ENGINE_API Geometry2DInstances final
        {
            GeometryInfo Info;
            vector<RenderInstance2D> Instances;

            Geometry2DInstances(const GeometryInfo& info, const vector<RenderInstance2D>& instances = {})
            : Info(info), Instances(instances) {}
        };

        struct ENGINE_API Batch final
        {
            string GeometryUSID;
            GeometryInstances* Geometry = nullptr;
            Geometry2DInstances* Geometry2D = nullptr;
        };

        struct ENGINE_API ConstantBuffer final {};

        class ENGINE_API BatchManager final {

        public:
            static const usize k_vertexBufferByteSize = K_MEMORY_MIB;
            static const usize k_indexBufferByteSize = K_MEMORY_MIB;
            static const usize k_instanceBufferInstanceCount = 1000000;
            static const usize k_instanceBufferByteSize = sizeof(RenderInstance) * k_instanceBufferInstanceCount;

            BatchManager(Context* context);
            ~BatchManager();

            void StoreGlobalGeometryData(const string& usid, const void* vertices, usize verticesByteSize, usize vertexCount, const void* indices, usize indicesByteSize, usize indexCount);
            void StoreGlobalGeometry2DData(const string& usid, const void* vertices, usize verticesByteSize, usize vertexCount, const void* indices, usize indicesByteSize, usize indexCount);

            void BeginBatch(const string& geometryUSID);
            void BeginBatch2D(const string& geometryUSID);

            void RecordInstance(const RenderInstance& instance);
            void RecordInstance2D(const RenderInstance2D& instance);

            bool AddInstance(const string& usid, const RenderInstance& instance);
            bool AddInstance2D(const string& usid, const RenderInstance2D& instance);

            void RemoveInstance(const string& usid, const RenderInstance& instance);
            void RemoveInstance2D(const string& usid, const RenderInstance2D& instance);

            void ClearInstances(const string& usid);
            void ClearInstances2D(const string& usid);

            void EndBatch();
            void EndBatch2D();

            void DrawBatch();
            void DrawBatch2D();

            void DrawAll();
            void DrawAll2D();

            inline Buffer* GetVertexBuffer() { return &_vertex; }
            inline Buffer* GetIndexBuffer() { return &_index; }
            inline Buffer* GetInstanceBuffer() { return &_instance; }
            inline Buffer* GetInstance2DBuffer() { return &_instance2D; }

        private:
            Context* _context;
            Buffer _vertex;
            Buffer _index;
            Buffer _instance;
            Buffer _instance2D;
            Batch _batch;
            unordered_map<string, GeometryInstances> _geometryInstanceMap;
            unordered_map<string, Geometry2DInstances> _geometry2DInstanceMap;
        };

    }

}