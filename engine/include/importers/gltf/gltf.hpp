#pragma once

#include <geometry/mesh.h>

namespace xpe
{
    namespace gltf
    {

        using namespace math;

        struct ENGINE_API Buffer
        {
            int ByteLength;
            const char* URI;
            void* Data;
        };

        struct ENGINE_API BufferView
        {
            int BufferId;
            int ByteLength;
            int ByteOffset;
        };

        class ENGINE_API GLTFImporter final {

        public:
            static Model3D Import(const char* filepath);

        };

    }
}