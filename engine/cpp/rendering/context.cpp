#include <rendering/context.hpp>

namespace xpe {

    namespace render {

        Buffer Context::CreateBuffer(const eBufferType &bufferType, usize byteSize, boolean duplicate) {
            Buffer buffer;
            CreateBuffer(buffer, bufferType, byteSize, duplicate);
            return buffer;
        }

    }

}
