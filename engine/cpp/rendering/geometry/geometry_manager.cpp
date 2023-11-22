#include <rendering/geometry/geometry_manager.h>

namespace xpe {

    namespace render {

        sVertexBuffer* cGeometryManager::s_VertexBuffer = nullptr;
        sIndexBuffer* cGeometryManager::s_IndexBuffer = nullptr;

        void cGeometryManager::Init()
        {
            s_VertexBuffer = new sVertexBuffer();
            s_IndexBuffer = new sIndexBuffer();
        }

        void cGeometryManager::Free()
        {
            delete s_VertexBuffer;
            delete s_IndexBuffer;
        }

        void cGeometryManager::Bind()
        {
            context::BindVertexBuffer(*s_VertexBuffer);
            context::BindIndexBuffer(*s_IndexBuffer);
        }

        sGeometry cGeometryManager::AddGeometry(const sGeometry& _geometry)
        {
            sGeometry geometry(_geometry);
            geometry.VertexOffset = s_VertexBuffer->AddVertices(geometry.Vertices);
            geometry.IndexOffset = s_IndexBuffer->AddIndices(geometry.Indices);
            s_VertexBuffer->Flush();
            s_IndexBuffer->Flush();
            return geometry;
        }

    }

}