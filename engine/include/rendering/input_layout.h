#pragma once

#include <rendering/core.h>
#include <rendering/vertex.h>

namespace xpe {

    namespace render {

        struct ENGINE_API InputLayout final {
            GPUResource Layout;
            Blob* VertexBlob = nullptr;
            VertexFormat Format;
        };

    }

}