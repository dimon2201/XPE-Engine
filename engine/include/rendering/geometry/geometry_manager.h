#pragma once

#include <rendering/geometry/geometries.h>

namespace xpe {

    namespace render {

        class ENGINE_API cGeometryManager final
        {

        public:
            static void Init();
            static void Free();
            static void Bind();
            static sGeometry AddGeometry(const sGeometry& _geometry);

        private:
            static sVertexBuffer* s_VertexBuffer;
            static sIndexBuffer* s_IndexBuffer;
        };

    }

}