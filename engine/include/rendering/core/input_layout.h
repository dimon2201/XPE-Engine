#pragma once

#include <rendering/core/core.h>
#include <rendering/core/vertex.h>

namespace xpe {

    namespace render {

        struct ENGINE_API InputLayout final {
            void* Layout;
            Blob* VertexBlob = nullptr;
            VertexFormat Format;
        };

    }

}