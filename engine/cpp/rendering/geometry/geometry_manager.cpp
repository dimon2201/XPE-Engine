#include <rendering/geometry/geometry_manager.h>
#include <rendering/bindings.h>

namespace xpe {

    namespace render {

        void cGeometryManager::Init()
        {
            Buffers::Vertex = new sVertexBuffer();
            Buffers::Index  = new sIndexBuffer();
        }

        void cGeometryManager::Free()
        {
            delete Buffers::Vertex;
            delete Buffers::Index;
        }

        std::pair<sGeometry, sGeometryInfo> cGeometryManager::AddGeometry(const sGeometry& geometry)
        {
            sGeometryInfo geometryInfo;
            geometryInfo.PrimitiveTopology = geometry.PrimitiveTopology;
            geometryInfo.VertexOffset = Buffers::Vertex->AddVertices(geometry.Vertices);
            geometryInfo.IndexOffset = Buffers::Index->AddIndices(geometry.Indices);
            geometryInfo.VertexCount = geometry.Vertices.size();
            geometryInfo.IndexCount = geometry.Indices.size();
            Buffers::Vertex->Flush();
            Buffers::Index->Flush();
            return { geometry, geometryInfo };
        }

    }

}