#pragma once

#include <rendering/core.h>
#include <rendering/geometry.h>

namespace xpe {

    namespace render {

        using namespace xpe::core;

        class Context;

        struct ENGINE_API Batch final {
            std::string GeometryUSID;
            GeometryInfo* GeometryInfo;
            usize InstanceCount;
        };

        struct ENGINE_API RenderInstance final {
            glm::vec4 Position;
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

            void StoreGlobalGeometryData(const std::string& usid, const void* vertices, usize verticesByteSize, usize vertexCount, const void* indices, usize indicesByteSize, usize indexCount);
            void BeginBatch(const std::string& geometryUSID);
            void RecordConstantBuffer(const ConstantBuffer* buffer);
            void RecordInstance(const RenderInstance& instance);
            void EndBatch();
            void DrawBatch();

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
            std::map<std::string, GeometryInfo> _geometries;
            Batch _batch;
        };

    }

}