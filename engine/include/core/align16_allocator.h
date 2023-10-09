#pragma once

namespace xpe {

    namespace core {

        ENGINE_API void* align16_malloc(usize size);
        ENGINE_API void  align16_free(void* address);

    }

}