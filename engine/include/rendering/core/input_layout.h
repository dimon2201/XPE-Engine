#pragma once

#include <rendering/core/core.h>
#include <rendering/core/vertex.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sInputLayout final
        {
            void* Layout;
            sBlob* VertexBlob = nullptr;
            sVertexFormat Format;
        };

    }

}